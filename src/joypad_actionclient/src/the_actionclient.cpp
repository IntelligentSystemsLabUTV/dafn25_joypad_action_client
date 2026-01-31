#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iostream>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <sensor_msgs/msg/joy.hpp>

#include <dua_hardware_interfaces/action/arm.hpp>
#include <dua_hardware_interfaces/action/disarm.hpp>
#include <dua_aircraft_interfaces/action/takeoff.hpp>
#include <dua_aircraft_interfaces/action/landing.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

class JoyActionClient : public rclcpp::Node
{
public:
  using Arm = dua_hardware_interfaces::action::Arm;
  using Disarm = dua_hardware_interfaces::action::Disarm;
  using Takeoff = dua_aircraft_interfaces::action::Takeoff;
  using Landing = dua_aircraft_interfaces::action::Landing;

  JoyActionClient()
   : Node("joy_action_client") 
  {
    // TODO: riscrivere usando params_manager_cpp
    this->declare_parameter("arm.name", "arm");
    this->declare_parameter("arm.button", -1); 
    
    this->declare_parameter("disarm.name", "disarm");
    this->declare_parameter("disarm.button", -1);
    
    this->declare_parameter("takeoff.name", "takeoff");
    this->declare_parameter("takeoff.button", -1);
    this->declare_parameter("takeoff.altitude", 2.5); 
    
    this->declare_parameter("landing.name", "landing");
    this->declare_parameter("landing.button", -1);

    arm_action_name_ = this->get_parameter("arm.name").as_string();
    arm_button_idx_ = this->get_parameter("arm.button").as_int();

    disarm_action_name_ = this->get_parameter("disarm.name").as_string();
    disarm_button_idx_ = this->get_parameter("disarm.button").as_int();

    takeoff_action_name_ = this->get_parameter("takeoff.name").as_string();
    takeoff_button_idx_ = this->get_parameter("takeoff.button").as_int();
    takeoff_altitude_ = this->get_parameter("takeoff.altitude").as_double();

    landing_action_name_ = this->get_parameter("landing.name").as_string();
    landing_button_idx_ = this->get_parameter("landing.button").as_int();

    // TODO: fin qui

    param_callback_handle_ = this->add_on_set_parameters_callback(
    	std::bind(&JoyActionClient::parameters_callback, this, _1));
 
    // TODO: valutare se vada modificato usando simple_actionclient_cpp
    client_arm_ = rclcpp_action::create_client<Arm>(this, arm_action_name_);
    client_disarm_ = rclcpp_action::create_client<Disarm>(this, disarm_action_name_);
    client_takeoff_ = rclcpp_action::create_client<Takeoff>(this, takeoff_action_name_);
    client_landing_ = rclcpp_action::create_client<Landing>(this, landing_action_name_);
    // TODO: fin qui

    joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
      "/joy",
       10,    // QoS Depth (numero degli ultimi messaggi che il nodo tiene in memoria) 
      std::bind(&JoyActionClient::joy_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "JoyActionClient initialized. Waiting for Joy commands...");
  }

private:
  rclcpp_action::Client<Arm>::SharedPtr client_arm_;
  rclcpp_action::Client<Disarm>::SharedPtr client_disarm_;
  rclcpp_action::Client<Takeoff>::SharedPtr client_takeoff_;
  rclcpp_action::Client<Landing>::SharedPtr client_landing_;

  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
  OnSetParametersCallbackHandle::SharedPtr param_callback_handle_;

  std::string arm_action_name_, disarm_action_name_, takeoff_action_name_, landing_action_name_;
  int arm_button_idx_, disarm_button_idx_, takeoff_button_idx_, landing_button_idx_;
  double takeoff_altitude_;

  std::vector<int> last_buttons_;

  // TODO: se necessario, riscrivere, altrimenti eliminare (fa pena :))
  rcl_interfaces::msg::SetParametersResult parameters_callback(
    const std::vector<rclcpp::Parameter> & parameters)
  {
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    result.reason = "success";

    for (const auto & param : parameters) {
      if (param.get_name() == "arm.button") {
        if (param.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER) {
          arm_button_idx_ = param.as_int();
          RCLCPP_INFO(this->get_logger(), "Reconfigured Arm Button: %d", arm_button_idx_);
        } else {
          result.successful = false;
          result.reason = "arm.button must be an integer";
        }
      }
      else if (param.get_name() == "disarm.button") {
        if (param.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER) {
          disarm_button_idx_ = param.as_int();
          RCLCPP_INFO(this->get_logger(), "Reconfigured Disarm Button: %d", disarm_button_idx_);
        }
      }
      else if (param.get_name() == "takeoff.button") {
        if (param.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER) {
          takeoff_button_idx_ = param.as_int();
          RCLCPP_INFO(this->get_logger(), "Reconfigured Takeoff Button: %d", takeoff_button_idx_);
        }
      }
      else if (param.get_name() == "landing.button") {
        if (param.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER) {
          landing_button_idx_ = param.as_int();
          RCLCPP_INFO(this->get_logger(), "Reconfigured Landing Button: %d", landing_button_idx_);
        }
      }
      else if (param.get_name() == "takeoff.altitude") {
         if (param.get_type() == rclcpp::ParameterType::PARAMETER_DOUBLE) {
            takeoff_altitude_ = param.as_double();
            RCLCPP_INFO(this->get_logger(), "Reconfigured Takeoff Altitude: %.2f", takeoff_altitude_);
         }
      }
    }
    return result;
  }
  // TODO: fin qui 

  void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg)
  {
    if (last_buttons_.empty()) {
      last_buttons_.resize(msg->buttons.size(), 0);
    }

    /* LOGICA DI CONTROLLO
     * Si controlla se è stato premuto il tasto associato all'azione.
     * La verifica usa un "fronte di salita" per inviare il goal una sola volta per pressione.
    */

    // TODO: valutare priorità dei messaggi di priorità, se l'else if su takeoff può essere eliminato o altri meccanismi di scelta su combinazioni di tasti premuti
    if (check_button_press(msg, arm_button_idx_)) {
      send_arm_goal();
    }
    else if (check_button_press(msg, disarm_button_idx_)) {
      send_disarm_goal();
    }
    else if (check_button_press(msg, takeoff_button_idx_)) {
      send_takeoff_goal();
    }
    else if (check_button_press(msg, landing_button_idx_)) {
      send_landing_goal();
    }

    last_buttons_ = msg->buttons;
  }

  bool check_button_press(const sensor_msgs::msg::Joy::SharedPtr & msg, int button_idx)
  {
    if (button_idx < 0 || static_cast<size_t>(button_idx) >= msg->buttons.size()) {
      return false; 
    }

    return (msg->buttons[button_idx] == 1 && last_buttons_[button_idx] == 0);
  }

  void send_arm_goal()
  {
    if (!client_arm_->wait_for_action_server(std::chrono::seconds(1))) {
      RCLCPP_ERROR(this->get_logger(), "Action server %s not available", arm_action_name_.c_str());
      return;
    }

    auto goal_msg = Arm::Goal();
    
    auto send_goal_options = rclcpp_action::Client<Arm>::SendGoalOptions();
    send_goal_options.result_callback = std::bind(&JoyActionClient::result_callback_generic<Arm>, this, _1, "Arm");
    
    RCLCPP_INFO(this->get_logger(), "Sending ARM goal");
    client_arm_->async_send_goal(goal_msg, send_goal_options);
  }

  void send_disarm_goal()
  {
    if (!client_disarm_->wait_for_action_server(std::chrono::seconds(1))) {
      RCLCPP_ERROR(this->get_logger(), "Action server %s not available", disarm_action_name_.c_str());
      return;
    }
    auto goal_msg = Disarm::Goal();
    
    auto send_goal_options = rclcpp_action::Client<Disarm>::SendGoalOptions();
    send_goal_options.result_callback = std::bind(&JoyActionClient::result_callback_generic<Disarm>, this, _1, "Disarm");
    
    RCLCPP_INFO(this->get_logger(), "Sending DISARM goal");
    client_disarm_->async_send_goal(goal_msg, send_goal_options);
  }

  void send_takeoff_goal()
  {
    if (!client_takeoff_->wait_for_action_server(std::chrono::seconds(1))) {
      RCLCPP_ERROR(this->get_logger(), "Action server %s not available", takeoff_action_name_.c_str());
      return;
    }
    
    auto goal_msg = Takeoff::Goal();
    goal_msg.takeoff_pose.header.frame_id = "map"; 
    goal_msg.takeoff_pose.header.stamp = this->get_clock()->now(); 
    goal_msg.takeoff_pose.pose.position.z = takeoff_altitude_; 

    auto send_goal_options = rclcpp_action::Client<Takeoff>::SendGoalOptions();
    send_goal_options.result_callback = std::bind(&JoyActionClient::result_callback_generic<Takeoff>, this, _1, "Takeoff");
    
    RCLCPP_INFO(this->get_logger(), "Sending TAKEOFF goal (Alt: %.2f)", takeoff_altitude_);
    client_takeoff_->async_send_goal(goal_msg, send_goal_options);
  }

  void send_landing_goal()
  {
    if (!client_landing_->wait_for_action_server(std::chrono::seconds(1))) {
      RCLCPP_ERROR(this->get_logger(), "Action server %s not available", landing_action_name_.c_str());
      return;
    }
    
    auto goal_msg = Landing::Goal();
    goal_msg.descend = false; 

    auto send_goal_options = rclcpp_action::Client<Landing>::SendGoalOptions();
    send_goal_options.result_callback = std::bind(&JoyActionClient::result_callback_generic<Landing>, this, _1, "Landing");
    
    RCLCPP_INFO(this->get_logger(), "Sending LANDING goal");
    client_landing_->async_send_goal(goal_msg, send_goal_options);
  }

  template <typename ActionT>
  void result_callback_generic(const typename rclcpp_action::ClientGoalHandle<ActionT>::WrappedResult & result, const std::string & action_name)
  {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_WARN(this->get_logger(), "Action %s SUCCEEDED", action_name.c_str());
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(this->get_logger(), "Action %s FAILED (Aborted)", action_name.c_str());
        break;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_ERROR(this->get_logger(), "Action %s FAILED (Canceled)", action_name.c_str());
        break;
      default:
        RCLCPP_FATAL(this->get_logger(), "Action %s ERROR (Unknown code)", action_name.c_str());
        break;
    }
  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  
  auto node = std::make_shared<JoyActionClient>();
  rclcpp::spin(node);
  
  rclcpp::shutdown();
  return 0;
}
