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
    use_sim_time = LaunchConfiguration('use_sim_time')
    is_mirrored_arg = DeclareLaunchArgument(
        'is_mirrored', default_value='false', description='mirror map'
    )
    is_mirrored = LaunchConfiguration('is_mirrored')
# ---------------- 1. micro-ROS Drive 參數 ----------------
    drive_microros_transport_arg = DeclareLaunchArgument(
        'drive_microros_transport', default_value='serial',
        description='micro-ROS transport type for drive: serial, udp4, tcp4'
    )
    drive_microros_port_arg = DeclareLaunchArgument(
        'drive_microros_port', default_value='/dev/ttyUSB3',
        description='Serial port or UDP/TCP port for drive agent'
    )
    drive_microros_baudrate_arg = DeclareLaunchArgument(
        'drive_microros_baudrate', default_value='921600',
        description='Baudrate for drive serial transport'
    )

    # ---------------- 2. micro-ROS Mission 參數 ----------------
    mission_microros_transport_arg = DeclareLaunchArgument(
        'mission_microros_transport', default_value='serial',
        description='micro-ROS transport type for mission: serial, udp4, tcp4'
    )
    mission_microros_port_arg = DeclareLaunchArgument(
        'mission_microros_port', default_value='/dev/ttyUSB0',
        description='Serial port or UDP/TCP port for mission agent'
    )
    mission_microros_baudrate_arg = DeclareLaunchArgument(
        'mission_microros_baudrate', default_value='921600',
        description='Baudrate for mission serial transport'
    )

    # ---------------- 3. Launch Configurations ----------------
    drive_microros_transport = LaunchConfiguration('drive_microros_transport')
    drive_microros_port = LaunchConfiguration('drive_microros_port')
    drive_microros_baudrate = LaunchConfiguration('drive_microros_baudrate')

    mission_microros_transport = LaunchConfiguration('mission_microros_transport')
    mission_microros_port = LaunchConfiguration('mission_microros_port')
    mission_microros_baudrate = LaunchConfiguration('mission_microros_baudrate')

    # ---------------- 4. micro-ROS Agent 節點 ----------------
    # Drive Agent 節點
    microros_agent_drive_node = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='micro_ros_agent_drive',
        output='screen',
        arguments=[
            drive_microros_transport,
            '--dev', drive_microros_port,
            '-b', drive_microros_baudrate
        ]
    )

    # Mission Agent 節點
    microros_agent_mission_node = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='micro_ros_agent_mission',
        output='screen',
        arguments=[
            mission_microros_transport,
            '--dev', mission_microros_port,
            '-b', mission_microros_baudrate
        ]
    )

    # ---------------- 核心驅動與調度節點 ----------------
    chassis_pilot_node = Node(
        package='chassis_pilot', executable='chassis_pilot_node', name='chassis_pilot',
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
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}, {'is_mirrored': is_mirrored}]
    )
    mission_two = LifecycleNode(
        package='fsm_main', executable='mission_two_node', name='mission_two_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}, {'is_mirrored': is_mirrored}]
    )
    mission_three = LifecycleNode(
        package='fsm_main', executable='mission_three_node', name='mission_three_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}, {'is_mirrored': is_mirrored}]
    )
    mission_four_front = LifecycleNode(
        package='fsm_main', executable='mission_four_front_node', name='mission_four_front_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}, {'is_mirrored': is_mirrored}]
    )
    mission_four_back = LifecycleNode(
        package='fsm_main', executable='mission_four_back_node', name='mission_four_back_node',
        namespace='', output='screen', parameters=[{'use_sim_time': use_sim_time}, {'is_mirrored': is_mirrored}]
    )

    # ---------------- 組裝 LaunchDescription ----------------
    ld = LaunchDescription()
    
    # 加入參數
    ld.add_action(use_sim_time_arg)
    ld.add_action(is_mirrored_arg)

    ld.add_action(drive_microros_transport_arg)
    ld.add_action(drive_microros_port_arg)
    ld.add_action(drive_microros_baudrate_arg)
    ld.add_action(mission_microros_transport_arg)
    ld.add_action(mission_microros_port_arg)
    ld.add_action(mission_microros_baudrate_arg)

    # 加入節點
    ld.add_action(microros_agent_mission_node)
    ld.add_action(microros_agent_drive_node)
    ld.add_action(chassis_pilot_node)
    ld.add_action(fsm_manager_node)
    ld.add_action(mission_one)
    ld.add_action(mission_two)
    ld.add_action(mission_three)
    ld.add_action(mission_four_front)
    ld.add_action(mission_four_back)

    return ld