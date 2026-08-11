#ifndef FSM_BASE_STATE_NODE_HPP_
#define FSM_BASE_STATE_NODE_HPP_

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

#include "ament_index_cpp/get_package_share_directory.hpp"

// 引入你的自定義動作與訊息型態
#include "interfaces/action/navi_goal.hpp"
#include "interfaces/msg/custom_waypoint.hpp"

// Nav2 導航所需的 Action 與訊息型態
#include "nav2_msgs/action/navigate_through_poses.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class BaseStateNode : public rclcpp_lifecycle::LifecycleNode {
public:
    using NaviGoal = interfaces::action::NaviGoal;
    using GoalHandleNavi = rclcpp_action::ClientGoalHandle<NaviGoal>;

    using NavigateThroughPoses = nav2_msgs::action::NavigateThroughPoses;
    using GoalHandleNav2 = rclcpp_action::ClientGoalHandle<NavigateThroughPoses>;

    // 路徑要使用哪一種規劃器來執行
    enum class PlannerType {
        CHASSIS_PILOT,
        NAV2
    };

    // 建構子：強制要求傳入節點名稱
    explicit BaseStateNode(const std::string & node_name) 
    : LifecycleNode(node_name) {}

    virtual ~BaseStateNode() = default;

    // --- Lifecycle 介面封裝 ---
    CallbackReturn on_configure(const rclcpp_lifecycle::State &) override {
        RCLCPP_INFO(get_logger(), "--- [BaseStateNode] 正在執行通用配置 ---");
        
        // 1. 建立進度與狀態發布者
        feedback_pub_ = this->create_publisher<std_msgs::msg::Float32>("~/progress", 10);
        status_pub_ = this->create_publisher<std_msgs::msg::String>("~/current_state", 10);
        done_pub_ = this->create_publisher<std_msgs::msg::String>("/state_feedback", 10);

        arm_pub_    = this->create_publisher<std_msgs::msg::Int32>("robot/cmd_arm", 10);
        intake_pub_ = this->create_publisher<std_msgs::msg::Bool>("robot/cmd_intake", 10);
        claw_pub_   = this->create_publisher<std_msgs::msg::Bool>("robot/cmd_claw", 10);
        
        // 2. 建立對接底盤 ChassisPilot 的 Action Client
        action_client_ = rclcpp_action::create_client<NaviGoal>(this, "navi_goal");

        // 2-1. 建立對接 Nav2 的 Action Client
        nav2_action_client_ = rclcpp_action::create_client<NavigateThroughPoses>(
            this, "navigate_through_poses");

        // 3. 自動讀取路徑 YAML 檔案並載入記憶體
        load_paths_from_yaml();
        
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_activate(const rclcpp_lifecycle::State &) override {
        feedback_pub_->on_activate();
        status_pub_->on_activate();
        done_pub_->on_activate(); 

        arm_pub_->on_activate();
        intake_pub_->on_activate();
        claw_pub_->on_activate();
        
        // 每次狀態節點被激活時，重置導航狀態鎖
        is_path_navigating_ = false;
        is_path_arrived_ = false;

        publish_status("ACTIVED");
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_deactivate(const rclcpp_lifecycle::State &) override {
        publish_status("DEACTIVED");
        feedback_pub_->on_deactivate();
        status_pub_->on_deactivate();
        done_pub_->on_deactivate(); 
        
        arm_pub_->on_deactivate();
        intake_pub_->on_deactivate();
        claw_pub_->on_deactivate();
        return CallbackReturn::SUCCESS;
    }

protected:
    // ----- 子類可直接讀取的導航狀態鎖 -----
    bool is_path_navigating_{false}; // 車子是否正在路线上奔馳
    bool is_path_arrived_{false};    // 車子是否已經圓滿抵達終點

    // 工具函式：發送進度 (0.0 ~ 1.0)
    void send_progress(float progress) {
        auto msg = std_msgs::msg::Float32();
        msg.data = progress;
        feedback_pub_->publish(msg); 
    }

    // 工具函式：發送自定義狀態訊息
    void publish_status(const std::string & status) {
        auto msg = std_msgs::msg::String();
        msg.data = status;
        status_pub_->publish(msg);
    }

    // 讓 Manager 知道任務做完了
    void notify_manager_done() {
        auto msg = std::make_unique<std_msgs::msg::String>();
        msg->data = "done";
        done_pub_->publish(std::move(msg));
        RCLCPP_INFO(this->get_logger(), "Task Finished. Notifying Manager...");
    }

    // 子類點餐工具（查 YAML -> 依照 planner 分流至 chassis_pilot 或 nav2）
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

    //  子類點餐工具：控制進料吸取機構 (true: 放料/吸取, false: 收回)
    void set_intake_state(bool deploy) {
        auto msg = std_msgs::msg::Bool();
        msg.data = deploy;
        intake_pub_->publish(msg);
        RCLCPP_INFO(get_logger(), "🌪️ [BaseStateNode] send intake state: [%s]", deploy ? "開啟" : "關閉");
    }

    // 🗜️ 子類點餐工具：控制夾爪機構 (true: 閉合夾緊, false: 張開釋放)
    void set_claw_state(bool close_claw) {
        auto msg = std_msgs::msg::Bool();
        msg.data = close_claw;
        claw_pub_->publish(msg);
        RCLCPP_INFO(get_logger(), "🗜️ [BaseStateNode] send claw state: [%s]", close_claw ? "閉合" : "張開");
    }

private:
    // 每條路徑的資訊：要用哪種規劃器 + 路徑點
    struct PathInfo {
        PlannerType planner{PlannerType::CHASSIS_PILOT};
        std::vector<interfaces::msg::CustomWaypoint> waypoints;
    };

    // ---------- chassis_pilot 導航實作 ----------
    void move_via_chassis_pilot(
        const std::string & path_name,
        const std::vector<interfaces::msg::CustomWaypoint> & waypoints) 
    {
        if (!action_client_->wait_for_action_server(std::chrono::seconds(2))) {
            RCLCPP_ERROR(get_logger(), "底盤 Action 伺服器 (/navi_goal) 未上線！");
            return;
        }

        auto goal_msg = NaviGoal::Goal();
        goal_msg.trajectory = waypoints;
        goal_msg.max_angular_speed = 1.0;
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

    // ---------- nav2 導航實作 ----------
    void move_via_nav2(
        const std::string & path_name,
        const std::vector<interfaces::msg::CustomWaypoint> & waypoints)
    {
        if (!nav2_action_client_->wait_for_action_server(std::chrono::seconds(2))) {
            RCLCPP_ERROR(get_logger(), "Nav2 Action 伺服器 (/navigate_through_poses) 未上線！");
            return;
        }

        auto goal_msg = NavigateThroughPoses::Goal();
        rclcpp::Time now = this->get_clock()->now();

        for (const auto & wp : waypoints) {
            geometry_msgs::msg::PoseStamped pose;
            pose.header.frame_id = "map";
            pose.header.stamp = now;
            pose.pose.position.x = wp.x;
            pose.pose.position.y = wp.y;
            pose.pose.position.z = 0.0;

            // yaw -> quaternion（僅繞 Z 軸旋轉）
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

    // YAML 檔案自動化載入與解析函數
    void load_paths_from_yaml() {
        try {
            std::string pkg_path = ament_index_cpp::get_package_share_directory("fsm_main");
            std::string file_path = pkg_path + "/config/path.yaml";
            
            YAML::Node config = YAML::LoadFile(file_path);
            if (!config["paths"]) return;

            for (auto it = config["paths"].begin(); it != config["paths"].end(); ++it) {
                std::string path_name = it->first.as<std::string>();
                YAML::Node path_node = it->second;

                PathInfo path_info;
                YAML::Node points_node;

                if (path_node.IsMap() && path_node["points"]) {
                    // 新格式： { planner: chassis_pilot|nav2, points: [...] }
                    std::string planner_str = path_node["planner"]
                        ? path_node["planner"].as<std::string>()
                        : "chassis_pilot";
                    path_info.planner = (planner_str == "nav2")
                        ? PlannerType::NAV2
                        : PlannerType::CHASSIS_PILOT;
                    points_node = path_node["points"];
                } else {
                    // 舊格式：純陣列，向下相容，預設走 chassis_pilot
                    path_info.planner = PlannerType::CHASSIS_PILOT;
                    points_node = path_node;
                }

                for (size_t i = 0; i < points_node.size(); ++i) {
                    interfaces::msg::CustomWaypoint wp;
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

    // ----- chassis_pilot Action Client 的三個非同步回呼函數 -----
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

    // ----- nav2 Action Client 的三個非同步回呼函數 -----
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
            feedback->number_of_poses_remaining, feedback->distance_traveled);
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

    // 類別私有成員發布者
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Float32>> feedback_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>> status_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>> done_pub_; 

    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Int32>> arm_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Bool>> intake_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Bool>> claw_pub_;
    
    // Action 與 YAML 緩存
    rclcpp_action::Client<NaviGoal>::SharedPtr action_client_;
    rclcpp_action::Client<NavigateThroughPoses>::SharedPtr nav2_action_client_;
    std::map<std::string, PathInfo> path_database_;
};
#endif