#ifndef FSM_MAIN__FSM_DEF_HPP_
#define FSM_MAIN__FSM_DEF_HPP_

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <boost/sml.hpp>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"

namespace sml = boost::ext::sml;

// 1. 事件定義
struct Ev_Start {};      // 啟動排程佇列
struct Ev_TaskDone {};   // 單一任務完成
struct Ev_Abort {};      // 強制中止回到 Idle

struct Waypoint {
    double x, y, yaw, v;
};

class FsmManagerNode;

// 2. 類別宣告
class FsmManagerNode : public rclcpp::Node {
public:
    FsmManagerNode();
    void change_node_state(const std::string & node_name, uint8_t transition_id);

    // 佇列操作介面供 SML Guard 與 Action 使用
    bool has_next_mission() const {
        return current_mission_idx_ < mission_queue_.size();
    }

    std::string get_current_mission() const {
        if (has_next_mission()) {
            return mission_queue_[current_mission_idx_];
        }
        return "";
    }

    void advance_queue() {
        if (current_mission_idx_ < mission_queue_.size()) {
            current_mission_idx_++;
        }
    }

    void reset_queue() {
        current_mission_idx_ = 0;
    }

    // 設定自訂任務佇列（例如傳入 {"M1", "M3", "M4F"}）
    void set_mission_queue(const std::vector<std::string> & queue) {
        mission_queue_ = queue;
        current_mission_idx_ = 0;
    }

    // 解析字串並設定佇列 (支援 "1,3,4f"、"M1, M3, M4F"、"2,3,4b" 等格式)
    void parse_and_set_queue(const std::string & input_str);

private:
    struct ManagerLogic; 
    std::unique_ptr<sml::sm<ManagerLogic>> sm_;
    rclcpp::TimerBase::SharedPtr init_timer_;
    
    std::map<std::string, rclcpp::Client<lifecycle_msgs::srv::ChangeState>::SharedPtr> mission_clients_;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_trigger_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_feedback_;

    // 儲存當前待執行的任務清單與指標
    std::vector<std::string> mission_queue_{"M1", "M2", "M3", "M4F", "M4B"};
    size_t current_mission_idx_{0};
};

// 3. 狀態機邏輯 (Queue + Dispatcher 模式)
struct FsmManagerNode::ManagerLogic {
    auto operator()() const noexcept {
        using namespace sml;

        // --- Guards (判斷下一個任務是什麼) ---
        auto is_mission = [](const std::string & target) {
            return [target](const FsmManagerNode & node) -> bool {
                return node.get_current_mission() == target;
            };
        };

        auto queue_empty = [](const FsmManagerNode & node) -> bool {
            return !node.has_next_mission();
        };

        // --- Actions ---
        auto activate = [](auto node_name) {
            return [node_name](FsmManagerNode& node_ref) {
                node_ref.change_node_state(node_name, 
                    lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
            };
        };

        auto deactivate = [](auto node_name) {
            return [node_name](FsmManagerNode& node_ref) {
                node_ref.change_node_state(node_name, 
                    lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
            };
        };

        auto start = [](auto node_name) {
            return [node_name](FsmManagerNode& node) {
                RCLCPP_INFO(node.get_logger(), "正在進行 %s 的初始設定...", node_name);
            };
        };

        auto navi_to = [](auto target_mission) {
            return [target_mission](FsmManagerNode& node) {
                RCLCPP_INFO(node.get_logger(), "導航前往任務地點: %s", target_mission);
            };
        };

        auto advance = [](FsmManagerNode& node) {
            node.advance_queue();
        };

        return make_transition_table(
            // 1. 從 Idle 收到 Ev_Start，指標重置並進入 Dispatch 分發狀態
            *"Idle"_s + event<Ev_Start> / &FsmManagerNode::reset_queue = "Dispatch"_s,

            // 2. Dispatch 自動根據當前佇列指標跳轉（SML 匿名轉換 + Guard）
            "Dispatch"_s [ is_mission("M1") ]  / (activate("mission_one_node"), start("mission_one_node"), navi_to("M1")) = "M1_Running"_s,
            "Dispatch"_s [ is_mission("M2") ]  / (activate("mission_two_node"), start("mission_two_node"), navi_to("M2")) = "M2_Running"_s,
            "Dispatch"_s [ is_mission("M3") ]  / (activate("mission_three_node"), start("mission_three_node"), navi_to("M3")) = "M3_Running"_s,
            "Dispatch"_s [ is_mission("M4F") ] / (activate("mission_four_front_node"), start("mission_four_front_node"), navi_to("M4F")) = "M4F_Running"_s,
            "Dispatch"_s [ is_mission("M4B") ] / (activate("mission_four_back_node"), start("mission_four_back_node"), navi_to("M4B")) = "M4B_Running"_s,
            "Dispatch"_s [ queue_empty ]       / [](FsmManagerNode& node) {
                RCLCPP_INFO(node.get_logger(), "🎉 [FSM] 所選任務清單已全數執行完畢！回到 Idle。");
            } = "Idle"_s,

            // 3. 各任務完成後 (Ev_TaskDone)：停用節點 -> 佇列前進 -> 回到 Dispatch
            "M1_Running"_s  + event<Ev_TaskDone> / (deactivate("mission_one_node"), advance)        = "Dispatch"_s,
            "M2_Running"_s  + event<Ev_TaskDone> / (deactivate("mission_two_node"), advance)        = "Dispatch"_s,
            "M3_Running"_s  + event<Ev_TaskDone> / (deactivate("mission_three_node"), advance)      = "Dispatch"_s,
            "M4F_Running"_s + event<Ev_TaskDone> / (deactivate("mission_four_front_node"), advance) = "Dispatch"_s,
            "M4B_Running"_s + event<Ev_TaskDone> / (deactivate("mission_four_back_node"), advance)  = "Dispatch"_s,

            // 4. 任何時候收到 Ev_Abort 都能直接回 Idle
            "Dispatch"_s    + event<Ev_Abort> = "Idle"_s
        );
    }
};

#endif