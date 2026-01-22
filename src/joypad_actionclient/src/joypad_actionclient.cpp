// AI
#include <chrono>
#include <sstream>
#include <stdexcept>

#include <actions_example_cpp/fib_client.hpp>

/*
// TODO: investigate whether any of this is useful
JoypadActionClient::JoypadActionClient(const rclcpp::NodeOptions & options)
: Node("joypad_action_client", options),
  arm_client_(this, params_.arm.name),
  disarm_client_(this, params_.disarm.name),
  takeoff_client_(this, params_.takeoff.name),
  landing_client_(this, params_.landing.name)
{
  declare_params(*this, params_);

  joy_sub_ = create_subscription<sensor_msgs::msg::Joy>(
    "/joy", 10,
    std::bind(&JoypadActionClient::joy_callback, this, std::placeholders::_1));
}

void JoypadActionClient::joy_callback(
  const sensor_msgs::msg::Joy::SharedPtr msg)
{
  auto check = [&](const ActionConfig & a)
  {
    return msg->buttons[a.button];
  };

  if (check(params_.arm))
    arm_client_.send_goal({});

  if (check(params_.disarm))
    disarm_client_.send_goal({});

  if (check(params_.takeoff))
  {
    dua_aircraft_interfaces::action::Takeoff::Goal goal;
    goal.takeoff_pose.header.frame_id = "map";
    goal.takeoff_pose.header.stamp = now();
    goal.takeoff_pose.pose.position.z = params_.takeoff_altitude;
    takeoff_client_.send_goal(goal);
  }

  if (check(params_.landing))
  {
    dua_aircraft_interfaces::action::Landing::Goal goal;
    goal.descend = false;
    goal.decision_altitude = params_.landing_decision_altitude;
    landing_client_.send_goal(goal);
  }
}
*/

#define UNUSED(arg) (void)(arg)

/**
 * @brief Creates a new JoypadActionClient node.
 */
JoypadActionClient::JoypadActionClient()
: Node("fibonacci_client")
{
  //! Create the action client like a service client
  client_ = rclcpp_action::create_client<JoypadAction>(
    this,
    "/fibonacci_computer/fibonacci");

  //! Initialize client options: bind all callbacks
  client_opts_.goal_response_callback = std::bind(
    &JoypadActionClient::goal_response_clbk,
    this,
    std::placeholders::_1);
  client_opts_.feedback_callback = std::bind(
    &JoypadActionClient::feedback_callback,
    this,
    std::placeholders::_1,
    std::placeholders::_2);
  client_opts_.result_callback = std::bind(
    &JoypadActionClient::result_callback,
    this,
    std::placeholders::_1);

  RCLCPP_INFO(this->get_logger(), "Node initialized");
}

/**
 * @brief Called when the goal is sent to the server over the goal service.
 *
 * @param goal_handle Goal handle pointer.
 */
void JoypadActionClient::goal_response_clbk(
  JoypadActionGoalHandleSharedPtr goal_handle)
{
  //! Check if the handle is valid, i.e. the goal was accepted or rejected
  if (goal_handle == nullptr) {
    RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
  } else {
    RCLCPP_INFO(
      this->get_logger(),
      "Goal was accepted by server (%s at %f)",
      rclcpp_action::to_string(goal_handle->get_goal_id()).c_str(),
      goal_handle->get_goal_stamp().seconds());
  }
}

/**
 * @brief Prints the feedback message.
 *
 * @param goal_handle Handle to the goal being processed.
 * @param feedback New feedback message.
 */
void JoypadActionClient::feedback_callback(
  JoypadActionGoalHandleSharedPtr goal_handle,
  const std::shared_ptr<const JoypadAction::Feedback> feedback)
{
  UNUSED(goal_handle);
  // Just print the partial sequence we just got
  std::stringstream ss("");
  ss << "Partial sequence: ";
  for (auto number : feedback->partial_sequence) {
    ss << number << " ";
  }
  RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());
}

/**
 * @brief Handles the goal cancellation response.
 *
 * @param cancel_resp Cancellation response message.
 */
void JoypadActionClient::cancel_callback(CancelResponseSharedPtr cancel_resp)
{
  switch (cancel_resp->return_code) {
    case CancelResponse::ERROR_NONE:
      RCLCPP_WARN(this->get_logger(), "Goal CANCELED");
      break;
    case CancelResponse::ERROR_REJECTED:
      RCLCPP_ERROR(this->get_logger(), "Goal cancellation request REJECTED");
      break;
    case CancelResponse::ERROR_UNKNOWN_GOAL_ID:
      RCLCPP_ERROR(this->get_logger(), "Unkown goal ID");
      break;
    case CancelResponse::ERROR_GOAL_TERMINATED:
      RCLCPP_WARN(this->get_logger(), "Goal TERMINATED");
      break;
    default:
      break;
  }
}

/**
 * @brief Called when the result server just sent the result.
 *
 * @param result Wrapped result object.
 */
void JoypadActionClient::result_callback(
  const JoypadActionGoalHandle::WrappedResult & result)
{
  //! This is called when the server has just sent the result back!
  //! The result object holds:
  //! - Code indicating whether the result is successful, canceled or aborted
  //! - Result as specified by the interface
  //! This routine is a good starting template for this kind of callback

  RCLCPP_INFO(
    this->get_logger(),
    "Got result of (%s)",
    rclcpp_action::to_string(result.goal_id).c_str());

  // Parse the result
  switch (result.code) {
    case rclcpp_action::ResultCode::SUCCEEDED:
      break;
    case rclcpp_action::ResultCode::ABORTED:
      RCLCPP_ERROR(this->get_logger(), "ABORTED");
      return;
    case rclcpp_action::ResultCode::CANCELED:
      RCLCPP_ERROR(this->get_logger(), "CANCELED");
      result_ss_ << "(Partial) ";
      break;
    case rclcpp_action::ResultCode::UNKNOWN:
      RCLCPP_ERROR(this->get_logger(), "UNKNOWN");
      return;
    default:
      RCLCPP_ERROR(this->get_logger(), "Unknown result code");
      return;
  }

  // Parse the (successful) result
  result_ss_ << "Result: ";
  for (auto number : result.result->sequence) {
    result_ss_ << number << " ";
  }
}

/**
 * @brief Getter for result char string pointer.
 *
 * @return Result char string pointer.
 */
std::string JoypadActionClient::get_result_str()
{
  return result_ss_.str();
}

/**
 * @brief Sends a new goal to the server.
 *
 * @param order Order up to which the JoypadAction sequence has to be computed.
 * @return Goal handle future.
 */
std::shared_future<JoypadActionGoalHandleSharedPtr> JoypadActionClient::send_goal(
  int order)
{
  //! Note that since this whole system relies entirely on callbacks, this
  //! just has to send the goal, everything else will happen automatically
  //! afterwards!
  //! I.e. actions induce event-based asynchronous programming, which you can
  //! make synchronous by waiting explicitly on futures and similar objects

  // Initialize result
  result_ss_.str("");

  //! Initialize goal request object
  JoypadAction::Goal goal_request{};
  goal_request.set__order(order);

  //! Wait for the server to become available
  while (!client_->wait_for_action_server(std::chrono::seconds(1))) {
    RCLCPP_WARN(this->get_logger(), "Server not available...");
    if (!rclcpp::ok()) {
      throw std::runtime_error("Middleware crashed");
    }
  }

  //! Send goal to goal server, setting callbacks for this request, and
  //! return the related future
  return client_->async_send_goal(goal_request, client_opts_);
}

/**
 * @brief Requests results for a given goal.
 *
 * @param goal_handle Goal handle to request result for.
 * @return Goal WrappedResult object future.
 */
std::shared_future<JoypadActionGoalHandle::WrappedResult> JoypadActionClient::request_result(
  JoypadActionGoalHandleSharedPtr goal_handle)
{
  return client_->async_get_result(goal_handle);
}

/**
 * @brief Requests cancellation of a given goal.
 *
 * @param goal_handle Goal handle to request cancellation of.
 * @return Cancellation response object future.
 */
std::shared_future<CancelResponseSharedPtr> JoypadActionClient::request_cancel(
  JoypadActionGoalHandleSharedPtr goal_handle)
{
  return client_->async_cancel_goal(
    goal_handle,
    std::bind(
      &JoypadActionClient::cancel_callback,
      this,
      std::placeholders::_1));
}
