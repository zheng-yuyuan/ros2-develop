#define _USE_MATH_DEFINES
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <cmath>
#include <random>
using namespace std::chrono_literals;

class SignalGenerator : public rclcpp::Node
{
public:
  SignalGenerator() : Node("signal_generator"), gen(std::random_device{}())
  {
    pub_ = create_publisher<std_msgs::msg::Float64>("raw_signal",10);
    timer_ = create_wall_timer(1.0s/600, std::bind(&SignalGenerator::cb, this));
    amp = 1.0; freq =20.0; noise_std = amp*0.01; t=0.0;
  }
private:
  void cb(){
    std_msgs::msg::Float64 msg;
    double sine = amp * sin(2*M_PI*freq*t);
    std::normal_distribution<double> dist(0, noise_std);
    msg.data = sine + dist(gen);
    pub_->publish(msg);
    t += 1.0/600.0;
  }
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  double amp,freq,noise_std,t;
  std::mt19937 gen;
};
int main(int argc, char** argv){
  rclcpp::init(argc,argv);
  rclcpp::spin(std::make_shared<SignalGenerator>());
  rclcpp::shutdown();
  return 0;
}
