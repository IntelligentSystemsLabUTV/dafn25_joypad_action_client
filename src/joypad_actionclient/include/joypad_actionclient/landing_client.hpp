#include <sstream>

//! Service calls will all handle futures
#include <future>

//! Necessary to define pointers to addres many action-related objects
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp> //! Necessary to use actions

#include <simple_actionclient_cpp/simple_actionclient.hpp>
#include <dua_aircraft_interfaces/action/landing.hpp>

//! Again, let's make our life easier
using Landing = dua_aircraft_interfaces::action::Landing;
using LandingGoalHandle = rclcpp_action::ClientGoalHandle<Landing>;
using LandingGoalHandleSharedPtr = LandingGoalHandle::SharedPtr;
using CancelResponse = action_msgs::srv::CancelGoal::Response;
using CancelResponseSharedPtr = CancelResponse::SharedPtr;

class LandingClient : public rclcpp::Node
{
public:
  LandingClient();
  std::string get_result_str();

  //! These are just for us, to wrap operations on the (private) back-end
	std::shared_future<LandingGoalHandleSharedPtr> send_goal(void);
  std::shared_future<LandingGoalHandle::WrappedResult> request_result(
    LandingGoalHandleSharedPtr goal_handle);
  std::shared_future<CancelResponseSharedPtr> request_cancel(
    LandingGoalHandleSharedPtr goal_handle);

private:
  std::stringstream result_ss_;

  //! Client goes with options needed to send goal requests
  rclcpp_action::Client<Landing>::SharedPtr client_;
  rclcpp_action::Client<Landing>::SendGoalOptions client_opts_;

  //! Name of the following callbacks says it all, but pay attention to their
  //! signatures and argument types!
  void goal_response_clbk(LandingGoalHandleSharedPtr goal_handle);
  void feedback_callback(
    LandingGoalHandleSharedPtr goal_handle,
    const std::shared_ptr<const Landing::Feedback> feedback);
  void result_callback(const LandingGoalHandle::WrappedResult & result);
  void cancel_callback(CancelResponseSharedPtr cancel_resp);
};
