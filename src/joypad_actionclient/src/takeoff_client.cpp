#include <chrono>
#include <sstream>
#include <stdexcept>

#include <joypad_actionclient/takeoff_client.hpp>

TakeoffClient::TakeoffClient()
: Node("takeoff_client")
{
  client_ = rclcpp_action::create_client<Takeoff>(
    this,
    "takeoff_client");

  client_opts_.goal_response_callback = std::bind(
    &TakeoffClient::goal_response_clbk,
    this,
    std::placeholders::_1);

  client_opts_.feedback_callback = std::bind(
    &TakeoffClient::feedback_callback,
    this,
    std::placeholders::_1,
    std::placeholders::_2);

  client_opts_.result_callback = std::bind(
    &TakeoffClient::result_callback,
    this,
    std::placeholders::_1);

  RCLCPP_INFO(this->get_logger(), "[TAKEOFF] Node initialized");
}

/***
	Reminder of how Takeoff actions are handled
	```bash
		❯ ros2 interface show dua_hardware_interfaces/action/Takeoff
		# Takeoff operation.
		#
		# Roberto Masocco <r.masocco@dotxautomation.com>
		#
		# June 6, 2023

		# GOAL
		# Target position and orientation to reach upon takeoff
		geometry_msgs/PoseStamped takeoff_pose
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
		geometry_msgs/PoseStamped current_pose
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
void TakeoffClient::goal_response_clbk(TakeoffGoalHandleSharedPtr goal_handle){ return; }

// TODO: brainstorm logic and implement
void TakeoffClient::feedback_callback(TakeoffGoalHandleSharedPtr goal_handle,const std::shared_ptr<const Takeoff::Feedback> feedback){ return; }

// TODO: brainstorm logic and implement
void TakeoffClient::cancel_callback(CancelResponseSharedPtr cancel_resp){ return; }

// TODO: brainstorm logic and implement
void TakeoffClient::result_callback(const TakeoffGoalHandle::WrappedResult & result)
{
	// TODO: check the result field in result and print to terminal using either RCLCPP_WARN (result == 1), RCLCPP_ERROR (result == 2) or RCLCPP_FATAL (result == 3) a message indicating how severe the situation is (up to us for the moment)
	// I'm leaving this piece of code as is until cbuild will actually build this action client
  RCLCPP_INFO(this->get_logger(),"[TAKEOFF] Got result");
	return;
}

// TODO: brainstorm logic and implement
std::string TakeoffClient::get_result_str()
{
  return result_ss_.str();
}

// TODO: brainstorm logic and implement
std::shared_future<TakeoffGoalHandleSharedPtr> TakeoffClient::send_goal(){ return(std::shared_future<TakeoffGoalHandleSharedPtr>()); }

// TODO: brainstorm logic and implement
std::shared_future<TakeoffGoalHandle::WrappedResult> TakeoffClient::request_result(TakeoffGoalHandleSharedPtr goal_handle)
{
  return client_->async_get_result(goal_handle);
}

// TODO: brainstorm logic and implement
std::shared_future<CancelResponseSharedPtr> TakeoffClient::request_cancel(TakeoffGoalHandleSharedPtr goal_handle){ return std::shared_future<CancelResponseSharedPtr>(); }
