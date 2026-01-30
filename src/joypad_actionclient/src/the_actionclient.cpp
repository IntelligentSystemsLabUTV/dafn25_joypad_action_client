#include <chrono>
#include <sstream>
#include <stdexcept>

#include <joypad_actionclient/the_actionclient.hpp>

#include <params_manager_cpp/params_manager.hpp>

JoypadActionclientNode::JoypadActionclientNode(const rclcpp::NodeOptions & opts)
: Node("the_actionclient_node", opts)
{
  RCLCPP_INFO(this->get_logger(), "[THE JOYPAD ACTIONCLIENT NODE] Main node initialized");

  auto sub_node = std::make_shared<JoySub>();

  auto arm_client_node = std::make_shared<ArmClient>();
  auto disarm_client_node = std::make_shared<DisarmClient>();
  auto landing_client_node = std::make_shared<LandingClient>();
  auto takeoff_client_node = std::make_shared<TakeoffClient>();

  // TODO: assert whether this is good enough (true is for verbosity i.e. if it prints messages onto the terminal)
  auto params_manager = std::make_shared<params_manager::Manager>(this, true);
	init_parameters();

// TODO: initialize all (if any) attributes (variables) we might want to have for the main node
// if you do define something in addition to these you gotta also modify ~/workspace/src/joypad_actionclient/include/joypad_actionclient/the_actionclient.hpp like you would do with a .h file in developing a C application. Here you write the implementation of the functions, there you only declare them by their signature)

// place what I mentioned above in between these comments ^^^

  rclcpp::spin(sub_node);
  rclcpp::shutdown();

  RCLCPP_INFO(this->get_logger(), "[THE JOYPAD ACTIONCLIENT NODE] Main node shutting down...");
}

void JoypadActionclientNode::init_parameters(){
	
	
}

// For methods (functions) you gotta define them outside of the function above (that one "constructs the node)
// If you want to add more functions to this node the signature would be similar to the one above, where the heading `JoypadActionclientNode` specifies that the function name following it is a method of the `JoypadActionclientNode` class (e.g. `JoypadActionclientNode::PrintABC(){` would be how a `PrintABC` method would be declared)

int main(int argc, char ** argv) {
  std::cout << "[MAIN] Locking in" << std::endl;

  rclcpp::init(argc, argv);

  // Create and start main node for the joypad actionclient "service" 
  auto the_actionclient_node = std::make_shared<JoypadActionclientNode>();

  std::cout << "[MAIN] Taking a nap" << std::endl;
  exit(EXIT_SUCCESS);
	return 0;
}
