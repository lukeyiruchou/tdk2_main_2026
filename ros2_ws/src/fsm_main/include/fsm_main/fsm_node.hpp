#ifndef FSM_NODE_HPP_
#define FSM_NODE_HPP_

#include <yaml-cpp/yaml.h>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/int32.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_srvs/srv/set_bool.hpp" // 新增：用於動態切換模式的 Service

#include "ament_index_cpp/get_package_share_directory.hpp"

// 維持底盤與 Nav2 介面
#include "chassis_pilot/action/navi_goal.hpp"
#include "chassis_pilot/msg/custom_waypoint.hpp"
#include "nav2_msgs/action/navigate_through_poses.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class BaseStateNode : public rclcpp_lifecycle::LifecycleNode {
public:
    using NaviGoal = chassis_pilot::action::NaviGoal;
    using GoalHandleNavi = rclcpp_action::ClientGoalHandle<NaviGoal>;

    using NavigateThroughPoses = nav2_msgs::action::NavigateThroughPoses;
    using GoalHandleNav2 = rclcpp_action::ClientGoalHandle<NavigateThroughPoses>;

    enum class PlannerType {
        CHASSIS_PILOT,
        NAV2
    };

    // 建構子：宣告參數，並註冊動態切換 Service
    explicit BaseStateNode(const std::string & node_name) 
    : LifecycleNode(node_name) {
        this->declare_parameter<bool>("is_mirrored", false);

        // 註冊切換服務：~/switch_mirror_mode
        switch_mirror_srv_ = this->create_service<std_srvs::srv::SetBool>(
            "~/switch_mirror_mode",
            std::bind(&BaseStateNode::switch_mirror_callback, this, std::placeholders::_1, std::placeholders::_2));
    }

    virtual ~BaseStateNode() = default;

    // --- Lifecycle 介面封裝 ---
    CallbackReturn on_configure(const rclcpp_lifecycle::State &) override {
        RCLCPP_INFO(get_logger(), "--- [BaseStateNode] 正在執行通用配置 ---");
        
        // 1. 讀取是否為鏡像場地參數
        is_mirrored_ = this->get_parameter("is_mirrored").as_bool();
        RCLCPP_INFO(get_logger(), "場地鏡像模式 (is_mirrored): [%s]", is_mirrored_ ? "開啟 (鏡像)" : "關閉 (一般)");

        // 2. 建立進度與狀態發布者
        feedback_pub_ = this->create_publisher<std_msgs::msg::Float32>("~/progress", 10);
        done_pub_ = this->create_publisher<std_msgs::msg::String>("/state_feedback", 10);

        arm_pub_    = this->create_publisher<std_msgs::msg::Int32>("robot/cmd_arm", 10);
        intake_pub_ = this->create_publisher<std_msgs::msg::Bool>("robot/cmd_intake", 10);
        claw_pub_   = this->create_publisher<std_msgs::msg::Bool>("robot/cmd_claw", 10);

        // 3. 建立訂閱者 (Subscriber)
        arm_status_sub_ = this->create_subscription<std_msgs::msg::Int32>(
            "robot/arm_status", 10,
            std::bind(&BaseStateNode::arm_status_callback, this, std::placeholders::_1));

        // 4. 建立 Action Clients
        action_client_ = rclcpp_action::create_client<NaviGoal>(this, "navi_goal");
        nav2_action_client_ = rclcpp_action::create_client<NavigateThroughPoses>(
            this, "navigate_through_poses");

        // 5. 根據 is_mirrored_ 載入對應的 YAML 檔案
        load_paths_from_yaml();
        
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_activate(const rclcpp_lifecycle::State &) override {
        feedback_pub_->on_activate();
        done_pub_->on_activate(); 

        arm_pub_->on_activate();
        intake_pub_->on_activate();
        claw_pub_->on_activate();
        
        is_path_navigating_ = false;
        is_path_arrived_ = false;

        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_deactivate(const rclcpp_lifecycle::State &) override {
        feedback_pub_->on_deactivate();
        done_pub_->on_deactivate(); 
        
        arm_pub_->on_deactivate();
        intake_pub_->on_deactivate();
        claw_pub_->on_deactivate();
        return CallbackReturn::SUCCESS;
    }

protected:
    // ----- 子類可直接讀取的變數 -----
    bool is_mirrored_{false};        // 當前是否為鏡像模式
    bool is_path_navigating_{false}; // 是否正在導航中
    bool is_path_arrived_{false};    // 是否已抵達終點
    int arm_status_{0};              // 當前手臂狀態 (由 /arm_status 更新)

    void send_progress(float progress) {
        auto msg = std_msgs::msg::Float32();
        msg.data = progress;
        feedback_pub_->publish(msg); 
    }

    void publish_status(const std::string & status) {
        RCLCPP_DEBUG(get_logger(), "State: %s", status.c_str());
    }

    void notify_manager_done() {
        auto msg = std::make_unique<std_msgs::msg::String>();
        msg->data = "done";
        done_pub_->publish(std::move(msg));
        RCLCPP_INFO(this->get_logger(), "Task Finished. Notifying Manager...");
    }

    void move_along_path(const std::string & path_name) {
        auto it = path_database_.find(path_name);
        if (it == path_database_.end()) {
            RCLCPP_ERROR(get_logger(), "找不到路徑代號: %s !! 拒絕發送移動請求。", path_name.c_str());
            is_path_arrived_ = true; 
            return;
        }

        const PathInfo & path_info = it->second;
        is_path_navigating_ = false;
        is_path_arrived_ = false;

        if (path_info.planner == PlannerType::NAV2) {
            move_via_nav2(path_name, path_info.waypoints);
        } else {
            move_via_chassis_pilot(path_name, path_info.waypoints);
        }
    }

    void set_arm_state(int script_id) {
        auto msg = std_msgs::msg::Int32();
        msg.data = script_id;
        arm_pub_->publish(msg);
        RCLCPP_INFO(get_logger(), "🦾 [BaseStateNode] 發送手臂腳本: [%d]", script_id);
    }

    void set_intake_state(bool deploy) {
        auto msg = std_msgs::msg::Bool();
        msg.data = deploy;
        intake_pub_->publish(msg);
        RCLCPP_INFO(get_logger(), "🌪️ [BaseStateNode] send intake state: [%s]", deploy ? "開啟" : "關閉");
    }

    void set_claw_state(bool close_claw) {
        auto msg = std_msgs::msg::Bool();
        msg.data = close_claw;
        claw_pub_->publish(msg);
        RCLCPP_INFO(get_logger(), "🗜️ [BaseStateNode] send claw state: [%s]", close_claw ? "閉合" : "張開");
    }

private:
    struct PathInfo {
        PlannerType planner{PlannerType::CHASSIS_PILOT};
        std::vector<chassis_pilot::msg::CustomWaypoint> waypoints;
    };

    // 動態切換鏡像模式回調函式
    void switch_mirror_callback(
        const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
        std::shared_ptr<std_srvs::srv::SetBool::Response> response)
    {
        // 安全檢查：若車輛正在導航移動中，禁止熱抽換路徑
        if (is_path_navigating_) {
            response->success = false;
            response->message = "警告：機器人正在導航移動中，拒絕切換鏡像模式！";
            RCLCPP_WARN(get_logger(), "%s", response->message.c_str());
            return;
        }

        is_mirrored_ = request->data;
        this->set_parameter(rclcpp::Parameter("is_mirrored", is_mirrored_));

        // 重新讀取對應的 YAML 檔案並更新資料庫
        load_paths_from_yaml();

        response->success = true;
        response->message = is_mirrored_ 
            ? "【切換成功】已切換為 [鏡像場地]，成功熱重載 path_mirrored.yaml！"
            : "【切換成功】已切換為 [一般場地]，成功熱重載 path.yaml！";
            
        RCLCPP_INFO(get_logger(), "%s", response->message.c_str());
    }

    void arm_status_callback(const std_msgs::msg::Int32::SharedPtr msg) {
        arm_status_ = msg->data;
    }

    void move_via_chassis_pilot(
        const std::string & path_name,
        const std::vector<chassis_pilot::msg::CustomWaypoint> & waypoints) 
    {
        if (!action_client_->wait_for_action_server(std::chrono::seconds(2))) {
            RCLCPP_ERROR(get_logger(), "底盤 Action 伺服器 (/navi_goal) 未上線！");
            return;
        }

        auto goal_msg = NaviGoal::Goal();
        goal_msg.trajectory = waypoints;
        goal_msg.max_angular_speed = 0.3;
        goal_msg.max_accel = 0.2;
        goal_msg.cruise_mode = false;

        auto send_goal_options = rclcpp_action::Client<NaviGoal>::SendGoalOptions();
        send_goal_options.goal_response_callback = 
            std::bind(&BaseStateNode::goal_response_callback, this, std::placeholders::_1);
        send_goal_options.feedback_callback = 
            std::bind(&BaseStateNode::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
        send_goal_options.result_callback = 
            std::bind(&BaseStateNode::result_callback, this, std::placeholders::_1);

        RCLCPP_INFO(get_logger(), "[chassis_pilot] 正在發送軌跡 Action，目標路線：[%s]", path_name.c_str());
        action_client_->async_send_goal(goal_msg, send_goal_options);
    }

    void move_via_nav2(
        const std::string & path_name,
        const std::vector<chassis_pilot::msg::CustomWaypoint> & waypoints) 
    {
        if (!nav2_action_client_->wait_for_action_server(std::chrono::seconds(2))) {
            RCLCPP_ERROR(get_logger(), "Nav2 Action 伺服器 (/navigate_through_poses) 未上線！");
            return;
        }

        auto goal_msg = NavigateThroughPoses::Goal();
        rclcpp::Time now = this->get_clock()->now();

        for (const auto & wp : waypoints) {
            geometry_msgs::msg::PoseStamped pose;
            pose.header.frame_id = "world";
            pose.header.stamp = now;
            pose.pose.position.x = wp.x;
            pose.pose.position.y = wp.y;
            pose.pose.position.z = 0.0;

            double half_yaw = static_cast<double>(wp.yaw) * 0.5;
            pose.pose.orientation.x = 0.0;
            pose.pose.orientation.y = 0.0;
            pose.pose.orientation.z = std::sin(half_yaw);
            pose.pose.orientation.w = std::cos(half_yaw);

            goal_msg.poses.push_back(pose);
        }

        auto send_goal_options = rclcpp_action::Client<NavigateThroughPoses>::SendGoalOptions();
        send_goal_options.goal_response_callback = 
            std::bind(&BaseStateNode::nav2_goal_response_callback, this, std::placeholders::_1);
        send_goal_options.feedback_callback = 
            std::bind(&BaseStateNode::nav2_feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
        send_goal_options.result_callback = 
            std::bind(&BaseStateNode::nav2_result_callback, this, std::placeholders::_1);

        RCLCPP_INFO(get_logger(), "[nav2] 正在發送 NavigateThroughPoses 目標，目標路線：[%s]（共 %zu 點）", 
            path_name.c_str(), waypoints.size());
        nav2_action_client_->async_send_goal(goal_msg, send_goal_options);
    }

    // 根據 is_mirrored_ 動態選擇檔案載入
    void load_paths_from_yaml() {
        path_database_.clear(); // 清空緩存，避免多次 configure 殘留
        
        try {
            std::string pkg_path = ament_index_cpp::get_package_share_directory("fsm_main");
            
            // 決定 YAML 檔名
            std::string file_name = is_mirrored_ ? "path_mirrored.yaml" : "path.yaml";
            std::string file_path = pkg_path + "/config/" + file_name;

            RCLCPP_INFO(get_logger(), "準備讀取路徑檔: %s", file_path.c_str());
            
            YAML::Node config = YAML::LoadFile(file_path);
            if (!config["paths"]) {
                RCLCPP_WARN(get_logger(), "YAML 檔案中未發現 'paths' 區塊: %s", file_path.c_str());
                return;
            }

            for (auto it = config["paths"].begin(); it != config["paths"].end(); ++it) {
                std::string path_name = it->first.as<std::string>();
                YAML::Node path_node = it->second;

                PathInfo path_info;
                YAML::Node points_node;

                if (path_node.IsMap() && path_node["points"]) {
                    std::string planner_str = path_node["planner"]
                        ? path_node["planner"].as<std::string>()
                        : "chassis_pilot";
                    path_info.planner = (planner_str == "nav2")
                        ? PlannerType::NAV2
                        : PlannerType::CHASSIS_PILOT;
                    points_node = path_node["points"];
                } else {
                    path_info.planner = PlannerType::CHASSIS_PILOT;
                    points_node = path_node;
                }

                for (size_t i = 0; i < points_node.size(); ++i) {
                    chassis_pilot::msg::CustomWaypoint wp;
                    wp.x = points_node[i]["x"].as<float>();
                    wp.y = points_node[i]["y"].as<float>();
                    wp.yaw = points_node[i]["yaw"].as<float>();
                    wp.target_velocity = points_node[i]["v"].as<float>();
                    path_info.waypoints.push_back(wp);
                }

                RCLCPP_INFO(get_logger(), "[YAML] 成功載入路線: %s (共 %zu 個路徑點, planner=%s)", 
                    path_name.c_str(), path_info.waypoints.size(),
                    path_info.planner == PlannerType::NAV2 ? "nav2" : "chassis_pilot");

                path_database_[path_name] = path_info;
            }
        } catch (const std::exception & e) {
            RCLCPP_ERROR(get_logger(), "讀取路徑 YAML 檔失敗: %s", e.what());
        }
    }

    void goal_response_callback(const GoalHandleNavi::SharedPtr & goal_handle) {
        if (!goal_handle) {
            RCLCPP_ERROR(get_logger(), "目標路徑被底盤拒絕！");
        } else {
            RCLCPP_INFO(get_logger(), "底盤已接受軌跡任務，開跑！");
            is_path_navigating_ = true; 
        }
    }

    void feedback_callback(GoalHandleNavi::SharedPtr, const std::shared_ptr<const NaviGoal::Feedback> feedback) {
        RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 1500, 
            "底盤回傳進度 -> 當前路徑點: %u, 剩餘總距: %.2f 米", 
            feedback->current_waypoint_index + 1, feedback->distance_to_end);
    }

    void result_callback(const GoalHandleNavi::WrappedResult & result) {
        is_path_navigating_ = false; 
        if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
            RCLCPP_INFO(get_logger(), "🎉 [chassis_pilot 閉環] 底盤安全平滑抵達終點！");
            is_path_arrived_ = true; 
        } else {
            RCLCPP_WARN(get_logger(), "底盤導航未成功結束，狀態代碼: %d", static_cast<int>(result.code));
            is_path_arrived_ = true; 
        }
    }

    void nav2_goal_response_callback(const GoalHandleNav2::SharedPtr & goal_handle) {
        if (!goal_handle) {
            RCLCPP_ERROR(get_logger(), "目標路徑被 Nav2 拒絕！");
        } else {
            RCLCPP_INFO(get_logger(), "Nav2 已接受導航任務，開跑！");
            is_path_navigating_ = true; 
        }
    }

    void nav2_feedback_callback(
        GoalHandleNav2::SharedPtr, 
        const std::shared_ptr<const NavigateThroughPoses::Feedback> feedback) 
    {
        RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 1500, 
            "Nav2 回傳進度 -> 剩餘路徑點數: %d, 已行駛距離: %.2f 米", 
            feedback->number_of_poses_remaining, feedback->distance_remaining);
    }

    void nav2_result_callback(const GoalHandleNav2::WrappedResult & result) {
        is_path_navigating_ = false; 
        if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
            RCLCPP_INFO(get_logger(), "🎉 [nav2 閉環] Nav2 安全抵達終點！");
            is_path_arrived_ = true; 
        } else {
            RCLCPP_WARN(get_logger(), "Nav2 導航未成功結束，狀態代碼: %d", static_cast<int>(result.code));
            is_path_arrived_ = true; 
        }
    }

    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Float32>> feedback_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>> done_pub_; 
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Int32>> arm_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Bool>> intake_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Bool>> claw_pub_;
    
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr arm_status_sub_;

    rclcpp_action::Client<NaviGoal>::SharedPtr action_client_;
    rclcpp_action::Client<NavigateThroughPoses>::SharedPtr nav2_action_client_;
    std::map<std::string, PathInfo> path_database_;

    // 新增：服務實體
    rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr switch_mirror_srv_;
};
#endif