#include "fsm_main/fsm_node.hpp"
#include "rclcpp/rclcpp.hpp"

class MissionOneNode : public BaseStateNode {//here
public:
    // 呼叫父類建構子，定義節點名稱
    MissionOneNode() : BaseStateNode("mission_one_node") {}//here

protected:
    // 覆寫配置邏輯
    CallbackReturn on_configure(const rclcpp_lifecycle::State & state) override {
        // 1. 先執行父類的通用配置 (如建立進度發布者)
        if (BaseStateNode::on_configure(state) != CallbackReturn::SUCCESS) {
            return CallbackReturn::FAILURE;
        }

        // 2. 實作此狀態特有的初始化 (例如：讀取該狀態專用的參數)
        RCLCPP_INFO(get_logger(), "正在配置：載入動作序列庫...");
        return CallbackReturn::SUCCESS;
    }

    // 覆寫激活邏輯 (進入此狀態)
    CallbackReturn on_activate(const rclcpp_lifecycle::State & state) override {
        // 1. 先執行父類的激活 (如啟用 Publisher)
        BaseStateNode::on_activate(state);

        // 2. 啟動此狀態的專屬任務 (例如：開啟定時器模擬工作)
        RCLCPP_INFO(get_logger(), "mission one activate");
        progress_ = 0.0;

        move_along_path("PATH_1_0");

        timer_ = this->create_wall_timer(
            std::chrono::seconds(1), 
            std::bind(&MissionOneNode::execute_step, this));//here

        return CallbackReturn::SUCCESS;
    }

    // 覆寫停用邏輯 (離開此狀態)
    CallbackReturn on_deactivate(const rclcpp_lifecycle::State & state) override {
        // 1. 停止此狀態的工作
        RCLCPP_INFO(get_logger(), "mission one stop");
        if (timer_) {
            timer_->cancel();
        }

        // 2. 執行父類的停用 (如停用 Publisher)
        return BaseStateNode::on_deactivate(state);
    }

private:
// ----------------------------------------------------------任務主要部分



    void execute_step() {
        if (is_path_navigating_) {
            RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 2000, "車子還在路上，Mission 暫停推進...");
            return;
        }

        // 階段 2：當父類內置的 Action 接收到 Result 成功並點亮旗標後，觸發到站定點動作
        if (is_path_arrived_) {
            if (progress_ < 0.1) {
                progress_ += 0.1; // 
                move_along_path("PATH_1_1");
                RCLCPP_INFO(get_logger(), "車體已到站！正在執行任務... 進度: %.2f", progress_);
            } 
            else if (progress_ < 1){
                progress_ += 0.1; // 
                RCLCPP_INFO(get_logger(), "車體已到站！正在執行任務... 進度: %.2f", progress_);
            }
            // 階段 3：定點任務也功德圓滿，切換到下一關 Lifecycle 狀態
            else {
                RCLCPP_INFO(get_logger(), "✅ Mission One complete！移交大腦狀態機。");
                timer_->cancel();
                notify_manager_done(); // 呼叫父類工具通知 Manager 關閉自己並開啟 MissionTwo
            }
        }
    }

    float progress_;
    rclcpp::TimerBase::SharedPtr timer_;
};


int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MissionOneNode>();//here
    rclcpp::spin(node->get_node_base_interface());
    rclcpp::shutdown();
    return 0;
}