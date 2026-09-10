#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class Listener : public rclcpp::Node
{
public:
  Listener() : Node("listener")
  {
    sub_ = this->create_subscription<std_msgs::msg::String>(
      "chatter", 10, std::bind(&Listener::cb, this, std::placeholders::_1)
    );
  }

private:
  void cb(const std_msgs::msg::String::SharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "I heard: %s", msg->data.c_str());
  }
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<Listener>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
