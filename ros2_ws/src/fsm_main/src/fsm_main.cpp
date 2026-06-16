#include "fsm_main/fsm_def.hpp"

FsmManagerNode::FsmManagerNode() : Node("fsm_manager_node") {
    // 初始化任務節點清單
    std::vector<std::string> target_nodes = {"mission_one_node", "mission_two_node", 
         "mission_three_node",  "mission_four_front_node", "mission_four_back_node"};

    for (const auto & name : target_nodes) {
        std::string service_name = "/" + name + "/change_state";
        mission_clients_[name] = this->create_client<lifecycle_msgs::srv::ChangeState>(service_name);
    }

    // 初始化狀態機
    sm_ = std::make_unique<sml::sm<ManagerLogic>>(*this);

    // ✨ 1. 補齊核心：建立外部觸發 Topic 監聽器
    // 監聽 "/fsm/trigger" 頻道，並將字串翻譯成 Boost SML 懂的實體事件
    sub_trigger_ = this->create_subscription<std_msgs::msg::String>(
        "/fsm/trigger", 10, [this](const std_msgs::msg::String::SharedPtr msg) {
            RCLCPP_INFO(this->get_logger(), "收到外部事件觸發請求: [%s]", msg->data.c_str());
            
            if (msg->data == "Ev_GoToM1") {
                this->sm_->process_event(Ev_GoToM1{});
            } else if (msg->data == "Ev_GoToM2") {
                this->sm_->process_event(Ev_GoToM2{});
            } else if (msg->data == "Ev_GoToM3") {
                this->sm_->process_event(Ev_GoToM3{});
            } else if (msg->data == "Ev_GoToM4F") {
                this->sm_->process_event(Ev_GoToM4F{});
            } else if (msg->data == "Ev_GoToM4B") {
                this->sm_->process_event(Ev_GoToM4B{});
            } else {
                RCLCPP_WARN(this->get_logger(), "無法識別的外部事件: %s", msg->data.c_str());
            }
        });

    // 🛠️ 2. 頻道對齊：將相對路徑 "state_feedback" 改為絕對路徑 "/state_feedback"
    // 這樣才能跟父類 BaseStateNode 的 done_pub_ 完美接上線！
    sub_feedback_ = this->create_subscription<std_msgs::msg::String>(
        "/state_feedback", 10, [this](const std_msgs::msg::String::SharedPtr msg) {
            if (msg->data == "done") {
                RCLCPP_INFO(this->get_logger(), "🎯 收到來自 Mission 節點的 done 訊號！推進狀態機...");
                this->sm_->process_event(Ev_TaskDone{});
            }
        });

        init_timer_ = this->create_wall_timer(
        std::chrono::seconds(1),
        [this, target_nodes]() {
            RCLCPP_INFO(this->get_logger(), "⚙️ [系統初始化] 正在預先配置所有任務節點...");
            
            for (const auto & name : target_nodes) {
                // 狀態碼 1 代表 TRANSITION_CONFIGURE
                this->change_node_state(name, 1); 
            }
            
            RCLCPP_INFO(this->get_logger(), "✅ [系統初始化] 5個任務節點已全數進入 Inactive 狀態，靜候指令。");
            this->init_timer_->cancel(); // 功成身退，關閉此計時器
        });

    RCLCPP_INFO(this->get_logger(), "FSM Manager 已成功就緒。");
}

void FsmManagerNode::change_node_state(const std::string & node_name, uint8_t transition_id) {
    if (mission_clients_.find(node_name) == mission_clients_.end()) {
        RCLCPP_ERROR(this->get_logger(), "找不到 Client: %s", node_name.c_str());
        return;
    }

    auto client = mission_clients_[node_name];
    if (!client->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_WARN(this->get_logger(), "Service [%s] 不可用", node_name.c_str());
        return;
    }

    auto request = std::make_shared<lifecycle_msgs::srv::ChangeState::Request>();
    request->transition.id = transition_id;
    client->async_send_request(request);
    
    RCLCPP_INFO(this->get_logger(), "發送指令 %d 給 %s", transition_id, node_name.c_str());
}