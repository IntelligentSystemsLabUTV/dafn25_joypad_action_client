# TODO: investigate why doing . install/local_setup.zsh followed by ros2 launch joypad_actionclient joypad_actionclient.launch.py and by ros2 node info /joypad_actionclient doesn't show parameters nor client services even though we did install those
cmake_minimum_required(VERSION 3.8)
project(joypad_actionclient)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(action_msgs REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)

find_package(dua_hardware_interfaces REQUIRED)	# Need this for the Arm, Disarm actions
find_package(dua_aircraft_interfaces REQUIRED)	# Need this for the Takeoff, Landing actions
find_package(sensor_msgs REQUIRED)									# Need this for the Joy topic subscriber

# May be removed ?
find_package(dua_common_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)

install(
  DIRECTORY include/
  DESTINATION include)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME})

add_executable(joypad_actionclient
				src/the_actionclient.cpp	# the main source
				src/joy_subscriber.cpp		# subscriber to the /joy topic
				src/arm_client.cpp				# implementations of the individual action-clients
				src/disarm_client.cpp			
				src/landing_client.cpp		
				src/takeoff_client.cpp		
				) # TODO: add after src/joypad_actionclient.cpp all other cpp files in src
target_include_directories(joypad_actionclient PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(joypad_actionclient PUBLIC c_std_99 cxx_std_17)
ament_target_dependencies(
  joypad_actionclient
  rclcpp
  rclcpp_action
	dua_common_interfaces
	dua_hardware_interfaces
	dua_aircraft_interfaces
	sensor_msgs
)
# Install executables
install(TARGETS joypad_actionclient 
  DESTINATION lib/${PROJECT_NAME})

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
