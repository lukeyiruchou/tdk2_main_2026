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
#include "ament_index_cpp/get_package_share_directory.hpp"

// 引入你的自定義動作與訊息型態
#include "interfaces/action/navi_goal.hpp"
#include "interfaces/msg/custom_waypoint.hpp"

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class BaseStateNode : public rclcpp_lifecycle::LifecycleNode {
public:
    using NaviGoal = interfaces::action::NaviGoal;
    using GoalHandleNavi = rclcpp_action::ClientGoalHandle<NaviGoal>;

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
        
        // 2. 直接建立對接底盤 ChassisPilot 的 Action Client
        action_client_ = rclcpp_action::create_client<NaviGoal>(this, "navi_goal");

        // 3. 自動讀取路徑 YAML 檔案並載入記憶體
        load_paths_from_yaml();
        
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_activate(const rclcpp_lifecycle::State &) override {
        feedback_pub_->on_activate();
        status_pub_->on_activate();
        done_pub_->on_activate(); 
        
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

    // 子類點餐工具（查 YAML -> 直發 Action）
    void move_along_path(const std::string & path_name) {
        auto it = path_database_.find(path_name);
        if (it == path_database_.end()) {
            RCLCPP_ERROR(get_logger(), "找不到路徑代號: %s !! 拒絕發送移動請求。", path_name.c_str());
            is_path_arrived_ = true; 
            return;
        }

        if (!action_client_->wait_for_action_server(std::chrono::seconds(2))) {
            RCLCPP_ERROR(get_logger(), "底盤 Action 伺服器 (/navi_goal) 未上線！");
            return;
        }

        auto goal_msg = NaviGoal::Goal();
        goal_msg.trajectory = it->second;
        goal_msg.max_angular_speed = 1.0;
        goal_msg.max_accel = 0.2;
        goal_msg.cruise_mode = false;

        is_path_navigating_ = false;
        is_path_arrived_ = false;

        auto send_goal_options = rclcpp_action::Client<NaviGoal>::SendGoalOptions();
        send_goal_options.goal_response_callback = 
            std::bind(&BaseStateNode::goal_response_callback, this, std::placeholders::_1);
        send_goal_options.feedback_callback = 
            std::bind(&BaseStateNode::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
        send_goal_options.result_callback = 
            std::bind(&BaseStateNode::result_callback, this, std::placeholders::_1);

        RCLCPP_INFO(get_logger(), "正在發送軌跡 Action 至底盤，目標路線：[%s]", path_name.c_str());
        action_client_->async_send_goal(goal_msg, send_goal_options);
    }

private:
    // YAML 檔案自動化載入與解析函數
    void load_paths_from_yaml() {
        try {
            std::string pkg_path = ament_index_cpp::get_package_share_directory("fsm_main");
            std::string file_path = pkg_path + "/config/path.yaml";
            
            YAML::Node config = YAML::LoadFile(file_path);
            if (!config["paths"]) return;

            for (auto it = config["paths"].begin(); it != config["paths"].end(); ++it) {
                std::string path_name = it->first.as<std::string>();
                YAML::Node points_node = it->second;
                std::vector<interfaces::msg::CustomWaypoint> waypoints;
                
                for (size_t i = 0; i < points_node.size(); ++i) {
                    interfaces::msg::CustomWaypoint wp;
                    wp.x = points_node[i]["x"].as<float>();
                    wp.y = points_node[i]["y"].as<float>();
                    wp.yaw = points_node[i]["yaw"].as<float>();
                    wp.target_velocity = points_node[i]["v"].as<float>();
                    waypoints.push_back(wp);
                }
                path_database_[path_name] = waypoints;
                RCLCPP_INFO(get_logger(), "[YAML] 成功載入路線: %s (共 %zu 個路徑點)", path_name.c_str(), waypoints.size());
            }
        } catch (const std::exception & e) {
            RCLCPP_ERROR(get_logger(), "讀取路徑 YAML 檔失敗: %s", e.what());
        }
    }

    // Action Client 的三個非同步回呼函數
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
            RCLCPP_INFO(get_logger(), "🎉 [Action 閉環] 底盤安全平滑抵達終點！");
            is_path_arrived_ = true; 
        } else {
            RCLCPP_WARN(get_logger(), "底盤導航未成功結束，狀態代碼: %d", static_cast<int>(result.code));
            is_path_arrived_ = true; 
        }
    }

    // 類別私有成員發布者
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Float32>> feedback_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>> status_pub_;
    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>> done_pub_; 
    
    // Action 與 YAML 緩存
    rclcpp_action::Client<NaviGoal>::SharedPtr action_client_;
    std::map<std::string, std::vector<interfaces::msg::CustomWaypoint>> path_database_;
}; // <-- 確保這個類別結尾大括號完美閉合

#endif 