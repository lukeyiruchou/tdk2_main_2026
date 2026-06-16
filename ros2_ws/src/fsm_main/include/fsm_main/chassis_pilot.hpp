#ifndef FSM_MAIN__CHASSIS_PILOT_HPP_
#define FSM_MAIN__CHASSIS_PILOT_HPP_

#include <cmath>
#include <algorithm>
#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "interfaces/action/navi_goal.hpp"
#include "interfaces/msg/custom_waypoint.hpp"

using namespace std::chrono_literals;

// 定義移動策略，取代舊的 move_mode
enum class MoveStrategy {
    SMOOTH_STOP,   // 到達目標時減速至 0
    CONTINUOUS     // 到達目標時保持速度 (Cruise 模式)
};

class ChassisPilot : public rclcpp::Node {
public:
    using NaviGoal = interfaces::action::NaviGoal;
    using GoalHandleNavi = rclcpp_action::ServerGoalHandle<NaviGoal>;

    explicit ChassisPilot(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

private:
    // Action Server Callbacks
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const NaviGoal::Goal> goal);
    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandleNavi> goal_handle);
    void handle_accepted(const std::shared_ptr<GoalHandleNavi> goal_handle);

    // Topic Callbacks
    void position_callback(const nav_msgs::msg::Odometry::SharedPtr msg);

    // 核心邏輯
    void control_loop();
    void update_state();
    void stop_robot();
    static double ang_norm(double a);
    static double quat_to_yaw(const geometry_msgs::msg::Quaternion & q);

    // ROS 2 成員
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr position_subscriber_;
    rclcpp_action::Server<NaviGoal>::SharedPtr action_server_;
    rclcpp::TimerBase::SharedPtr timer_;

    // 狀態變數
    double x_{0}, y_{0}, yaw_{0};
    double last_v_cmd_{0};
    double last_w_cmd_{0}; // 用於梯形規劃加速段的指令追蹤
    bool have_state_{false};

    // 目標與策略變數
    std::shared_ptr<GoalHandleNavi> current_goal_handle_;

    std::vector<interfaces::msg::CustomWaypoint> trajectory_; // 儲存整條平滑軌跡
    size_t current_waypoint_idx_{0};


    double goal_x_{0}, goal_y_{0}, goal_yaw_{0};
    double dist_to_goal_{0}, yaw_to_goal_{0};
    double target_velocity_{0};
    
    // 控制參數 (從 Action Goal 取得)
    double max_v_{4.0}, max_accel_{2.0};
    double max_w_{0.3}, max_ang_accel_{0.5};
    double pos_tol_{0.05}, yaw_tol_{0.05};
    double min_v_{0.1};
    double min_w_{0.05};
    
    double look_ahead_distance_{1.0};

    MoveStrategy strategy_{MoveStrategy::SMOOTH_STOP};
};

#endif