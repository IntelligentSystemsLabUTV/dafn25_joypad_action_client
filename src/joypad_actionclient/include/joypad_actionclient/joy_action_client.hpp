#ifndef JOYPAD_ACTIONCLIENT__JOY_ACTION_CLIENT_HPP_
#define JOYPAD_ACTIONCLIENT__JOY_ACTION_CLIENT_HPP_

#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <thread>
#include <map> 

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

#include <dua_hardware_interfaces/action/arm.hpp>
#include <dua_hardware_interfaces/action/disarm.hpp>
#include <dua_aircraft_interfaces/action/takeoff.hpp>
#include <dua_aircraft_interfaces/action/landing.hpp>
#include <dua_common_interfaces/msg/command_result_stamped.hpp>

#include <params_manager_cpp/params_manager.hpp>
#include <simple_actionclient_cpp/simple_actionclient.hpp>

#define MIN_BUTTONS_NUM   0 
#define MAX_BUTTONS_NUM   11

// NOTE: se impostati a 0, simple_actionclient.hpp lo imposterà a 5000, cioè 5s
#define   SEND_GOAL_TIMEOUT  2000
#define RESULT_GOAL_TIMEOUT  5000
#define CANCEL_GOAL_TIMEOUT  2000

#define MAX_NUM_THREADS 8

namespace joypad_actionclient {

void clear_threads_vector(std::vector<std::thread>& threads);

// NOTE: ecco una descrizione del metodo try_emblace_back presa da
// cppreference
// Conditionally appends an object of type T to the end of the container.
// If size() == capacity() is true, there are no effects. Otherwise, appends direct-non-list-initialized with std::forward<Args>(args)... object of type T.
//No iterators or references are invalidated, except end(), which is invalidated if the insertion occurs.
// TL;DR: è una append che non resize-a il vettore (i vettori inplace a differenza di quelli normali hanno una capacità max)
// Il tipo vector non supporta emplace e gli inplace_vector non sono standard C++, quindi implementiamo una funzione che reimpiazzi try_emplace_back
template <typename F, typename... Args>
  void my_emplace(std::vector<std::thread>& threads, F&& f, Args&&... args);

class JoyActionClient : public rclcpp::Node
{
public:
  using Arm = dua_hardware_interfaces::action::Arm;
  using Disarm = dua_hardware_interfaces::action::Disarm;
  using Takeoff = dua_aircraft_interfaces::action::Takeoff;
  using Landing = dua_aircraft_interfaces::action::Landing;
  using CommandResultStamped = dua_common_interfaces::msg::CommandResultStamped;

  explicit JoyActionClient(const rclcpp::NodeOptions & options);
  // NOTE: ~JoyActionClient è il distruttore della classe JoyActionClient
  // L'implementazione fa solo un join dei thread che crea per gestire
  // le azioni (un thread per action-client) 
  ~JoyActionClient();
  
  // NOTE: questo è il costruttore overwrote nel caso non si passino parametri (realisticamente inutile)
  JoyActionClient();
  
  // NOTE: init_parameters è necessario da specifica di params_manager_cpp
  void init_parameters();

private:
  std::shared_ptr<params_manager::Manager> params_manager_;

  int64_t btn_idx_help_mapping_idx_;
  int64_t btn_idx_help_legend_idx_;

  std::string arm_action_name_;
  int64_t arm_button_idx_;

  std::string disarm_action_name_;
  int64_t disarm_button_idx_;

  std::string takeoff_action_name_;
  int64_t takeoff_button_idx_;
  double takeoff_altitude_;

  std::string landing_action_name_;
  int64_t landing_button_idx_;

  std::shared_ptr<simple_actionclient::Client<Arm>> client_arm_;
  std::shared_ptr<simple_actionclient::Client<Disarm>> client_disarm_;
  std::shared_ptr<simple_actionclient::Client<Takeoff>> client_takeoff_;
  std::shared_ptr<simple_actionclient::Client<Landing>> client_landing_;

  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
  
  std::vector<int> last_buttons_;
  
  std::vector<std::thread> active_threads_;

  // NOTE: questo andrebbe caricato da parametri ma la specifica
  // del progetto è stata modificata e il ruolo di questo parametro è da definire meglio
  const double LANDING_DECISION_ALTITUDE = 5.0;

  void validate_parameters();
  
  std::string get_button_label(int64_t idx) const;

  void print_control_scheme() const;
  void print_all_buttons_legend() const;

  void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg);
  bool check_button_press(const sensor_msgs::msg::Joy::SharedPtr & msg, int button_idx);

  void execute_arm();
  void execute_disarm();
  void execute_takeoff();
  void execute_landing();

  template <typename ActionT>
  void process_result(
    const std::tuple<bool, rclcpp_action::ResultCode, typename std::shared_ptr<typename ActionT::Result>> & ret,
    const std::string & action_name)
  {
    auto result_code = std::get<rclcpp_action::ResultCode>(ret);
    
    if (result_code != rclcpp_action::ResultCode::SUCCEEDED) {
      if (result_code == rclcpp_action::ResultCode::ABORTED) {
        RCLCPP_ERROR(this->get_logger(), "Action %s Transport: ABORTED", action_name.c_str());
      } else if (result_code == rclcpp_action::ResultCode::CANCELED) {
        RCLCPP_ERROR(this->get_logger(), "Action %s Transport: CANCELED", action_name.c_str());
      } else {
        RCLCPP_FATAL(this->get_logger(), "Action %s Transport: UNKNOWN ERROR", action_name.c_str());
      }
      return;
    }

    // NOTE: Facciamo std::get<2> per prendere l'ultimo elemento del vettore ret
    // Dagli header di simple_actionclient_cpp abbiamo
    // return std::make_tuple(true, goal_result->code, goal_result->result);
    // quindi std::get<2>(ret) è goal_result->result
    auto result_ptr = std::get<2>(ret);
    if (!result_ptr) {
      RCLCPP_FATAL(this->get_logger(), "Action %s: Null result pointer", action_name.c_str());
      return;
    }

    const auto & cmd_result = result_ptr->result; 

    if (cmd_result.result == CommandResultStamped::SUCCESS) {
        RCLCPP_WARN(this->get_logger(), "Action %s: SUCCEEDED", action_name.c_str());
    } 
    else if (cmd_result.result == CommandResultStamped::FAILED) {
        RCLCPP_ERROR(this->get_logger(), "Action %s: FAILED - %s", action_name.c_str(), cmd_result.error_msg.c_str());
    }
    else {
        RCLCPP_FATAL(this->get_logger(), "Action %s: OPERATIONAL ERROR (Code: %d) - %s", 
                     action_name.c_str(), cmd_result.result, cmd_result.error_msg.c_str());
    }
  }
};

} // namespace joypad_actionclient

#endif
