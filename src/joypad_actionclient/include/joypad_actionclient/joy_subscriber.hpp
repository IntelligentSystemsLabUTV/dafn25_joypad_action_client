#include <rclcpp/rclcpp.hpp> 

#include <sensor_msgs/msg/joy.hpp>

using JoyMsg = sensor_msgs::msg::Joy;

class JoySub : public rclcpp::Node
{
public:
    JoySub();

private:
  rclcpp::Subscription<JoyMsg>::SharedPtr subscriber_;
  void msg_callback(const JoyMsg::SharedPtr joy_msg);
};
