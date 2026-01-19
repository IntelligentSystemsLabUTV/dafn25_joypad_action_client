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

# NOTE: here I'm adding the packages that are included for both dua_aircraft_interfaces and dua_hardware_interfaces, which are required to implement all actions required by project delivery
# most of these could probably be removed
find_package(dua_hardware_interfaces REQUIRED)
find_package(dua_aircraft_interfaces REQUIRED)
find_package(dua_common_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)

set(ACTION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/action")
file(GLOB ACTION_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${ACTION_DIR}/*.action")

# NOTE: here ends all lines additional to the ones already given by the shared repo

# Install headers
install(
  DIRECTORY include/
  DESTINATION include)

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

ament_export_include_directories(include)
ament_export_dependencies(rclcpp)

ament_package()
