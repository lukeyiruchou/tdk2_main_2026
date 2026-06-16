import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
import xacro

def generate_launch_description():
    # 1. 取得套件的 share 路徑
    # 註：確保你的 urdf 檔案放在 fsm_main/urdf/omni_robot.urdf.xacro
    pkg_fsm_main = get_package_share_directory('fsm_main')
    xacro_file = os.path.join(pkg_fsm_main, 'urdf', 'omni_robot.urdf.xacro')

    # 2. 使用 xacro 解析工具，將 xacro 檔案即時轉譯成純 URDF 字串
    robot_description_raw = xacro.process_file(xacro_file).toxml()

    # 3. 引入 Gazebo 官方提供的空白世界 Launch 檔
    pkg_gazebo_ros = get_package_share_directory('gazebo_ros')
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_gazebo_ros, 'launch', 'gazebo.launch.py')
        )
    )

    # 4. 建立 robot_state_publisher 節點（負責將 URDF 字串轉換成 ROS 2 內部系統看得懂的描述）
    node_robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_description_raw, 'use_sim_time': True}]
    )

    # 5. 建立 spawn_entity 節點（真正負責把機器人實體「變出來」丟進 Gazebo 世界的工具）
    spawn_entity = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', 'robot_description',
                   '-entity', 'omni_planar_robot',
                   '-x', '0.0', # 初始出生座標 X
                   '-y', '0.0', # 初始出生座標 Y
                   '-z', '0.1'],# 初始出生座標 Z
        output='screen'
    )

    # 6. 封裝並回傳所有要啟動的節點與環境
    return LaunchDescription([
        gazebo,
        node_robot_state_publisher,
        spawn_entity
    ])