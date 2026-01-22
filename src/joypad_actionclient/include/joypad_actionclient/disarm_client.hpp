#include <sstream>

//! Service calls will all handle futures
#include <future>

//! Necessary to define pointers to addres many action-related objects
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp> //! Necessary to use actions

#include <simple_actionclient_cpp/simple_actionclient.hpp>
#include <dua_hardware_interfaces/action/disarm.hpp>

//! Again, let's make our life easier
using Disarm = dua_hardware_interfaces::action::Disarm;
using DisarmGoalHandle = rclcpp_action::ClientGoalHandle<Disarm>;
using DisarmGoalHandleSharedPtr = DisarmGoalHandle::SharedPtr;
using CancelResponse = action_msgs::srv::CancelGoal::Response;
using CancelResponseSharedPtr = CancelResponse::SharedPtr;

class DisarmClient : public rclcpp::Node
{
public:
  DisarmClient();
  std::string get_result_str();

  //! These are just for us, to wrap operations on the (private) back-end
	std::shared_future<DisarmGoalHandleSharedPtr> send_goal(void);
  std::shared_future<DisarmGoalHandle::WrappedResult> request_result(
    DisarmGoalHandleSharedPtr goal_handle);
  std::shared_future<CancelResponseSharedPtr> request_cancel(
    DisarmGoalHandleSharedPtr goal_handle);

private:
  std::stringstream result_ss_;

  //! Client goes with options needed to send goal requests
  rclcpp_action::Client<Disarm>::SharedPtr client_;
  rclcpp_action::Client<Disarm>::SendGoalOptions client_opts_;

  //! Name of the following callbacks says it all, but pay attention to their
  //! signatures and argument types!
  void goal_response_clbk(DisarmGoalHandleSharedPtr goal_handle);
  void feedback_callback(
    DisarmGoalHandleSharedPtr goal_handle,
    const std::shared_ptr<const Disarm::Feedback> feedback);
  void result_callback(const DisarmGoalHandle::WrappedResult & result);
  void cancel_callback(CancelResponseSharedPtr cancel_resp);
};
