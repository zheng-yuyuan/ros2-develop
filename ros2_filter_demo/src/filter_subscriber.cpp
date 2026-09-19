#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <vector>
#include <algorithm>
using std::placeholders::_1;

class FilterSub : public rclcpp::Node
{
public:
  FilterSub() : Node("filter_sub")
  {
    sub_ = create_subscription<std_msgs::msg::Float64>("raw_signal",10,std::bind(&FilterSub::cb,this,_1));
    pub_low = create_publisher<std_msgs::msg::Float64>("lowpass_signal",10);
    pub_med = create_publisher<std_msgs::msg::Float64>("median_signal",10);
    alpha = 0.08; win_size =5; low_val=0;
  }
private:
  void cb(const std_msgs::msg::Float64::SharedPtr msg){
    double raw = msg->data;
    //低通滤波
    low_val = alpha * raw + (1-alpha)*low_val;
    std_msgs::msg::Float64 low_msg; low_msg.data = low_val;
    pub_low->publish(low_msg);
    //中值滤波
    buf.push_back(raw);
    if(buf.size()>win_size) buf.erase(buf.begin());
    double med = raw;
    if(buf.size()==win_size){
      auto tmp = buf; std::sort(tmp.begin(),tmp.end());
      med = tmp[win_size/2];
    }
    std_msgs::msg::Float64 med_msg; med_msg.data = med;
    pub_med->publish(med_msg);
  }
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_low,pub_med;
  double alpha,low_val; int win_size;
  std::vector<double> buf;
};
int main(int argc, char** argv){
  rclcpp::init(argc,argv);
  rclcpp::spin(std::make_shared<FilterSub>());
  rclcpp::shutdown();
  return 0;
}
