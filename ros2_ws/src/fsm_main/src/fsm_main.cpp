#include "fsm_main/fsm_def.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

FsmManagerNode::FsmManagerNode() : Node("fsm_manager_node") {
    // 1. 初始化任務節點清單與 Lifecycle Clients
    std::vector<std::string> target_nodes = {
        "mission_one_node", "mission_two_node", 
        "mission_three_node", "mission_four_front_node", "mission_four_back_node"
    };

    for (const auto & name : target_nodes) {
        std::string service_name = "/" + name + "/change_state";
        mission_clients_[name] = this->create_client<lifecycle_msgs::srv::ChangeState>(service_name);
    }

    // 2. 初始化狀態機
    sm_ = std::make_unique<sml::sm<ManagerLogic>>(*this);

    // 3. 外部觸發 Topic 監聽器 (/fsm/trigger)
    // 支援格式：
    // - "1, 3, 4f" 或 "2, 3, 4b"：自訂序列並立即啟動
    // - "start"：直接以目前佇列啟動
    // - "stop" 或 "abort"：強制中止並回 Idle
    sub_trigger_ = this->create_subscription<std_msgs::msg::String>(
        "/fsm/trigger", 10, [this](const std_msgs::msg::String::SharedPtr msg) {
            std::string cmd = msg->data;
            RCLCPP_INFO(this->get_logger(), "收到外部觸發指令: [%s]", cmd.c_str());

            if (cmd == "stop" || cmd == "abort") {
                RCLCPP_WARN(this->get_logger(), "⚠️ 收到中斷請求，強制結束當前排程！");
                this->sm_->process_event(Ev_Abort{});
                return;
            }

            // 如果不是純 "start"，表示有傳入自訂任務排程（如 "1,3,4f"、"2,3,4b"）
            if (cmd != "start" && !cmd.empty()) {
                this->parse_and_set_queue(cmd);
            }

            // 啟動狀態機執行佇列
            RCLCPP_INFO(this->get_logger(), "🚀 開始執行任務排程！");
            this->sm_->process_event(Ev_Start{});
        });

    // 4. 監聽任務完成訊號 (/state_feedback)
    sub_feedback_ = this->create_subscription<std_msgs::msg::String>(
        "/state_feedback", 10, [this](const std_msgs::msg::String::SharedPtr msg) {
            if (msg->data == "done") {
                RCLCPP_INFO(this->get_logger(), "🎯 收到來自 Mission 節點的 done 訊號！推進下一個任務...");
                this->sm_->process_event(Ev_TaskDone{});
            }
        });

    // 5. 節點啟動 1 秒後，將所有任務節點預先 CONFIGURE 成 Inactive 狀態
    init_timer_ = this->create_wall_timer(
        std::chrono::seconds(1),
        [this, target_nodes]() {
            RCLCPP_INFO(this->get_logger(), "⚙️ [系統初始化] 正在預先配置所有任務節點 (CONFIGURE)...");
            
            for (const auto & name : target_nodes) {
                // 狀態碼 1 代表 TRANSITION_CONFIGURE
                this->change_node_state(name, lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE); 
            }
            
            RCLCPP_INFO(this->get_logger(), "✅ [系統初始化] 任務節點已進入 Inactive 狀態，靜候調度。");
            this->init_timer_->cancel();
        });

    RCLCPP_INFO(this->get_logger(), "FSM Manager 已成功就緒，預設佇列: M1 -> M2 -> M3 -> M4F -> M4B");
}

// 解析輸入字串並重組任務佇列 (支援如 "1, 3, 4f"、"m2,m3,m4b" 等任意組合)
void FsmManagerNode::parse_and_set_queue(const std::string & input_str) {
    std::vector<std::string> new_queue;
    std::stringstream ss(input_str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        // 去除頭尾空白並轉為大寫
        item.erase(std::remove_if(item.begin(), item.end(), ::isspace), item.end());
        std::transform(item.begin(), item.end(), item.begin(), ::toupper);

        if (item.empty()) continue;

        // 若未帶 "M" 前綴則自動補上（例如 "1" -> "M1", "4F" -> "M4F"）
        if (item.rfind("M", 0) != 0) {
            item = "M" + item;
        }

        // 驗證是否為有效任務代號
        if (item == "M1" || item == "M2" || item == "M3" || item == "M4F" || item == "M4B") {
            new_queue.push_back(item);
        } else {
            RCLCPP_WARN(this->get_logger(), "⚠️ 忽略無法識別的任務代號: %s", item.c_str());
        }
    }

    if (!new_queue.empty()) {
        mission_queue_ = new_queue;
        current_mission_idx_ = 0;
        
        std::string debug_str;
        for (size_t i = 0; i < mission_queue_.size(); ++i) {
            debug_str += mission_queue_[i] + (i + 1 < mission_queue_.size() ? " -> " : "");
        }
        RCLCPP_INFO(this->get_logger(), "📋 [任務排程已更新] 執行路徑: [ %s ]", debug_str.c_str());
    } else {
        RCLCPP_WARN(this->get_logger(), "⚠️ 解析結果為空，保留原任務佇列。");
    }
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
    
    RCLCPP_INFO(this->get_logger(), "發送狀態轉換指令 (ID: %d) 給 %s", transition_id, node_name.c_str());
}