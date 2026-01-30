#include <sstream>

#include <future>

#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp> 
#include <simple_actionclient_cpp/simple_actionclient.hpp>
#include <dua_aircraft_interfaces/action/takeoff.hpp>

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

  std::shared_future<TakeoffGoalHandleSharedPtr> send_goal(void);
  std::shared_future<TakeoffGoalHandle::WrappedResult> request_result(
    TakeoffGoalHandleSharedPtr goal_handle);
  std::shared_future<CancelResponseSharedPtr> request_cancel(
    TakeoffGoalHandleSharedPtr goal_handle);

private:
  std::stringstream result_ss_;

  rclcpp_action::Client<Takeoff>::SharedPtr client_;
  rclcpp_action::Client<Takeoff>::SendGoalOptions client_opts_;

  void goal_response_clbk(TakeoffGoalHandleSharedPtr goal_handle);
  void feedback_callback(
    TakeoffGoalHandleSharedPtr goal_handle,
  const std::shared_ptr<const Takeoff::Feedback> feedback);
  void result_callback(const TakeoffGoalHandle::WrappedResult & result);
  void cancel_callback(CancelResponseSharedPtr cancel_resp);
};
