#include <sstream>

#include <future>

#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp> 
#include <simple_actionclient_cpp/simple_actionclient.hpp>
#include <dua_hardware_interfaces/action/disarm.hpp>

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

  std::shared_future<DisarmGoalHandleSharedPtr> send_goal(void);
  std::shared_future<DisarmGoalHandle::WrappedResult> request_result(
    DisarmGoalHandleSharedPtr goal_handle);
  std::shared_future<CancelResponseSharedPtr> request_cancel(
    DisarmGoalHandleSharedPtr goal_handle);

private:
  std::stringstream result_ss_;

  rclcpp_action::Client<Disarm>::SharedPtr client_;
  rclcpp_action::Client<Disarm>::SendGoalOptions client_opts_;

  void goal_response_clbk(DisarmGoalHandleSharedPtr goal_handle);
  void feedback_callback(
    DisarmGoalHandleSharedPtr goal_handle,
  const std::shared_ptr<const Disarm::Feedback> feedback);
  void result_callback(const DisarmGoalHandle::WrappedResult & result);
  void cancel_callback(CancelResponseSharedPtr cancel_resp);
};
