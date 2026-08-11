cmake_minimum_required(VERSION 3.8)
project(fsm_main)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
include_directories(include)

find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(lifecycle_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Boost REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(nav2_msgs REQUIRED)

find_package(interfaces REQUIRED)



add_executable(main_ctrl_node 
  src/main_ctrl.cpp 
  src/fsm_main.cpp
)
ament_target_dependencies(main_ctrl_node rclcpp std_msgs lifecycle_msgs)


add_executable(mission_one_node src/mission_one.cpp)
ament_target_dependencies(mission_one_node rclcpp rclcpp_lifecycle rclcpp_action interfaces ament_index_cpp nav2_msgs)
target_link_libraries(mission_one_node ${YAML_CPP_LIBRARIES}) # ✨ 補上這行

# ----- Mission 2 -----
add_executable(mission_two_node src/mission_two.cpp)
ament_target_dependencies(mission_two_node rclcpp rclcpp_lifecycle rclcpp_action interfaces ament_index_cpp nav2_msgs)
target_link_libraries(mission_two_node ${YAML_CPP_LIBRARIES}) # ✨ 補上這行

# ----- Mission 3 -----
add_executable(mission_three_node src/mission_three.cpp)
ament_target_dependencies(mission_three_node rclcpp rclcpp_lifecycle rclcpp_action interfaces ament_index_cpp nav2_msgs)
target_link_libraries(mission_three_node ${YAML_CPP_LIBRARIES}) # ✨ 補上這行

# ----- Mission 4 Front -----
add_executable(mission_four_front_node src/mission_four_front.cpp)
ament_target_dependencies(mission_four_front_node rclcpp rclcpp_lifecycle rclcpp_action interfaces ament_index_cpp nav2_msgs)
target_link_libraries(mission_four_front_node ${YAML_CPP_LIBRARIES}) # ✨ 補上這行

# ----- Mission 4 Back -----
add_executable(mission_four_back_node src/mission_four_back.cpp)
ament_target_dependencies(mission_four_back_node rclcpp rclcpp_lifecycle rclcpp_action interfaces ament_index_cpp nav2_msgs)
target_link_libraries(mission_four_back_node ${YAML_CPP_LIBRARIES}) # ✨ 補上這行


add_executable(chassis_pilot_node 
  src/chassis_pilot.cpp
)
ament_target_dependencies(chassis_pilot_node
  rclcpp
  rclcpp_action
  geometry_msgs
  nav_msgs
  std_msgs
  interfaces
)




# 安裝
install(TARGETS
  main_ctrl_node
  mission_one_node
  mission_two_node
  mission_three_node
  mission_four_front_node
  mission_four_back_node
  chassis_pilot_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  urdf
  config
  DESTINATION share/${PROJECT_NAME}
)

set(FSM_INCLUDE_DIRS
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${Boost_INCLUDE_DIRS}
)

target_include_directories(mission_one_node PUBLIC ${FSM_INCLUDE_DIRS})
target_include_directories(mission_two_node PUBLIC ${FSM_INCLUDE_DIRS})
target_include_directories(mission_three_node PUBLIC ${FSM_INCLUDE_DIRS})
target_include_directories(mission_four_front_node PUBLIC ${FSM_INCLUDE_DIRS})
target_include_directories(mission_four_back_node PUBLIC ${FSM_INCLUDE_DIRS})
target_include_directories(main_ctrl_node PUBLIC ${FSM_INCLUDE_DIRS})



if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
