#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from rclpy.action import ActionClient
from interfaces.action import NaviGoal
from interfaces.msg import CustomWaypoint

class PathTestClient(Node):
    def __init__(self):
        super().__init__('path_test_client')
        self._action_client = ActionClient(self, NaviGoal, 'navi_goal')

    def send_path(self):
        msg = NaviGoal.Goal()
        msg.max_angular_speed = 1.0
        msg.max_accel = 0.5
        msg.cruise_mode = False

        # 定義 4 個點組成的 S 型曲線
        points = [
             (0.425, 1.0, 0.0, 0.6),
            (3.0, 1.0, 0.785, 1.0),
           (3.0, 3.0, 1.57, 2.0),
            (3.0, 6.0, 0.785, 1.0),
             (6.0, 6.0, 0.0, 0.3)
        ]

        for x, y, yaw, v in points:
            wp = CustomWaypoint()
            wp.x = x
            wp.y = y
            wp.yaw = yaw
            wp.target_velocity = v
            msg.trajectory.append(wp)

        self.get_logger().info('正在發送連續軌跡任務...')
        self._action_client.wait_for_server()
        
        # 發送目標並綁定 Feedback 回呼
        self._send_goal_future = self._action_client.send_goal_async(
            msg, 
            feedback_callback=self.feedback_callback
        )
        # ✨ 新增：當伺服器接受目標後，進入 goal_response_callback 進行下一步追蹤
        self._send_goal_future.add_done_callback(self.goal_response_callback)

    def goal_response_callback(self, future):
        goal_handle = future.result()
        if not goal_handle.accepted:
            self.get_logger().error('任務被 ChassisPilot 拒絕！')
            rclpy.shutdown()
            return

        self.get_logger().info('ChassisPilot 接受任務，開始追隨軌跡...')
        # ✨ 新增：開始等待最終的 Result 成果
        self._get_result_future = goal_handle.get_result_async()
        self._get_result_future.add_done_callback(self.get_result_callback)

    def feedback_callback(self, feedback_msg):
        feedback = feedback_msg.feedback
        self.get_logger().info(
            f' 收到反饋 -> 當前目標點索引: {feedback.current_waypoint_index + 1}, '
            f'剩餘總距: {feedback.distance_to_end:.2f} m'
        )

    # ✨ 新增：當 ChassisPilot 功德圓滿，回報結果時觸發
    def get_result_callback(self, future):
        result = future.result().result
        status = future.result().status
        
        # 判斷 ROS 2 Action 的狀態代碼 (STATUS_SUCCEEDED 通常是 4)
        if status == 4: # GoalStatus.STATUS_SUCCEEDED
            self.get_logger().info('========================================')
            self.get_logger().info('🎉 成功！ChassisPilot 已安全平滑抵達終點！')
            self.get_logger().info('========================================')
        else:
            self.get_logger().warn(f'軌跡任務未成功完成，狀態碼: {status}')
            
        # 🏁 大功告成，命令整顆 Python 節點完全退出，釋放 Terminal
        self.get_logger().info('測試完成，關閉測試腳本。')
        rclpy.shutdown()

def main(args=None):
    rclpy.init(args=args)
    client = PathTestClient()
    client.send_path()
    try:
        rclpy.spin(client)
    except SystemExit: # 捕捉 shutdown 引發的正常退出，避免噴出難看的 Traceback 報錯
        pass

if __name__ == '__main__':
    main()