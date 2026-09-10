# tdk2_main_2026
# how to use
### build docker environment
- `cd Winter-Tutorial/docker`
- `docker compose up`
- `docker start ros2_tutorial`
### open work space
- `docker exec -it ros2_tutorial`
- `cd ~/ros2_ws`
- `colcon build`

```
tdk_ros2_ws
├─ 📁build
│  ├─ 📁interfaces
│  │  ├─ 📁ament_lint_cmake
│  │  ├─ 📁ament_xmllint
│  │  ├─ 📁CMakeFiles
│  │  │  └─ 📄cmake.check_cache
│  │  ├─ 📄CMakeCache.txt
│  │  └─ 📄colcon_build.rc
│  └─ 📁tdk_slam_manager
│     ├─ 📁.cmake
│     │  └─ 📁api
│     │     └─ 📁v1
│     │        └─ 📁reply
│     │           ├─ 📄codemodel-v2-ddee899a0481485a95d8.json
│     │           ├─ 📄directory-.-4e68adfd8677cf9e133d.json
│     │           ├─ 📄index-2026-05-26T15-22-48-0531.json
│     │           ├─ 📄target-laser_angle_filter_node-ab0e61ec85698343c887.json
│     │           ├─ 📄target-tdk_slam_manager_uninstall-cbde6fab86d785c0cbde.json
│     │           └─ 📄target-uninstall-99053c2f0389bfc845f4.json
│     ├─ 📁ament_cmake_core
│     │  ├─ 📁stamps
│     │  │  ├─ 📄ament_prefix_path.sh.stamp
│     │  │  ├─ 📄nameConfig-version.cmake.in.stamp
│     │  │  ├─ 📄nameConfig.cmake.in.stamp
│     │  │  ├─ 📄package.xml.stamp
│     │  │  ├─ 📄package_xml_2_cmake.py.stamp
│     │  │  └─ 📄path.sh.stamp
│     │  ├─ 📄package.cmake
│     │  ├─ 📄tdk_slam_managerConfig-version.cmake
│     │  └─ 📄tdk_slam_managerConfig.cmake
│     ├─ 📁ament_cmake_environment_hooks
│     │  ├─ 📄ament_prefix_path.dsv
│     │  ├─ 📄local_setup.bash
│     │  ├─ 📄local_setup.dsv
│     │  ├─ 📄local_setup.sh
│     │  ├─ 📄local_setup.zsh
│     │  ├─ 📄package.dsv
│     │  └─ 📄path.dsv
│     ├─ 📁ament_cmake_index
│     │  └─ 📁share
│     │     └─ 📁ament_index
│     │        └─ 📁resource_index
│     │           ├─ 📁packages
│     │           │  └─ 📄tdk_slam_manager
│     │           ├─ 📁package_run_dependencies
│     │           │  └─ 📄tdk_slam_manager
│     │           └─ 📁parent_prefix_path
│     │              └─ 📄tdk_slam_manager
│     ├─ 📁ament_cppcheck
│     ├─ 📁ament_flake8
│     ├─ 📁ament_lint_cmake
│     ├─ 📁ament_pep257
│     ├─ 📁ament_uncrustify
│     ├─ 📁ament_xmllint
│     ├─ 📁CMakeFiles
│     │  ├─ 📁laser_angle_filter_node.dir
│     │  │  ├─ 📁src
│     │  │  ├─ 📄build.make
│     │  │  ├─ 📄cmake_clean.cmake
│     │  │  ├─ 📄compiler_depend.make
│     │  │  ├─ 📄compiler_depend.ts
│     │  │  ├─ 📄depend.make
│     │  │  ├─ 📄DependInfo.cmake
│     │  │  ├─ 📄flags.make
│     │  │  ├─ 📄link.txt
│     │  │  └─ 📄progress.make
│     │  ├─ 📁tdk_slam_manager_uninstall.dir
│     │  │  ├─ 📄build.make
│     │  │  ├─ 📄cmake_clean.cmake
│     │  │  ├─ 📄compiler_depend.make
│     │  │  ├─ 📄compiler_depend.ts
│     │  │  ├─ 📄DependInfo.cmake
│     │  │  └─ 📄progress.make
│     │  ├─ 📁uninstall.dir
│     │  │  ├─ 📄build.make
│     │  │  ├─ 📄cmake_clean.cmake
│     │  │  ├─ 📄compiler_depend.make
│     │  │  ├─ 📄compiler_depend.ts
│     │  │  ├─ 📄DependInfo.cmake
│     │  │  └─ 📄progress.make
│     │  ├─ 📄cmake.check_cache
│     │  ├─ 📄CMakeDirectoryInformation.cmake
│     │  ├─ 📄CMakeRuleHashes.txt
│     │  ├─ 📄Makefile.cmake
│     │  ├─ 📄Makefile2
│     │  ├─ 📄progress.marks
│     │  └─ 📄TargetDirectories.txt
│     ├─ 📄CMakeCache.txt
│     ├─ 📄cmake_install.cmake
│     ├─ 📄colcon_build.rc
│     ├─ 📄CTestTestfile.cmake
│     └─ 📄Makefile
├─ 📁cpp_oop
│  ├─ 📁include
│  │  ├─ 📄Airplane.h
│  │  ├─ 📄Car.h
│  │  └─ 📄Vehicle.h
│  ├─ 📁src
│  │  ├─ 📄Airplane.cpp
│  │  ├─ 📄Car.cpp
│  │  ├─ 📄main.cpp
│  │  └─ 📄Vehicle.cpp
│  ├─ 📄Makefile
│  └─ 📄README.md
├─ 📁docker
│  ├─ 📄compose.yaml
│  └─ 📄Dockerfile
├─ 📁install
│  ├─ 📄local_setup.bash
│  ├─ 📄local_setup.ps1
│  ├─ 📄local_setup.sh
│  ├─ 📄local_setup.zsh
│  ├─ 📄setup.bash
│  ├─ 📄setup.ps1
│  ├─ 📄setup.sh
│  ├─ 📄setup.zsh
│  ├─ 📄_local_setup_util_ps1.py
│  └─ 📄_local_setup_util_sh.py
├─ 📁ros2_ws
│  ├─ 📁src
│  │  ├─ 📁fsm_main
│  │  │  ├─ 📁config
│  │  │  │  └─ 📄path.yaml
│  │  │  ├─ 📁include
│  │  │  │  ├─ 📁boost
│  │  │  │  │  └─ 📄sml.hpp
│  │  │  │  └─ 📁fsm_main
│  │  │  │     ├─ 📄chassis_pilot.hpp
│  │  │  │     ├─ 📄fsm_def.hpp
│  │  │  │     └─ 📄fsm_node.hpp
│  │  │  ├─ 📁launch
│  │  │  │  ├─ 📄fsm_launch.xml
│  │  │  │  ├─ 📄gaze_robot_spawn.py
│  │  │  │  └─ 📄test_launch.py
│  │  │  ├─ 📁src
│  │  │  │  ├─ 📄chassis_pilot.cpp
│  │  │  │  ├─ 📄fsm_main.cpp
│  │  │  │  ├─ 📄main_ctrl.cpp
│  │  │  │  ├─ 📄mission_four_back.cpp
│  │  │  │  ├─ 📄mission_four_front.cpp
│  │  │  │  ├─ 📄mission_one.cpp
│  │  │  │  ├─ 📄mission_three.cpp
│  │  │  │  └─ 📄mission_two.cpp
│  │  │  ├─ 📁test
│  │  │  │  └─ 📄test_chassis_pilot.py
│  │  │  ├─ 📁urdf
│  │  │  │  └─ 📄omni_robot.urdf.xacro
│  │  │  ├─ 📄CMakeLists.txt
│  │  │  └─ 📄package.xml
│  │  ├─ 📁interfaces
│  │  │  ├─ 📁action
│  │  │  │  └─ 📄NaviGoal.action
│  │  │  ├─ 📁include
│  │  │  │  └─ 📁interfaces
│  │  │  ├─ 📁msg
│  │  │  │  └─ 📄CustomWaypoint.msg
│  │  │  ├─ 📁src
│  │  │  ├─ 📁srv
│  │  │  │  └─ 📄SetPathCode.srv
│  │  │  ├─ 📄CMakeLists.txt
│  │  │  └─ 📄package.xml
│  │  ├─ 📁micro_ros_setup
│  │  │  ├─ 📁.github
│  │  │  │  ├─ 📁ISSUE_TEMPLATE
│  │  │  │  │  └─ 📄general-issue.md
│  │  │  │  ├─ 📁workflows
│  │  │  │  │  ├─ 📄ci.yml
│  │  │  │  │  ├─ 📄nightly.yml
│  │  │  │  │  └─ 📄pr.yml
│  │  │  │  └─ 📄CODEOWNERS
│  │  │  ├─ 📁.images
│  │  │  │  └─ 📄microros_logo.png
│  │  │  ├─ 📁config
│  │  │  │  ├─ 📁android
│  │  │  │  │  ├─ 📁generic
│  │  │  │  │  │  ├─ 📄build.sh
│  │  │  │  │  │  ├─ 📄client-colcon.meta
│  │  │  │  │  │  ├─ 📄client_uros_packages.repos
│  │  │  │  │  │  ├─ 📄create.sh
│  │  │  │  │  │  ├─ 📄flash.sh
│  │  │  │  │  │  └─ 📄package.xml
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  └─ 📄dev_uros_packages.repos
│  │  │  │  ├─ 📁freertos
│  │  │  │  │  ├─ 📁crazyflie21
│  │  │  │  │  ├─ 📁esp32
│  │  │  │  │  ├─ 📁nucleo_f446re
│  │  │  │  │  ├─ 📁nucleo_f446ze
│  │  │  │  │  ├─ 📁nucleo_f746zg
│  │  │  │  │  ├─ 📁nucleo_f767zi
│  │  │  │  │  ├─ 📁olimex-stm32-e407
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  ├─ 📄dev_uros_packages.repos
│  │  │  │  │  └─ 📄list_apps.sh
│  │  │  │  ├─ 📁generate_lib
│  │  │  │  │  ├─ 📁generic
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  └─ 📄dev_uros_packages.repos
│  │  │  │  ├─ 📁host
│  │  │  │  │  ├─ 📁generic
│  │  │  │  │  │  ├─ 📄build.sh
│  │  │  │  │  │  ├─ 📄client-host-colcon.meta
│  │  │  │  │  │  ├─ 📄client_host_packages.repos
│  │  │  │  │  │  ├─ 📄create.sh
│  │  │  │  │  │  ├─ 📄flash.sh
│  │  │  │  │  │  └─ 📄package.xml
│  │  │  │  │  ├─ 📄client_ros2_packages.txt
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  └─ 📄dev_uros_packages.repos
│  │  │  │  ├─ 📁mbed
│  │  │  │  │  ├─ 📁generic
│  │  │  │  │  │  ├─ 📄board.repos
│  │  │  │  │  │  ├─ 📄build.sh
│  │  │  │  │  │  ├─ 📄client_uros_packages.repos
│  │  │  │  │  │  ├─ 📄create.sh
│  │  │  │  │  │  ├─ 📄flash.sh
│  │  │  │  │  │  ├─ 📄package.xml
│  │  │  │  │  │  └─ 📄supported_platforms
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  ├─ 📄dev_uros_packages.repos
│  │  │  │  │  └─ 📄list_apps.sh
│  │  │  │  ├─ 📁raspbian
│  │  │  │  │  ├─ 📁generic
│  │  │  │  │  │  ├─ 📄build.sh
│  │  │  │  │  │  ├─ 📄client-colcon.meta
│  │  │  │  │  │  ├─ 📄client_uros_packages.repos
│  │  │  │  │  │  ├─ 📄configure.sh
│  │  │  │  │  │  ├─ 📄create.sh
│  │  │  │  │  │  ├─ 📄flash.sh
│  │  │  │  │  │  └─ 📄supported_platforms
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  └─ 📄dev_uros_packages.repos
│  │  │  │  ├─ 📁renesas_ra
│  │  │  │  │  ├─ 📁ra6m5
│  │  │  │  │  │  ├─ 📄board.repos
│  │  │  │  │  │  ├─ 📄build.sh
│  │  │  │  │  │  ├─ 📄client-colcon.meta
│  │  │  │  │  │  ├─ 📄client_uros_packages.repos
│  │  │  │  │  │  ├─ 📄configure.sh
│  │  │  │  │  │  ├─ 📄create.sh
│  │  │  │  │  │  └─ 📄flash.sh
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  ├─ 📄dev_uros_packages.repos
│  │  │  │  │  └─ 📄list_apps.sh
│  │  │  │  ├─ 📁zephyr
│  │  │  │  │  ├─ 📁generic
│  │  │  │  │  ├─ 📄dev_ros2_packages.txt
│  │  │  │  │  ├─ 📄dev_uros_packages.repos
│  │  │  │  │  └─ 📄list_apps.sh
│  │  │  │  ├─ 📄agent_ros2_packages.txt
│  │  │  │  ├─ 📄agent_uros_packages.repos
│  │  │  │  ├─ 📄client_ros2_packages.txt
│  │  │  │  ├─ 📄client_uros_packages.repos
│  │  │  │  └─ 📄utils.sh
│  │  │  ├─ 📁scripts
│  │  │  │  ├─ 📄build_agent.sh
│  │  │  │  ├─ 📄build_firmware.sh
│  │  │  │  ├─ 📄clean_env.sh
│  │  │  │  ├─ 📄component
│  │  │  │  ├─ 📄configure_firmware.sh
│  │  │  │  ├─ 📄create_agent_ws.sh
│  │  │  │  ├─ 📄create_firmware_ws.sh
│  │  │  │  ├─ 📄create_ws.sh
│  │  │  │  ├─ 📄flash_firmware.sh
│  │  │  │  └─ 📄yaml_filter.py
│  │  │  ├─ 📄.gitignore
│  │  │  ├─ 📄3rd-party-licenses.txt
│  │  │  ├─ 📄CHANGELOG.rst
│  │  │  ├─ 📄CMakeLists.txt
│  │  │  ├─ 📄CONTRIBUTING.md
│  │  │  ├─ 📄LICENSE
│  │  │  ├─ 📄NOTICE
│  │  │  ├─ 📄package.xml
│  │  │  └─ 📄README.md
│  │  ├─ 📁uros
│  │  │  ├─ 📁micro-ROS-Agent
│  │  │  │  ├─ 📁.github
│  │  │  │  │  ├─ 📁ISSUE_TEMPLATE
│  │  │  │  │  │  ├─ 📄bug_report.md
│  │  │  │  │  │  └─ 📄general-issue.md
│  │  │  │  │  └─ 📁workflows
│  │  │  │  │     └─ 📄ci.yml
│  │  │  │  ├─ 📁micro_ros_agent
│  │  │  │  │  ├─ 📁bin
│  │  │  │  │  │  ├─ 📄Xml_interface_gen.py
│  │  │  │  │  │  └─ 📄Xml_read_default_profiles.py
│  │  │  │  │  ├─ 📁cmake
│  │  │  │  │  │  └─ 📄SuperBuild.cmake
│  │  │  │  │  ├─ 📁include
│  │  │  │  │  │  └─ 📁agent
│  │  │  │  │  │     ├─ 📁graph_manager
│  │  │  │  │  │     │  ├─ 📄graph_manager.hpp
│  │  │  │  │  │     │  └─ 📄graph_typesupport.hpp
│  │  │  │  │  │     ├─ 📁utils
│  │  │  │  │  │     │  └─ 📄demangle.hpp
│  │  │  │  │  │     └─ 📄Agent.hpp
│  │  │  │  │  ├─ 📁launch
│  │  │  │  │  │  └─ 📄micro_ros_agent_launch.py
│  │  │  │  │  ├─ 📁resource
│  │  │  │  │  │  ├─ 📄rclcpp.xml
│  │  │  │  │  │  └─ 📄StaticValues.xml
│  │  │  │  │  ├─ 📁src
│  │  │  │  │  │  ├─ 📁agent
│  │  │  │  │  │  │  ├─ 📁graph_manager
│  │  │  │  │  │  │  │  ├─ 📄graph_manager.cpp
│  │  │  │  │  │  │  │  └─ 📄graph_typesupport.cpp
│  │  │  │  │  │  │  ├─ 📁utils
│  │  │  │  │  │  │  │  └─ 📄demangle.cpp
│  │  │  │  │  │  │  └─ 📄Agent.cpp
│  │  │  │  │  │  └─ 📄main.cpp
│  │  │  │  │  ├─ 📁utils
│  │  │  │  │  │  └─ 📄__init__.py
│  │  │  │  │  ├─ 📄CHANGELOG.rst
│  │  │  │  │  ├─ 📄CMakeLists.txt
│  │  │  │  │  ├─ 📄package.xml
│  │  │  │  │  └─ 📄README.md
│  │  │  │  ├─ 📁snap
│  │  │  │  │  ├─ 📁hooks
│  │  │  │  │  │  ├─ 📄configure
│  │  │  │  │  │  └─ 📄install
│  │  │  │  │  ├─ 📁local
│  │  │  │  │  │  ├─ 📄fastdds_no_shared_memory.xml
│  │  │  │  │  │  └─ 📄micro-ros-agent-daemon
│  │  │  │  │  └─ 📄snapcraft.yaml
│  │  │  │  ├─ 📄.gitignore
│  │  │  │  ├─ 📄3rd-party-licenses.txt
│  │  │  │  ├─ 📄CONTRIBUTING.md
│  │  │  │  ├─ 📄LICENSE
│  │  │  │  ├─ 📄NOTICE
│  │  │  │  └─ 📄README.md
│  │  │  └─ 📁micro_ros_msgs
│  │  │     ├─ 📁.github
│  │  │     │  └─ 📁ISSUE_TEMPLATE
│  │  │     │     └─ 📄general-issue.md
│  │  │     ├─ 📁msg
│  │  │     │  ├─ 📄Entity.msg
│  │  │     │  ├─ 📄Graph.msg
│  │  │     │  └─ 📄Node.msg
│  │  │     ├─ 📄.gitignore
│  │  │     ├─ 📄CHANGELOG.rst
│  │  │     ├─ 📄CMakeLists.txt
│  │  │     ├─ 📄CONTRIBUTING.md
│  │  │     ├─ 📄LICENSE
│  │  │     ├─ 📄package.xml
│  │  │     └─ 📄README.md
│  │  └─ 📄ros2.repos
│  └─ 📄README.md
├─ 📁tdk_slam_ws
│  ├─ 📁docker
│  │  ├─ 📄docker-compose.yaml
│  │  ├─ 📄Dockerfile
│  │  ├─ 📄docker_run.sh
│  │  └─ 📄entrypoint.sh
│  ├─ 📁src
│  │  ├─ 📁sensor_dep
│  │  │  ├─ 📁ira_laser_tools
│  │  │  │  ├─ 📁launch
│  │  │  │  │  ├─ 📄laserscan_multi_merger.launch
│  │  │  │  │  └─ 📄laserscan_virtualizer.launch
│  │  │  │  ├─ 📁src
│  │  │  │  │  ├─ 📄laserscan_multi_merger.cpp
│  │  │  │  │  └─ 📄laserscan_virtualizer.cpp
│  │  │  │  ├─ 📄.gitignore
│  │  │  │  ├─ 📄CHANGELOG.rst
│  │  │  │  ├─ 📄CMakeLists.txt
│  │  │  │  ├─ 📄LICENSE
│  │  │  │  ├─ 📄package.xml
│  │  │  │  └─ 📄README.md
│  │  │  └─ 📁rplidar_ros
│  │  │     ├─ 📁debian
│  │  │     │  └─ 📄udev
│  │  │     ├─ 📁include
│  │  │     │  ├─ 📄rplidar_node.hpp
│  │  │     │  └─ 📄visibility.h
│  │  │     ├─ 📁launch
│  │  │     │  ├─ 📄rplidar_a1_launch.py
│  │  │     │  ├─ 📄rplidar_a2m12_launch.py
│  │  │     │  ├─ 📄rplidar_a2m7_launch.py
│  │  │     │  ├─ 📄rplidar_a2m8_launch.py
│  │  │     │  ├─ 📄rplidar_a3_launch.py
│  │  │     │  ├─ 📄rplidar_c1_launch.py
│  │  │     │  ├─ 📄rplidar_s1_launch.py
│  │  │     │  ├─ 📄rplidar_s1_tcp_launch.py
│  │  │     │  ├─ 📄rplidar_s2e_launch.py
│  │  │     │  ├─ 📄rplidar_s2_launch.py
│  │  │     │  ├─ 📄rplidar_s3_launch.py
│  │  │     │  ├─ 📄rplidar_t1_launch.py
│  │  │     │  ├─ 📄view_rplidar_a1_launch.py
│  │  │     │  ├─ 📄view_rplidar_a2m12_launch.py
│  │  │     │  ├─ 📄view_rplidar_a2m7_launch.py
│  │  │     │  ├─ 📄view_rplidar_a2m8_launch.py
│  │  │     │  ├─ 📄view_rplidar_a3_launch.py
│  │  │     │  ├─ 📄view_rplidar_c1_launch.py
│  │  │     │  ├─ 📄view_rplidar_s1_launch.py
│  │  │     │  ├─ 📄view_rplidar_s1_tcp_launch.py
│  │  │     │  ├─ 📄view_rplidar_s2e_launch.py
│  │  │     │  ├─ 📄view_rplidar_s2_launch.py
│  │  │     │  ├─ 📄view_rplidar_s3_launch.py
│  │  │     │  └─ 📄view_rplidar_t1_launch.py
│  │  │     ├─ 📁rviz
│  │  │     │  └─ 📄rplidar_ros.rviz
│  │  │     ├─ 📁scripts
│  │  │     │  ├─ 📄create_udev_rules.sh
│  │  │     │  ├─ 📄delete_udev_rules.sh
│  │  │     │  └─ 📄rplidar.rules
│  │  │     ├─ 📁sdk
│  │  │     │  ├─ 📁include
│  │  │     │  │  ├─ 📄rplidar.h
│  │  │     │  │  ├─ 📄rplidar_cmd.h
│  │  │     │  │  ├─ 📄rplidar_driver.h
│  │  │     │  │  ├─ 📄rplidar_protocol.h
│  │  │     │  │  ├─ 📄rptypes.h
│  │  │     │  │  ├─ 📄sl_crc.h
│  │  │     │  │  ├─ 📄sl_lidar.h
│  │  │     │  │  ├─ 📄sl_lidar_cmd.h
│  │  │     │  │  ├─ 📄sl_lidar_driver.h
│  │  │     │  │  ├─ 📄sl_lidar_driver_impl.h
│  │  │     │  │  ├─ 📄sl_lidar_protocol.h
│  │  │     │  │  └─ 📄sl_types.h
│  │  │     │  ├─ 📁src
│  │  │     │  │  ├─ 📁arch
│  │  │     │  │  │  ├─ 📁linux
│  │  │     │  │  │  │  ├─ 📄arch_linux.h
│  │  │     │  │  │  │  ├─ 📄net_serial.cpp
│  │  │     │  │  │  │  ├─ 📄net_serial.h
│  │  │     │  │  │  │  ├─ 📄net_socket.cpp
│  │  │     │  │  │  │  ├─ 📄thread.hpp
│  │  │     │  │  │  │  ├─ 📄timer.cpp
│  │  │     │  │  │  │  └─ 📄timer.h
│  │  │     │  │  │  ├─ 📁macOS
│  │  │     │  │  │  │  ├─ 📄arch_macOS.h
│  │  │     │  │  │  │  ├─ 📄net_serial.cpp
│  │  │     │  │  │  │  ├─ 📄net_serial.h
│  │  │     │  │  │  │  ├─ 📄net_socket.cpp
│  │  │     │  │  │  │  ├─ 📄thread.hpp
│  │  │     │  │  │  │  ├─ 📄timer.cpp
│  │  │     │  │  │  │  └─ 📄timer.h
│  │  │     │  │  │  └─ 📁win32
│  │  │     │  │  │     ├─ 📄arch_win32.h
│  │  │     │  │  │     ├─ 📄net_serial.cpp
│  │  │     │  │  │     ├─ 📄net_serial.h
│  │  │     │  │  │     ├─ 📄net_socket.cpp
│  │  │     │  │  │     ├─ 📄timer.cpp
│  │  │     │  │  │     ├─ 📄timer.h
│  │  │     │  │  │     └─ 📄winthread.hpp
│  │  │     │  │  ├─ 📁dataunpacker
│  │  │     │  │  │  ├─ 📁unpacker
│  │  │     │  │  │  │  ├─ 📄handler_capsules.cpp
│  │  │     │  │  │  │  ├─ 📄handler_capsules.h
│  │  │     │  │  │  │  ├─ 📄handler_hqnode.cpp
│  │  │     │  │  │  │  ├─ 📄handler_hqnode.h
│  │  │     │  │  │  │  ├─ 📄handler_normalnode.cpp
│  │  │     │  │  │  │  └─ 📄handler_normalnode.h
│  │  │     │  │  │  ├─ 📄dataunnpacker_commondef.h
│  │  │     │  │  │  ├─ 📄dataunnpacker_internal.h
│  │  │     │  │  │  ├─ 📄dataunpacker.cpp
│  │  │     │  │  │  ├─ 📄dataunpacker.h
│  │  │     │  │  │  └─ 📄dataupacker_namespace.h
│  │  │     │  │  ├─ 📁hal
│  │  │     │  │  │  ├─ 📄abs_rxtx.h
│  │  │     │  │  │  ├─ 📄assert.h
│  │  │     │  │  │  ├─ 📄byteops.h
│  │  │     │  │  │  ├─ 📄byteorder.h
│  │  │     │  │  │  ├─ 📄event.h
│  │  │     │  │  │  ├─ 📄locker.h
│  │  │     │  │  │  ├─ 📄socket.h
│  │  │     │  │  │  ├─ 📄thread.cpp
│  │  │     │  │  │  ├─ 📄thread.h
│  │  │     │  │  │  ├─ 📄types.h
│  │  │     │  │  │  ├─ 📄util.h
│  │  │     │  │  │  └─ 📄waiter.h
│  │  │     │  │  ├─ 📄rplidar_driver.cpp
│  │  │     │  │  ├─ 📄sdkcommon.h
│  │  │     │  │  ├─ 📄sl_async_transceiver.cpp
│  │  │     │  │  ├─ 📄sl_async_transceiver.h
│  │  │     │  │  ├─ 📄sl_crc.cpp
│  │  │     │  │  ├─ 📄sl_lidarprotocol_codec.cpp
│  │  │     │  │  ├─ 📄sl_lidarprotocol_codec.h
│  │  │     │  │  ├─ 📄sl_lidar_driver.cpp
│  │  │     │  │  ├─ 📄sl_serial_channel.cpp
│  │  │     │  │  ├─ 📄sl_tcp_channel.cpp
│  │  │     │  │  └─ 📄sl_udp_channel.cpp
│  │  │     │  └─ 📄Makefile
│  │  │     ├─ 📁src
│  │  │     │  ├─ 📄rplidar_client.cpp
│  │  │     │  └─ 📄rplidar_node.cpp
│  │  │     ├─ 📄.gitignore
│  │  │     ├─ 📄CHANGELOG.rst
│  │  │     ├─ 📄CMakeLists.txt
│  │  │     ├─ 📄LICENSE
│  │  │     ├─ 📄package.xml
│  │  │     ├─ 📄README.md
│  │  │     ├─ 📄rplidar_A1.png
│  │  │     └─ 📄rplidar_A2.png
│  │  ├─ 📁tdk_nav2_manager
│  │  │  ├─ 📁behavior_tree
│  │  │  │  ├─ 📄bt_nav.xml
│  │  │  │  └─ 📄bt_nav.yaml
│  │  │  ├─ 📁config
│  │  │  │  ├─ 📄dwb_params.yaml
│  │  │  │  ├─ 📄mppi_params.yaml
│  │  │  │  ├─ 📄smac_2d_params.yaml
│  │  │  │  ├─ 📄smac_lattice_params.yaml
│  │  │  │  ├─ 📄tdk_nav2_params.yaml
│  │  │  │  └─ 📄theta_star_params.yaml
│  │  │  ├─ 📁costmap_config
│  │  │  │  ├─ 📄global_costmap.yaml
│  │  │  │  └─ 📄local_costmap.yaml
│  │  │  ├─ 📁launch
│  │  │  │  └─ 📄nav_launch.py
│  │  │  ├─ 📄CMakeLists.txt
│  │  │  └─ 📄package.xml
│  │  └─ 📁tdk_slam_manager
│  │     ├─ 📁cartographer_config
│  │     │  ├─ 📄cartographer_2d.lua
│  │     │  └─ 📄localization.lua
│  │     ├─ 📁config
│  │     │  ├─ 📄amcl_params.yaml
│  │     │  ├─ 📄ekf_config.yaml
│  │     │  ├─ 📄laser_merger_params.yaml
│  │     │  ├─ 📄mapper_params_online_async.yaml
│  │     │  ├─ 📄robot_params.yaml
│  │     │  └─ 📄slam_toolbox_params.yaml
│  │     ├─ 📁launch
│  │     │  ├─ 📄maze_world_launch.py
│  │     │  ├─ 📄sim_spawn_launch.py
│  │     │  └─ 📄spawn_launch.py
│  │     ├─ 📁maps
│  │     │  ├─ 📄carto_map_0.pbstream
│  │     │  ├─ 📄carto_map_0.pgm
│  │     │  ├─ 📄carto_map_0.yaml
│  │     │  ├─ 📄slam_map_0.data
│  │     │  └─ 📄slam_map_0.posegraph
│  │     ├─ 📁sim
│  │     │  ├─ 📁models
│  │     │  │  └─ 📁maze
│  │     │  │     ├─ 📁meshes
│  │     │  │     │  ├─ 📄ground.dae
│  │     │  │     │  └─ 📄ground.stl
│  │     │  │     ├─ 📄model.config
│  │     │  │     └─ 📄model.sdf
│  │     │  └─ 📁worlds
│  │     │     └─ 📄maze.world
│  │     ├─ 📁src
│  │     │  ├─ 📄laser_angle_filter.cpp
│  │     │  └─ 📄robot_pose_publisher.cpp
│  │     ├─ 📁urdf
│  │     │  └─ 📄sensors.urdf.xacro
│  │     ├─ 📄CMakeLists.txt
│  │     └─ 📄package.xml
│  ├─ 📄.gitignore
│  ├─ 📄experiment.md
│  └─ 📄README.md
├─ 📄.gitignore
└─ 📄README.md
```