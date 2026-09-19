cmake_minimum_required(VERSION 3.10)
project(motor_sim)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# 电机模拟器节点
add_executable(motor_sim_node src/motor_sim_node.cpp)
target_link_libraries(motor_sim_node
  rclcpp::rclcpp
  ${std_msgs_TARGETS}
)

# 力矩测试源节点
add_executable(torque_test_node src/torque_test_node.cpp)
target_link_libraries(torque_test_node
  rclcpp::rclcpp
  ${std_msgs_TARGETS}
)

install(TARGETS
  motor_sim_node
  torque_test_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
