#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
using namespace std::chrono_literals;

class TorqueTestNode : public rclcpp::Node
{
public:
    TorqueTestNode() : Node("torque_test_node"), t_(0.0)
    {
        pub_ = this->create_publisher<std_msgs::msg::Float64>("/cmd_torque", 10);
        timer_ = this->create_wall_timer(10ms, std::bind(&TorqueTestNode::timer_cb, this));
        RCLCPP_INFO(this->get_logger(), "Torque Test Source: Step torque input");
    }

private:
    void timer_cb()
    {
        std_msgs::msg::Float64 msg;
        // 阶跃测试：t>0.2s，输出 2 N·m力矩；之前为0
        if(t_ > 0.2){
            msg.data = 2.0;
        }else{
            msg.data = 0.0;
        }
        pub_->publish(msg);
        t_ += 0.01;
    }
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    double t_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TorqueTestNode>());
    rclcpp::shutdown();
    return 0;
}
