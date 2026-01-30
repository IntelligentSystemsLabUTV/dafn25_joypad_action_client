#include <iostream>
#include <joypad_actionclient/joy_subscriber.hpp>

JoySub::JoySub()
: Node("joy_subscriber")
{
  subscriber_ = this->create_subscription<JoyMsg>(
    "/joy",
    rclcpp::QoS(10), // TODO: investigate QoS to use. Do we care?
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

void JoySub::msg_callback(const JoyMsg::SharedPtr msg)
{
	// NOTE: this is only a little demo to show you how'd you extract specific fields from the msg struct (structs, pointers n stuff)
	// Bear in mind that `buttons` is an array so you might have to allocate (otherwise, statically define e.g. `float32_t array[12];`?) an array whose length matches (in the worst case scenario) the one of the `buttons` array. In other words you gotta figure out how to get the length of `buttons` and use it to define an array of your own, for the indices of pressed buttons at any given time
	// Once you've filled that array in we gotta decide what to do w em. I was thinking that if more than 1 button was pressed at time T, then we ignore the inputs as a whole (this is especially useful if team X wanted to implement actions that require key combos or if they don't want to handle priorities among the buttons of interest)
	// If the pressed button is only one, only then do you check in the parameters if among arm, disarm, takeoff and landing there's a matching `button` parameter
  int32_t secs 			= msg->header.stamp.sec;
  uint32_t nanosecs 	= msg->header.stamp.nanosec;
	
	RCLCPP_INFO(this->get_logger(), "[JOY SUB] Got message at nanoseconds=\"%u\"", secs * (uint32_t) 1e9 + nanosecs);
}
