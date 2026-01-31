cmake_minimum_required(VERSION 3.8)
project(joypad_actionclient)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(action_msgs REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(simple_actionclient_cpp REQUIRED)
find_package(params_manager_cpp REQUIRED)

find_package(dua_hardware_interfaces REQUIRED)  # Need this for the Arm, Disarm actions
find_package(dua_aircraft_interfaces REQUIRED)  # Need this for the Takeoff, Landing actions
find_package(sensor_msgs REQUIRED)        # Need this for the Joy topic subscriber

# May be removed ?
find_package(dua_common_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)

# Generate parameters source code
# TODO: understand why this does actually nothing. I read the README.me file in the params_manager_cpp repo but like... should it be in this line, in this file, in this directory? Do I have to move it elsewhere? Am I missing something in the launch file or in the JoypadActionclient node? 
## generate_init_parameters(
##   YAML_FILE "src/params.yaml"
##   OUT_FILE "src/init_params.cpp"
## )

install(
  DIRECTORY include/
  DESTINATION include)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME})

add_executable(joypad_actionclient
  src/the_actionclient.cpp
)

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
  simple_actionclient_cpp
  params_manager_cpp
  sensor_msgs
)

install(TARGETS joypad_actionclient 
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
