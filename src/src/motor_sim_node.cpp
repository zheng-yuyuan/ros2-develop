#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

class MotorSimNode : public rclcpp::Node
{
public:
    MotorSimNode() : Node("motor_sim_node")
    {
        // ===== 参数设置（二阶电机模型 J B TL）=====
        J_ = 0.01;    // 转动惯量
        B_ = 0.05;    // 粘性阻尼系数
        T_L_ = 0.0;   // 负载扰动力矩，这里先设0
        dt_ = 0.001;  // 仿真步长：1ms，状态更新频率=1000Hz
        ctrl_freq_ = 100; // ROS消息收发控制频率100Hz

        // 状态变量 x=[theta; omega]
        theta_ = 0.0;
        omega_ = 0.0;
        torque_cmd_ = 0.0;

        // 订阅：接收力矩指令 /cmd_torque
        torque_sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/cmd_torque", 10, std::bind(&MotorSimNode::torque_callback, this, std::placeholders::_1));

        // 发布：电机状态 [theta, omega]
        state_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>("/motor_state", 10);

        // 仿真定时器：1ms跑一次动力学积分（欧拉前向）
        sim_timer_ = this->create_wall_timer(
            std::chrono::duration<double>(dt_), std::bind(&MotorSimNode::sim_step, this));

        // 状态发布定时器：100Hz向外输出数据
        pub_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10), std::bind(&MotorSimNode::publish_state, this));

        RCLCPP_INFO(this->get_logger(), "Motor Simulator started. dt=%.3f s, sim_freq=%.0f Hz", dt_, 1.0/dt_);
    }

private:
    // 力矩订阅回调
    void torque_callback(const std_msgs::msg::Float64::SharedPtr msg)
    {
        torque_cmd_ = msg->data;
    }

    // 动力学单步积分 欧拉法
    void sim_step()
    {
        // J*dw/dt = Te - TL - B*w
        double dwdt = (torque_cmd_ - T_L_ - B_ * omega_) / J_;
        omega_ += dwdt * dt_;
        theta_ += omega_ * dt_;
    }

    // 发布角度+角速度
    void publish_state()
    {
        std_msgs::msg::Float64MultiArray msg;
        msg.data.push_back(theta_);
        msg.data.push_back(omega_);
        state_pub_->publish(msg);
    }

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr torque_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr state_pub_;
    rclcpp::TimerBase::SharedPtr sim_timer_;
    rclcpp::TimerBase::SharedPtr pub_timer_;

    double J_, B_, T_L_;
    double dt_;
    double theta_, omega_, torque_cmd_;
    double ctrl_freq_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorSimNode>());
    rclcpp::shutdown();
    return 0;
}
