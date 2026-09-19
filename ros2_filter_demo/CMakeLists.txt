cmake_minimum_required(VERSION 3.10)
project(ros2_filter_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# 节点1：信号发生器
add_executable(signal_generator src/signal_generator.cpp)
target_link_libraries(signal_generator rclcpp::rclcpp std_msgs::std_msgs)

# 节点2：滤波订阅
add_executable(filter_subscriber src/filter_subscriber.cpp)
target_link_libraries(filter_subscriber rclcpp::rclcpp std_msgs::std_msgs)

# 安装规则
install(TARGETS
  signal_generator
  filter_subscriber
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
