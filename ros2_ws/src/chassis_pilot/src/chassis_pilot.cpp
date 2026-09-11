#include "chassis_pilot/chassis_pilot.hpp"

#include <tf2/exceptions.h>
#include <geometry_msgs/msg/transform_stamped.hpp>

using namespace std::chrono_literals;

ChassisPilot::ChassisPilot(const rclcpp::NodeOptions & options) 
: Node("chassis_pilot", options) {
    velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

    // 初始化 TF2 Buffer 與 Listener
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    action_server_ = rclcpp_action::create_server<NaviGoal>(
        this, "navi_goal",
        std::bind(&ChassisPilot::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&ChassisPilot::handle_cancel, this, std::placeholders::_1),
        std::bind(&ChassisPilot::handle_accepted, this, std::placeholders::_1)
    );

    RCLCPP_INFO(this->get_logger(), "Chassis Pilot 已成功啟動！(座標系: world -> base_footprint)");

    // 50Hz 控制頻率
    timer_ = this->create_wall_timer(20ms, std::bind(&ChassisPilot::control_loop, this));
}

rclcpp_action::GoalResponse ChassisPilot::handle_goal(const rclcpp_action::GoalUUID &, std::shared_ptr<const NaviGoal::Goal>) {
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse ChassisPilot::handle_cancel(const std::shared_ptr<GoalHandleNavi>) {
    return rclcpp_action::CancelResponse::ACCEPT;
}

void ChassisPilot::handle_accepted(const std::shared_ptr<GoalHandleNavi> goal_handle) {
    if (!goal_handle) {
        RCLCPP_ERROR(this->get_logger(), "Goal handle is nullptr!");
        return;
    }
    current_goal_handle_ = goal_handle;

    auto goal = goal_handle->get_goal();
    if (!goal) {
        RCLCPP_ERROR(this->get_logger(), "Goal data is nullptr!");
        return;
    }

    // 1. 核心安全檢查：若收到的路徑為空，立刻拒絕任務避免崩潰
    if (goal->trajectory.empty()) {
        RCLCPP_WARN(this->get_logger(), "收到空的路徑軌跡！拒絕執行任務。");
        auto result = std::make_shared<NaviGoal::Result>();
        result->success = false;
        goal_handle->abort(result);
        current_goal_handle_ = nullptr;
        return;
    }

    // 2. 載入路徑點陣列與重置進度索引
    trajectory_ = goal->trajectory;
    current_waypoint_idx_ = 0;
    
    // 3. 讀取全域控制與加速度限制參數
    max_w_ = goal->max_angular_speed;
    max_accel_ = goal->max_accel; 
    
    // 依 cruise_mode 決定終點策略
    strategy_ = (goal->cruise_mode) ? MoveStrategy::CONTINUOUS : MoveStrategy::SMOOTH_STOP;
    
    // 4. 重置指令追蹤歷史，準備平滑起跑
    last_v_cmd_ = 0.0;
    last_w_cmd_ = 0.0;
    
    RCLCPP_INFO(this->get_logger(), "成功載入新軌跡！總計有 %zu 個路徑點。終點策略: %s", 
                trajectory_.size(),
                (strategy_ == MoveStrategy::CONTINUOUS ? "CONTINUOUS" : "SMOOTH_STOP"));
}

bool ChassisPilot::get_current_pose() {
    geometry_msgs::msg::TransformStamped transform_stamped;
    try {
        // 查詢從 world 到 base_footprint 的最新位姿轉換
        transform_stamped = tf_buffer_->lookupTransform(
            "world", "base_footprint", tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) {
        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "無法取得 TF 轉換 [world -> base_footprint]: %s", ex.what()
        );
        return false;
    }

    x_ = transform_stamped.transform.translation.x;
    y_ = transform_stamped.transform.translation.y;
    yaw_ = quat_to_yaw(transform_stamped.transform.rotation);
    have_state_ = true;
    return true;
}

void ChassisPilot::control_loop() {          
    // ---- 1. 安全與失明防線 ----



    if (!current_goal_handle_) {
        // 只有剛從運動狀態轉為無任務時，送一次 0 速度讓車停下
        if (!is_stopped_) {
            stop_robot();
            is_stopped_ = true;
            RCLCPP_INFO(this->get_logger(), "軌跡結束或待機：發送最後停止指令，釋放底盤控制權。");
        }
        // 之後進入純待機，不再發布任何 /cmd_vel，避免覆蓋外部指令
        return;
    }

    // 透過 TF 查詢當前車體位姿
    if (!get_current_pose()) {
        if (!is_stopped_) {
            stop_robot();
            is_stopped_ = true;
        }
        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "卡關原因：未取得 TF 位姿訊號！");
        return;
    }
    is_stopped_ = false;
    // ---- 2. 檢查任務是否被上層取消 ----
    if (current_goal_handle_->is_canceling()) {
        stop_robot();
        is_stopped_ = true;
        current_goal_handle_->canceled(std::make_shared<NaviGoal::Result>());
        current_goal_handle_ = nullptr;
        RCLCPP_INFO(this->get_logger(), "軌跡任務已被成功取消。");
        return;
    }

    // 確保索引未溢位
    if (current_waypoint_idx_ >= trajectory_.size()) {
        stop_robot();
        return;
    }

    // ---- 3. 前視點（Look-ahead）動態推進演算法 ----
    // 沿著路徑往後檢查，只要當前路徑點與車子太近，且還沒到最後一個點，進度就往前推進
    while (current_waypoint_idx_ < trajectory_.size() - 1) {
        double dx = trajectory_[current_waypoint_idx_].x - x_;
        double dy = trajectory_[current_waypoint_idx_].y - y_;
        double dist_to_wp = std::hypot(dx, dy);

        if (dist_to_wp < look_ahead_distance_) {
            current_waypoint_idx_++;
        } else {
            break; // 找到了合適的前視點，跳出搜尋
        }
    }

    // ---- 4. 提取當前幀的臨時前視目標 ----
    const auto & current_wp = trajectory_[current_waypoint_idx_];
    goal_x_   = current_wp.x;
    goal_y_   = current_wp.y;
    goal_yaw_ = current_wp.yaw;
    max_v_    = current_wp.target_velocity; // 以該路徑點的期望速度作為線速度上限

    // 更新當前臨時目標的偏差狀態 (更新 dist_to_goal_ 與 yaw_to_goal_)
    update_state();

    // 判斷當前前視點是否已經是整條軌跡的「最終終點」
    bool is_last_waypoint = (current_waypoint_idx_ == trajectory_.size() - 1);

    // ---- 5. 軌跡終點完全抵達判定 ----
    if (is_last_waypoint && dist_to_goal_ < pos_tol_ && std::abs(yaw_to_goal_) < yaw_tol_) {
        stop_robot();
        is_stopped_ = true;
        auto result = std::make_shared<NaviGoal::Result>();
        result->success = true;
        current_goal_handle_->succeed(result);
        current_goal_handle_ = nullptr;
        RCLCPP_INFO(this->get_logger(), "🏁 順利平滑抵達整條軌跡終點！");
        return;
    }

    double dt = 0.02; 
    geometry_msgs::msg::Twist cmd;

    // ---- 6. 線速度規劃 (前視空間與期望速度融合) ----
    double v_final = 0.0;

    if (is_last_waypoint && strategy_ == MoveStrategy::SMOOTH_STOP && dist_to_goal_ < look_ahead_distance_) {
        // 進入緩衝區後，依照距離給予低速 (P控制)
        double kp_approach = 0.5; 
        double approach_v = kp_approach * dist_to_goal_;

        // 限制在保底最低速度與最高速度之間
        v_final = std::clamp(approach_v, min_v_, max_v_);
        
        // 強制同步歷史指令
        last_v_cmd_ = v_final; 
    } 
    else {
        // 遠距離或 CONTINUOUS 模式：執行原本的加速度與根號減速邏輯
        double v_limit = max_v_;
        if (is_last_waypoint && strategy_ == MoveStrategy::SMOOTH_STOP) {
            v_limit = std::sqrt(2.0 * max_accel_ * dist_to_goal_);
        }
        v_limit = std::clamp(v_limit, 0.0, max_v_);

        // 保留遠距離的斜率限制，防止起步暴衝
        if (last_v_cmd_ < v_limit) {
            last_v_cmd_ = std::min(last_v_cmd_ + max_accel_ * dt, v_limit);
        } else {
            last_v_cmd_ = std::max(last_v_cmd_ - max_accel_ * dt, v_limit);
        }

        v_final = last_v_cmd_;
        if (v_final < min_v_ && v_limit > min_v_ && dist_to_goal_ > pos_tol_) {
            v_final = min_v_;
        }
    }

    // ---- 7. 全向輪底盤速度向量分解 ----
    double global_dir = std::atan2(goal_y_ - y_, goal_x_ - x_);
    double relative_dir = ang_norm(global_dir - yaw_);
    cmd.linear.x = v_final * std::cos(relative_dir);
    cmd.linear.y = v_final * std::sin(relative_dir);

    // ---- 8. 角速度規劃 (P 控制) ----
    double kp_yaw = 1.0; 
    double max_w_limit = max_w_; 

    double w_target = kp_yaw * yaw_to_goal_;
    w_target = std::clamp(w_target, -max_w_limit, max_w_limit);

    // 終點死區與保底推力保護
    if (std::abs(w_target) < min_w_ && std::abs(yaw_to_goal_) > yaw_tol_) {
        w_target = (yaw_to_goal_ > 0 ? 1.0 : -1.0) * min_w_;
    }
    else if (std::abs(yaw_to_goal_) <= yaw_tol_) {
        w_target = 0.0;
    }

    cmd.angular.z = w_target;
    last_w_cmd_ = w_target; 

    // 發布移動指令
    velocity_publisher_->publish(cmd);

    // ---- 9. 發布實時 Action 反饋 (Feedback) ----
    auto feedback = std::make_shared<NaviGoal::Feedback>();
    feedback->current_waypoint_index = current_waypoint_idx_;
    
    // 計算剩餘總距離（當前點偏差距離 + 後續所有剩餘線段的長度）
    double remaining_dist = dist_to_goal_;
    for (size_t i = current_waypoint_idx_; i < trajectory_.size() - 1; ++i) {
        remaining_dist += std::hypot(trajectory_[i+1].x - trajectory_[i].x, 
                                     trajectory_[i+1].y - trajectory_[i].y);
    }
    feedback->distance_to_end = remaining_dist;
    current_goal_handle_->publish_feedback(feedback);

    RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 500,
        "追隨點: [%zu/%zu] | 座標: %.2f,%.2f | 剩餘角度: %.2fRad | 目前線速: %.2f,%.2f", 
        current_waypoint_idx_ + 1, trajectory_.size(), x_, y_, yaw_to_goal_, cmd.linear.x, cmd.linear.y);
}

void ChassisPilot::stop_robot() {
    velocity_publisher_->publish(geometry_msgs::msg::Twist());
    last_v_cmd_ = 0.0;
    last_w_cmd_ = 0.0;
}

void ChassisPilot::update_state() {
    dist_to_goal_ = std::hypot(goal_x_ - x_, goal_y_ - y_);
    yaw_to_goal_ = ang_norm(goal_yaw_ - yaw_);
}

double ChassisPilot::ang_norm(double a) {
    while (a > M_PI) a -= 2.0 * M_PI;
    while (a < -M_PI) a += 2.0 * M_PI;
    return a;
}

double ChassisPilot::quat_to_yaw(const geometry_msgs::msg::Quaternion & q) {
    return std::atan2(2.0 * (q.w * q.z + q.x * q.y), 1.0 - 2.0 * (q.y * q.y + q.z * q.z));
}

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<ChassisPilot>();
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node->get_node_base_interface());
    executor.spin();
    rclcpp::shutdown();
    return 0;
}