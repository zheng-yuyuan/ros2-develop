cmake_minimum_required(VERSION 3.10)
project(pid_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(pid_node src/pid_double_loop.cpp)

target_include_directories(pid_node
  PRIVATE
  ${rclcpp_INCLUDE_DIRS}
  ${std_msgs_INCLUDE_DIRS}
)

target_link_libraries(pid_node
  ${rclcpp_LIBRARIES}
  ${std_msgs_LIBRARIES}
)

install(TARGETS
  pid_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
