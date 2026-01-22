#include <chrono>
#include <sstream>
#include <stdexcept>

#include <joypad_actionclient/disarm_client.hpp>

DisarmClient::DisarmClient()
: Node("disarm_client")
{
  client_ = rclcpp_action::create_client<Disarm>(
    this,
    "disarm_client");

  client_opts_.goal_response_callback = std::bind(
    &DisarmClient::goal_response_clbk,
    this,
    std::placeholders::_1);

  client_opts_.feedback_callback = std::bind(
    &DisarmClient::feedback_callback,
    this,
    std::placeholders::_1,
    std::placeholders::_2);

  client_opts_.result_callback = std::bind(
    &DisarmClient::result_callback,
    this,
    std::placeholders::_1);

  RCLCPP_INFO(this->get_logger(), "[DISARM] Node initialized");
}

/***
	Reminder of how Disarm actions are handled
	```bash
		❯ ros2 interface show dua_hardware_interfaces/action/Disarm
		# Disarming operation.
		#
		# Roberto Masocco <r.masocco@dotxautomation.com>
		#
		# June 6, 2023

		# GOAL
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
	```
***/

// Don't care
void DisarmClient::goal_response_clbk(DisarmGoalHandleSharedPtr goal_handle){ return; }

// Don't care
void DisarmClient::feedback_callback(DisarmGoalHandleSharedPtr goal_handle,const std::shared_ptr<const Disarm::Feedback> feedback){ return; }

// Don't care
void DisarmClient::cancel_callback(CancelResponseSharedPtr cancel_resp){ return; }

// Now we're talking. This handles results sent by the server
void DisarmClient::result_callback(const DisarmGoalHandle::WrappedResult & result)
{
	// TODO: check the result field in result and print to terminal using either RCLCPP_WARN (result == 1), RCLCPP_ERROR (result == 2) or RCLCPP_FATAL (result == 3) a message indicating how severe the situation is (up to us for the moment)
	// I'm leaving this piece of code as is until cbuild will actually build this action client
  RCLCPP_INFO(this->get_logger(),"[DISARM] Got result");
	return;
}

// This is utility (gotta keep in as long as we keep it in the corresponding .hpp file in include)
std::string DisarmClient::get_result_str()
{
  return result_ss_.str();
}

// Don't care
std::shared_future<DisarmGoalHandleSharedPtr> DisarmClient::send_goal(){ return(std::shared_future<DisarmGoalHandleSharedPtr>()); }

// We also care about this one since we're requesting the server for the result
std::shared_future<DisarmGoalHandle::WrappedResult> DisarmClient::request_result(DisarmGoalHandleSharedPtr goal_handle)
{
  return client_->async_get_result(goal_handle);
}

// Don't care
std::shared_future<CancelResponseSharedPtr> DisarmClient::request_cancel(DisarmGoalHandleSharedPtr goal_handle){ return std::shared_future<CancelResponseSharedPtr>(); }
