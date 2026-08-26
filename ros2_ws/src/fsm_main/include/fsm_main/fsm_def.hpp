    #ifndef FSM_MAIN__FSM_DEF_HPP_
#define FSM_MAIN__FSM_DEF_HPP_

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <boost/sml.hpp>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"

namespace sml = boost::ext::sml;

// 1. 事件定義 (確保與 .cpp 一致)
struct Ev_GoToM1 {};
struct Ev_GoToM2 {};
struct Ev_GoToM3 {};
struct Ev_GoToM4F {};
struct Ev_GoToM4B {};
struct Ev_TaskDone {};

struct Waypoint {
    double x, y, yaw, v;
};

class FsmManagerNode;

// 2. 類別宣告
class FsmManagerNode : public rclcpp::Node {
public:
    FsmManagerNode();
    void change_node_state(const std::string & node_name, uint8_t transition_id);


private:
    struct ManagerLogic; 
    std::unique_ptr<sml::sm<ManagerLogic>> sm_;
    rclcpp::TimerBase::SharedPtr init_timer_;
    
    std::map<std::string, rclcpp::Client<lifecycle_msgs::srv::ChangeState>::SharedPtr> mission_clients_;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_trigger_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_feedback_;
};

// 3. 狀態機邏輯
struct FsmManagerNode::ManagerLogic {
    auto operator()() const noexcept {
        using namespace sml;

        auto activate = [](auto node_name) {
            return [node_name](FsmManagerNode& node_ref) {
                node_ref.change_node_state(node_name, 
                    lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
            };
        };

        // 停用指定的節點
        auto deactivate = [](auto node_name) {
            return [node_name](FsmManagerNode& node_ref) {
                node_ref.change_node_state(node_name, 
                    lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
            };
        };

        auto start = [](auto node_name) {
    return [node_name](FsmManagerNode& node) {
        RCLCPP_INFO(node.get_logger(), "正在進行 %s 的初始設定...", node_name);
        // 這裡可以呼叫自定義 Service 或設定參數
            };
        };

        auto navi_to = [](auto target_mission) {
            return [target_mission](FsmManagerNode& node) {
                RCLCPP_INFO(node.get_logger(), "導航前往任務地點: %s", target_mission);
                // 這裡呼叫 node.send_nav_goal(x, y);
            };
        };

        return make_transition_table(
            // Mission 1
            *"Idle"_s       + event<Ev_GoToM1>   / (activate("mission_one_node"), start("mission_one_node"))   = "M1_Running"_s,
             "M1_Running"_s + event<Ev_TaskDone> / (deactivate("mission_one_node"), 
                                                     activate("mission_two_node"),
                                                    navi_to("M2")) = "M2_Running"_s,

            // Mission 2
             "Idle"_s       + event<Ev_GoToM2>   / (activate("mission_two_node"), start("mission_two_node"))   = "M2_Running"_s,
             "M2_Running"_s + event<Ev_TaskDone> / (deactivate("mission_two_node"), 
                                                    activate("mission_three_node"),
                                                    navi_to("M3")) = "M3_Running"_s,
            
            // Mission 3
            "Idle"_s       + event<Ev_GoToM3>   / (activate("mission_three_node"), start("mission_three_node")) = "M3_Running"_s,
            "M3_Running"_s + event<Ev_TaskDone> / (deactivate("mission_three_node"), 
                                                     activate("mission_four_front_node"),
                                                    navi_to("M4F")) = "M4F_Running"_s,
            
            // Mission 4 (連續性任務範例)
             "Idle"_s       + event<Ev_GoToM4F>   / (activate("mission_four_front_node"), start("mission_four_front_node")) = "M4F_Running"_s,
             "M4F_Running"_s + event<Ev_TaskDone> / (deactivate("mission_four_front_node"), 
                                                     activate("mission_four_back_node"),
                                                    navi_to("M4B")) = "M4B_Running"_s,

             "Idle"_s       + event<Ev_GoToM4B>   / (activate("mission_four_back_node"), start("mission_four_back_node")) = "M4B_Running"_s,
             "M4B_Running"_s + event<Ev_TaskDone> / deactivate("mission_four_back_node") = "Idle"_s
        );
    }
}; 

#endif