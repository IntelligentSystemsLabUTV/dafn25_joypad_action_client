#include "joypad_actionclient/joy_action_client.hpp"

namespace joypad_actionclient {

void clear_threads_vector(std::vector<std::thread>& threads){
    if (threads.size() >= MAX_NUM_THREADS){
      for (auto & t : threads)
        if (t.joinable())
          t.join();
      threads.clear();
    }
}

// NOTE: ecco una descrizione del metodo try_emblace_back presa da
// cppreference
// Conditionally appends an object of type T to the end of the container.
// If size() == capacity() is true, there are no effects. Otherwise, appends direct-non-list-initialized with std::forward<Args>(args)... object of type T.
//No iterators or references are invalidated, except end(), which is invalidated if the insertion occurs.
// TL;DR: è una append che non resize-a il vettore (i vettori inplace a differenza di quelli normali hanno una capacità max)
// Il tipo vector non supporta emplace e gli inplace_vector non sono standard C++, quindi implementiamo una funzione che reimpiazzi try_emplace_back
template <typename F, typename... Args>
  void my_emplace(std::vector<std::thread>& threads, F&& f, Args&&... args)
{
    clear_threads_vector(threads);
    threads.emplace_back(
        std::forward<F>(f),
        std::forward<Args>(args)...
    );
}

JoyActionClient::JoyActionClient(const rclcpp::NodeOptions & options)
	: Node("joy_action_client", options),
  params_manager_(std::make_shared<params_manager::Manager>(this, true))
{
  init_parameters();
  validate_parameters();

  client_arm_ 		= std::make_shared<simple_actionclient::Client<Arm>>(this, arm_action_name_);
  client_disarm_ 	= std::make_shared<simple_actionclient::Client<Disarm>>(this, disarm_action_name_);
  client_takeoff_ = std::make_shared<simple_actionclient::Client<Takeoff>>(this, takeoff_action_name_);
  client_landing_ = std::make_shared<simple_actionclient::Client<Landing>>(this, landing_action_name_);

  joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
    "/joy",
    10, // NOTE: QoS per il subscriber joy (10 messagi in coda max)
    std::bind(&JoyActionClient::joy_callback, this, std::placeholders::_1));

  print_control_scheme();

  RCLCPP_INFO(this->get_logger(), "JoyActionClient Component initialized.");
}

// Opzionale, definisce il costruttore nel caso in cui non gli passi parametri
JoyActionClient::JoyActionClient() 
: JoyActionClient(rclcpp::NodeOptions()) 
{}

JoyActionClient::~JoyActionClient()
{
  clear_threads_vector(active_threads_);
}

void JoyActionClient::validate_parameters()
{
  bool valid = true;
  auto check_btn = [&](int64_t val, const std::string& name) {
		// NOTE: specifichiamo gli indici minimo e massimo per la mappatura tasti
		// Una scelta più selettiva con, e.g. una blacklist avrebbe più senso
		// Questo controllo è un sanity check che il numero del bottone non sia
		// out-of-bounds
    if (val < MIN_BUTTONS_NUM || val > MAX_BUTTONS_NUM) {
      RCLCPP_FATAL(this->get_logger(), 
        "Parameter '%s' invalid: %ld. Must be between %d and %d", name.c_str(), val, MIN_BUTTONS_NUM, MAX_BUTTONS_NUM);
      valid = false;
    }
  };

  // NOTE: queste sono chiamate da una funzione definita nel qui sopra 
  check_btn(arm_button_idx_,     		"actions.arm.button");
  check_btn(disarm_button_idx_,  		"actions.disarm.button");
  check_btn(takeoff_button_idx_, 		"actions.takeoff.button");
  check_btn(landing_button_idx_, 		"actions.landing.button");
  check_btn(btn_idx_help_mapping_idx_, 	"btn_idx_help_mapping.button");
  check_btn(btn_idx_help_legend_idx_ , 	"btn_idx_help_legend.button" );

  if (takeoff_altitude_ < 0.0) {
    RCLCPP_FATAL(this->get_logger(), 
      "Parameter 'actions.takeoff.altitude' invalid: %.2f. Must be positive.", takeoff_altitude_);
    valid = false;
  }

  if (!valid) {
    throw std::runtime_error("Invalid parameter configuration. Node startup aborted.");
  }
}

std::string JoyActionClient::get_button_label(int64_t idx) const
{
  // NOTE: questa mappatura sembrerebbe standard Linux
  switch(idx) {
    case 0: return "A / Cross";
    case 1: return "B / Circle";
    case 2: return "X / Square";
    case 3: return "Y / Triangle";
    case 4: return "LB / L1";
    case 5: return "RB / R1";
    case 6: return "Back / Share";
    case 7: return "Start / Options";
    case 8: return "Power / PS";
    case 9: return "L3 (Stick Click)";
    case 10: return "R3 (Stick Click)";

    default: return "Button " + std::to_string(idx);
  }
}

void JoyActionClient::print_control_scheme() const
{
  std::stringstream ss;
  ss << "\n==========================================\n";
  ss << "      CURRENT CONFIGURATION MAPPING       \n";
  ss << "==========================================\n";
  ss << " ACTION      | BUTTON ID | LABEL (Xbox/PS)\n";
  ss << "-------------|-----------|----------------\n";
  
  auto add_row = [&](const std::string& action, int64_t id) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), " %-11s | %2ld        | %s\n", 
             action.c_str(), id, get_button_label(id).c_str());
    ss << buffer;
  };

  add_row("ARM", arm_button_idx_);
  add_row("DISARM", disarm_button_idx_);
  add_row("TAKEOFF", takeoff_button_idx_);
  add_row("LANDING", landing_button_idx_);
  
  ss << "-------------|-----------|----------------\n";
  add_row("SHOW MAP", 		btn_idx_help_mapping_idx_);
  add_row("SHOW LEGEND", 	btn_idx_help_legend_idx_ );
  
  ss << "==========================================\n";

  RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());
}

void JoyActionClient::print_all_buttons_legend() const
{
  std::stringstream ss;
  ss << "\n------------------------------------------\n";
  ss << "      FULL BUTTON REFERENCE LEGEND        \n";
  ss << "------------------------------------------\n";
  ss << " ID  | LABEL (Xbox/PS)                    \n";
  ss << "-----|------------------------------------\n";
  
  for (int64_t i = 0; i <= 10; ++i) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), " %2ld  | %s\n", i, get_button_label(i).c_str());
    ss << buffer;
  }
  ss << "------------------------------------------\n";
  
  RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());
}

void JoyActionClient::joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg)
{
  // NOTE: last_buttons_ è carino. Se tenesse conto non solo dell'ultimo time-step potrebbe
  // implementare politiche in cui l'azione associata alla pressione prolungata di un bottone
  // sia diversa da quella di una pressione singola
  if (last_buttons_.empty()) {
    last_buttons_.resize(msg->buttons.size(), 0);
    last_buttons_ = msg->buttons;
    return;
  }
  
  if (check_button_press(msg, static_cast<int>(arm_button_idx_))) {
    my_emplace(active_threads_, &JoyActionClient::execute_arm, this);
  }
  if (check_button_press(msg, static_cast<int>(disarm_button_idx_))) {
    my_emplace(active_threads_, &JoyActionClient::execute_disarm, this);
  }
  if (check_button_press(msg, static_cast<int>(takeoff_button_idx_))) {
    my_emplace(active_threads_, &JoyActionClient::execute_takeoff, this);
  }
  if (check_button_press(msg, static_cast<int>(landing_button_idx_))) {
    my_emplace(active_threads_, &JoyActionClient::execute_landing, this);
  }
  // NOTE: questa aggiunta è un'idea GENIALE di Francesco: con Start stampiamo
  // a schermo la mappatura dei tasti, mentre con Select solo quelli gestiti
  // degli actionclient 
  if (check_button_press(msg, static_cast<int>(btn_idx_help_mapping_idx_))) {
    my_emplace(active_threads_, &JoyActionClient::print_control_scheme, this);
  }
  if (check_button_press(msg, static_cast<int>(btn_idx_help_legend_idx_))) {
    my_emplace(active_threads_, &JoyActionClient::print_all_buttons_legend, this);
  }

  last_buttons_ = msg->buttons;
}

// NOTE: politica di detection del rising-edge alla pressione di un bottone
bool JoyActionClient::check_button_press(const sensor_msgs::msg::Joy::SharedPtr & msg, int button_idx)
{
  if (button_idx < 0 || static_cast<size_t>(button_idx) >= msg->buttons.size()) {
    return false;
  }
  return (msg->buttons[button_idx] == 1 && last_buttons_[button_idx] == 0);
}

void JoyActionClient::execute_arm()
{
  if (!client_arm_) return;
  
  Arm::Goal goal;
  RCLCPP_INFO(this->get_logger(), "Requesting ARM...");
  
  // NOTE: false potrebbe essere spin o cancel_on_timeout. Per me è spin (L)
  // NOTE: i timeout possono essere omessi, in tal caso simple_actionclient_cpp li imposterà di default a 5s
  auto ret = client_arm_->call_sync(goal, false, SEND_GOAL_TIMEOUT, RESULT_GOAL_TIMEOUT, CANCEL_GOAL_TIMEOUT);

  // NOTE: process_result è definito in ../include/joypad_actionclient/joy_action_client.hpp
  process_result<Arm>(ret, "Arm");
}

void JoyActionClient::execute_disarm()
{
  if (!client_disarm_) return;

  Disarm::Goal goal;
  RCLCPP_INFO(this->get_logger(), "Requesting DISARM...");
  
  auto ret = client_disarm_->call_sync(goal, false, SEND_GOAL_TIMEOUT, RESULT_GOAL_TIMEOUT, CANCEL_GOAL_TIMEOUT);
  process_result<Disarm>(ret, "Disarm");
}

void JoyActionClient::execute_takeoff()
{
  if (!client_takeoff_) return;

  Takeoff::Goal goal;
  goal.takeoff_pose.header.frame_id = "map";
  goal.takeoff_pose.header.stamp = this->get_clock()->now();
  goal.takeoff_pose.pose.position.z = takeoff_altitude_;

  RCLCPP_INFO(this->get_logger(), "Requesting TAKEOFF (Alt: %.2f)...", takeoff_altitude_);
  
  auto ret = client_takeoff_->call_sync(goal, false, SEND_GOAL_TIMEOUT, RESULT_GOAL_TIMEOUT, CANCEL_GOAL_TIMEOUT);
  process_result<Takeoff>(ret, "Takeoff");
}

void JoyActionClient::execute_landing()
{
  if (!client_landing_) return;

  Landing::Goal goal;
  goal.descend = false;

  RCLCPP_INFO(this->get_logger(), "Requesting LANDING...");
  
  auto ret = client_landing_->call_sync(goal, false, SEND_GOAL_TIMEOUT, RESULT_GOAL_TIMEOUT, CANCEL_GOAL_TIMEOUT);
  process_result<Landing>(ret, "Landing");
}

} // namespace joypad_actionclient

// NOTE: per capire il senso di questo leggere components.md
#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(joypad_actionclient::JoyActionClient)
