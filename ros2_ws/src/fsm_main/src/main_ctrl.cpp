#include "rclcpp/rclcpp.hpp"
#include "fsm_main/fsm_def.hpp" // 包含我們定義的類別

int main(int argc, char ** argv)
{
  // 1. 初始化 ROS 2 通訊環境
  rclcpp::init(argc, argv);

  // 2. 建立 Manager 節點實例
  // 這裡會觸發 FsmManagerNode 的建構子，進而初始化 SML 狀態機
  auto node = std::make_shared<FsmManagerNode>();

  // 3. 進入循環（Spin），開始監聽 Topic 與處理 FSM 事件
  RCLCPP_INFO(node->get_logger(), "FSM Manager Node 啟動中...");
  rclcpp::spin(node);

  // 4. 當收到結束訊號（如 Ctrl+C）後清理資源
  rclcpp::shutdown();
  return 0;
}