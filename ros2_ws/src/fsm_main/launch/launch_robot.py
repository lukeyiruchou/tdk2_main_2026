import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, LifecycleNode

def generate_launch_description():
    # ---------------- 宣告 Launch 參數 ----------------
    use_sim_time_arg = DeclareLaunchArgument(
        'use_sim_time', default_value='false', description='Use simulation clock if true'
    )
    
    # micro-ROS 相關參數（預設使用 Serial /dev/ttyUSB0, Baudrate 115200）
    microros_transport_arg = DeclareLaunchArgument(
        'microros_transport', default_value='serial',
        description='micro-ROS transport type: serial, udp4, tcp4'
    )
    microros_port_arg = DeclareLaunchArgument(
        'microros_port', default_value='/dev/ttyUSB0',
        description='Serial port device or UDP/TCP port'
    )
    microros_baudrate_arg = DeclareLaunchArgument(
        'microros_baudrate', default_value='115200',
        description='Baudrate for serial transport'
    )

    use_sim_time = LaunchConfiguration('use_sim_time')
    microros_transport = LaunchConfiguration('microros_transport')
    microros_port = LaunchConfiguration('microros_port')
    microros_baudrate = LaunchConfiguration('microros_baudrate')

    # ---------------- micro-ROS Agent 節點 ----------------
    microros_agent_node = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='micro_ros_agent',
        output='screen',
        arguments=[
            microros_transport,
            '--dev', microros_port,
            '-b', microros_baudrate
        ]
    )

    # ---------------- 核心驅動與調度節點 ----------------
    chassis_pilot_node = Node(
        package='fsm_main', executable='chassis_pilot_node', name='chassis_pilot',
        output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )
    
    # ✨ 核心：將 FSM Manager 加入啟動劇本中
    fsm_manager_node = Node(
        package='fsm_main', executable='main_ctrl_node', name='main_ctrl',
        output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )

    # ---------------- 實體化 5 個生命週期狀態節點 ----------------
    mission_one = LifecycleNode(
        package='fsm_main', executable='mission_one_node', name='mission_one_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )
    mission_two = LifecycleNode(
        package='fsm_main', executable='mission_two_node', name='mission_two_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )
    mission_three = LifecycleNode(
        package='fsm_main', executable='mission_three_node', name='mission_three_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )
    mission_four_front = LifecycleNode(
        package='fsm_main', executable='mission_four_front_node', name='mission_four_front_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )
    mission_four_back = LifecycleNode(
        package='fsm_main', executable='mission_four_back_node', name='mission_four_back_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )

    # ---------------- 組裝 LaunchDescription ----------------
    ld = LaunchDescription()
    
    # 加入參數
    ld.add_action(use_sim_time_arg)
    ld.add_action(microros_transport_arg)
    ld.add_action(microros_port_arg)
    ld.add_action(microros_baudrate_arg)

    # 加入節點
    ld.add_action(microros_agent_node)
    ld.add_action(chassis_pilot_node)
    ld.add_action(fsm_manager_node)
    ld.add_action(mission_one)
    ld.add_action(mission_two)
    ld.add_action(mission_three)
    ld.add_action(mission_four_front)
    ld.add_action(mission_four_back)

    return ld