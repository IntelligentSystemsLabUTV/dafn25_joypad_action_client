#include <sstream>

//! Service calls will all handle futures
#include <future>

//! Necessary to define pointers to addres many action-related objects
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp> //! Necessary to use actions

#include <simple_actionclient_cpp/simple_actionclient.hpp>
#include <dua_aircraft_interfaces/action/takeoff.hpp>

//! Again, let's make our life easier
using Takeoff = dua_aircraft_interfaces::action::Takeoff;
using TakeoffGoalHandle = rclcpp_action::ClientGoalHandle<Takeoff>;
using TakeoffGoalHandleSharedPtr = TakeoffGoalHandle::SharedPtr;
using CancelResponse = action_msgs::srv::CancelGoal::Response;
using CancelResponseSharedPtr = CancelResponse::SharedPtr;

class TakeoffClient : public rclcpp::Node
{
public:
  TakeoffClient();
  std::string get_result_str();

  //! These are just for us, to wrap operations on the (private) back-end
	std::shared_future<TakeoffGoalHandleSharedPtr> send_goal(void);
  std::shared_future<TakeoffGoalHandle::WrappedResult> request_result(
    TakeoffGoalHandleSharedPtr goal_handle);
  std::shared_future<CancelResponseSharedPtr> request_cancel(
    TakeoffGoalHandleSharedPtr goal_handle);

private:
  std::stringstream result_ss_;

  //! Client goes with options needed to send goal requests
  rclcpp_action::Client<Takeoff>::SharedPtr client_;
  rclcpp_action::Client<Takeoff>::SendGoalOptions client_opts_;

  //! Name of the following callbacks says it all, but pay attention to their
  //! signatures and argument types!
  void goal_response_clbk(TakeoffGoalHandleSharedPtr goal_handle);
  void feedback_callback(
    TakeoffGoalHandleSharedPtr goal_handle,
    const std::shared_ptr<const Takeoff::Feedback> feedback);
  void result_callback(const TakeoffGoalHandle::WrappedResult & result);
  void cancel_callback(CancelResponseSharedPtr cancel_resp);
};
