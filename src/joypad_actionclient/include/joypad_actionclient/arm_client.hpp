#include <sstream>

//! Service calls will all handle futures
#include <future>

//! Necessary to define pointers to addres many action-related objects
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp> //! Necessary to use actions

#include <simple_actionclient_cpp/simple_actionclient.hpp>
#include <dua_hardware_interfaces/action/arm.hpp>

//! Again, let's make our life easier
using Arm = dua_hardware_interfaces::action::Arm;
using ArmGoalHandle = rclcpp_action::ClientGoalHandle<Arm>;
using ArmGoalHandleSharedPtr = ArmGoalHandle::SharedPtr;
using CancelResponse = action_msgs::srv::CancelGoal::Response;
using CancelResponseSharedPtr = CancelResponse::SharedPtr;

class ArmClient : public rclcpp::Node
{
public:
  ArmClient();
  std::string get_result_str();

  //! These are just for us, to wrap operations on the (private) back-end
	std::shared_future<ArmGoalHandleSharedPtr> send_goal(void);
  std::shared_future<ArmGoalHandle::WrappedResult> request_result(
    ArmGoalHandleSharedPtr goal_handle);
  std::shared_future<CancelResponseSharedPtr> request_cancel(
    ArmGoalHandleSharedPtr goal_handle);

private:
  std::stringstream result_ss_;

  //! Client goes with options needed to send goal requests
  rclcpp_action::Client<Arm>::SharedPtr client_;
  rclcpp_action::Client<Arm>::SendGoalOptions client_opts_;

  //! Name of the following callbacks says it all, but pay attention to their
  //! signatures and argument types!
  void goal_response_clbk(ArmGoalHandleSharedPtr goal_handle);
  void feedback_callback(
    ArmGoalHandleSharedPtr goal_handle,
    const std::shared_ptr<const Arm::Feedback> feedback);
  void result_callback(const ArmGoalHandle::WrappedResult & result);
  void cancel_callback(CancelResponseSharedPtr cancel_resp);
};
