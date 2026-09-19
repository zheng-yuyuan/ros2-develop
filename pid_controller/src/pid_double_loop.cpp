#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/float64.hpp"
#include <cmath>

class DoubleLoopPid : public rclcpp::Node
{
public:
    DoubleLoopPid() : Node("double_loop_pid")
    {
        sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            "/motor_state",
            10,
            std::bind(&DoubleLoopPid::state_callback, this, std::placeholders::_1));
        pub_ = this->create_publisher<std_msgs::msg::Float64>("/motor_control",10);

        target_pos_ = 1.0;
        kp_pos_ = 8.0;
        ki_pos_ = 0.0;
        kd_pos_ = 0.5;
        kp_vel_ = 2.0;
        ki_vel_ = 0.0;
        kd_vel_ = 0.01;

        integral_pos_ = 0.0;
        integral_vel_ = 0.0;
        last_err_pos_ = 0.0;
        last_err_vel_ = 0.0;
        last_time_ = this->get_clock()->now();
    }
private:
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_;

    double target_pos_;
    double kp_pos_, ki_pos_, kd_pos_;
    double integral_pos_;
    double last_err_pos_;

    double kp_vel_, ki_vel_, kd_vel_;
    double integral_vel_;
    double last_err_vel_;

    rclcpp::Time last_time_;

    void state_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        auto now = this->get_clock()->now();
        double dt = (now - last_time_).seconds();
        last_time_ = now;
        if(dt < 1e-6) return;

        double pos = msg->data[0];
        double vel = msg->data[1];
        double err_pos = target_pos_ - pos;

        // 位置环PID
        integral_pos_ += err_pos * dt;
        // 积分限幅
        if(integral_pos_ > 10) integral_pos_ = 10;
        if(integral_pos_ < -10) integral_pos_ = -10;

        double d_pos = (err_pos - last_err_pos_) / dt;
        double target_vel = kp_pos_ * err_pos + ki_pos_ * integral_pos_ + kd_pos_ * d_pos;
        last_err_pos_ = err_pos;

        // 速度环PID
        double err_vel = target_vel - vel;
        integral_vel_ += err_vel * dt;
        if(integral_vel_ > 10) integral_vel_ = 10;
        if(integral_vel_ < -10) integral_vel_ = -10;

        double d_vel = (err_vel - last_err_vel_) / dt;
        double u = kp_vel_ * err_vel + ki_vel_ * integral_vel_ + kd_vel_ * d_vel;
        last_err_vel_ = err_vel;

        // 输出限幅
        if(u > 10.0) u =10.0;
        if(u < -10.0) u =-10.0;

        std_msgs::msg::Float64 out;
        out.data = u;
        pub_->publish(out);

        // 控制台打印，方便直接看数据
        RCLCPP_INFO(this->get_logger(),
            "pos:%.3f, vel:%.3f, target_vel:%.3f, u:%.3f",
            pos, vel, target_vel, u);
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DoubleLoopPid>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
