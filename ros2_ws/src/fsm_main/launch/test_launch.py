import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, LifecycleNode

def generate_launch_description():
    use_sim_time_arg = DeclareLaunchArgument(
        'use_sim_time', default_value='true', description='Use simulation clock if true'
    )
    use_sim_time = LaunchConfiguration('use_sim_time')

    # 核心驅動與調度節點
    chassis_pilot_node = Node(
        package='fsm_main', executable='chassis_pilot_node', name='chassis_pilot',
        output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )
    
    # ✨ 核心：將 FSM Manager 加入啟動剧本中
    fsm_manager_node = Node(
        package='fsm_main', executable='main_ctrl_node', name='main_ctrl',
        output='screen', parameters=[{'use_sim_time': use_sim_time}]
    )

    # 實體化 5 個生命週期狀態節點 (不主動激活，交給 FsmManager)
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

    ld = LaunchDescription()
    ld.add_action(use_sim_time_arg)
    ld.add_action(chassis_pilot_node)
    ld.add_action(fsm_manager_node) # ✨
    ld.add_action(mission_one)
    ld.add_action(mission_two)
    ld.add_action(mission_three)
    ld.add_action(mission_four_front)
    ld.add_action(mission_four_back)

    return ld