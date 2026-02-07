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

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(action_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(simple_actionclient_cpp REQUIRED)
find_package(params_manager_cpp REQUIRED)

find_package(dua_hardware_interfaces REQUIRED)
find_package(dua_aircraft_interfaces REQUIRED)
find_package(dua_common_interfaces REQUIRED)

set(PARAMS_YAML_FILE "${CMAKE_CURRENT_SOURCE_DIR}/config/params.yaml")

set(GENERATED_PARAMS_CPP "src/init_parameters.cpp")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src")

# Specifica da params_manager_cpp 
generate_init_parameters(
  YAML_FILE "${PARAMS_YAML_FILE}"
  OUT_FILE "${GENERATED_PARAMS_CPP}"
)

add_library(joypad_actionclient_component SHARED
  src/the_actionclient.cpp
  "${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_PARAMS_CPP}"
)

target_include_directories(joypad_actionclient_component PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_compile_features(joypad_actionclient_component PUBLIC c_std_99 cxx_std_17)

ament_target_dependencies(
  joypad_actionclient_component
  rclcpp
  rclcpp_action
  rclcpp_components
  dua_common_interfaces
  dua_hardware_interfaces
  dua_aircraft_interfaces
  simple_actionclient_cpp
  params_manager_cpp
  sensor_msgs
  action_msgs
)

rclcpp_components_register_nodes(joypad_actionclient_component
  "joypad_actionclient::JoyActionClient"
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS joypad_actionclient_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
