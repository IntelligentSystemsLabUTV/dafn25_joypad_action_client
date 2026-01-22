#include <iostream>
#include <joypad_actionclient/joy_subscriber.hpp>

JoySub::JoySub()
: Node("joy_subscriber")
{
  subscriber_ = this->create_subscription<JoyMsg>(
    "/joy",
    rclcpp::QoS(10), // TODO: investigate QoS to use
    std::bind(
      &JoySub::msg_callback,
      this,
      std::placeholders::_1));

  RCLCPP_INFO(this->get_logger(), "[JOY SUB] Subscriber initialized");
	return;
}

/***
	Reminder of how /joy messages are structured 
	```bash
		❯ ros2 interface show sensor_msgs/msg/Joy
		# Reports the state of a joystick's axes and buttons.

		# The timestamp is the time at which data is received from the joystick.
		std_msgs/Header header
			builtin_interfaces/Time stamp
				int32 sec
				uint32 nanosec
			string frame_id

		# The axes measurements from a joystick.
		float32[] axes

		# The buttons measurements from a joystick.
		int32[] buttons
	```
***/

// TODO: implement & brainstorm
void JoySub::msg_callback(const JoyMsg::SharedPtr msg)
{
  RCLCPP_INFO(this->get_logger(), "[JOY SUB] Got message");
	std::cout << "[JOY SUB] I'm so sad TwT" << std::endl;
}
