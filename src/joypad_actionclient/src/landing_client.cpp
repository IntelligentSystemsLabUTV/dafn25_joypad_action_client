#include <chrono>
#include <sstream>
#include <stdexcept>

#include <joypad_actionclient/landing_client.hpp>

LandingClient::LandingClient()
: Node("landing_client")
{
  client_ = rclcpp_action::create_client<Landing>(
    this,
    "landing_client");

  client_opts_.goal_response_callback = std::bind(
    &LandingClient::goal_response_clbk,
    this,
    std::placeholders::_1);

  client_opts_.feedback_callback = std::bind(
    &LandingClient::feedback_callback,
    this,
    std::placeholders::_1,
    std::placeholders::_2);

  client_opts_.result_callback = std::bind(
    &LandingClient::result_callback,
    this,
    std::placeholders::_1);

  RCLCPP_INFO(this->get_logger(), "[LANDING] Node initialized");
}

/***
	Reminder of how Landing actions are handled
	```bash
		❯ ros2 interface show dua_aircraft_interfaces/action/Landing
		# Landing operation.
		#
		# Roberto Masocco <r.masocco@dotxautomation.com>
		#
		# June 6, 2023

		# GOAL
		# Enforce landing operation without initial descent
		bool descend

		# Decision position
		geometry_msgs/PointStamped minimums
			std_msgs/Header header
				builtin_interfaces/Time stamp
					int32 sec
					uint32 nanosec
				string frame_id
			Point point
				float64 x
				float64 y
				float64 z
		---
		# RESULT
		# Operation result
		dua_common_interfaces/CommandResultStamped result
			#
			#
			std_msgs/Header header
				builtin_interfaces/Time stamp
					int32 sec
					uint32 nanosec
				string frame_id
			uint8 SUCCESS=1 #
			uint8 FAILED=2  #
			uint8 ERROR=3   #
			uint8 result
			string error_msg
		---
		# FEEDBACK
		# Current pose
		geometry_msgs/PoseStamped pose
			std_msgs/Header header
				builtin_interfaces/Time stamp
					int32 sec
					uint32 nanosec
				string frame_id
			Pose pose
				Point position
					float64 x
					float64 y
					float64 z
				Quaternion orientation
					float64 x 0
					float64 y 0
					float64 z 0
					float64 w 1
```
***/

// TODO: brainstorm logic and implement
void LandingClient::goal_response_clbk(LandingGoalHandleSharedPtr goal_handle){ return; }

// TODO: brainstorm logic and implement
void LandingClient::feedback_callback(LandingGoalHandleSharedPtr goal_handle,const std::shared_ptr<const Landing::Feedback> feedback){ return; }

// TODO: brainstorm logic and implement
void LandingClient::cancel_callback(CancelResponseSharedPtr cancel_resp){ return; }

// TODO: brainstorm logic and implement
void LandingClient::result_callback(const LandingGoalHandle::WrappedResult & result)
{
	// TODO: check the result field in result and print to terminal using either RCLCPP_WARN (result == 1), RCLCPP_ERROR (result == 2) or RCLCPP_FATAL (result == 3) a message indicating how severe the situation is (up to us for the moment)
	// I'm leaving this piece of code as is until cbuild will actually build this action client
  RCLCPP_INFO(this->get_logger(),"[LANDING] Got result");
	return;
}

// TODO: brainstorm logic and implement
std::string LandingClient::get_result_str()
{
  return result_ss_.str();
}

// TODO: brainstorm logic and implement
std::shared_future<LandingGoalHandleSharedPtr> LandingClient::send_goal(){ return(std::shared_future<LandingGoalHandleSharedPtr>()); }

// TODO: brainstorm logic and implement
std::shared_future<LandingGoalHandle::WrappedResult> LandingClient::request_result(LandingGoalHandleSharedPtr goal_handle)
{
  return client_->async_get_result(goal_handle);
}

// TODO: brainstorm logic and implement
std::shared_future<CancelResponseSharedPtr> LandingClient::request_cancel(LandingGoalHandleSharedPtr goal_handle){ return std::shared_future<CancelResponseSharedPtr>(); }
