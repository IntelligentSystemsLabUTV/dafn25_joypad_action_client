#include <chrono>
#include <sstream>
#include <stdexcept>

#include <joypad_actionclient/arm_client.hpp>
#include <joypad_actionclient/disarm_client.hpp>
#include <joypad_actionclient/landing_client.hpp>
#include <joypad_actionclient/takeoff_client.hpp>
#include <joypad_actionclient/joy_subscriber.hpp>

int main(int argc, char ** argv) {
  std::cout << "[MAIN] Lock in" << std::endl;

  rclcpp::init(argc, argv);
  auto sub_node = std::make_shared<JoySub>();

  auto arm_client_node = std::make_shared<ArmClient>();
  auto disarm_client_node = std::make_shared<DisarmClient>();
  auto landing_client_node = std::make_shared<LandingClient>();
  auto takeoff_client_node = std::make_shared<TakeoffClient>();

  rclcpp::spin(sub_node);
  rclcpp::shutdown();

  std::cout << "[MAIN] Shutting down" << std::endl;
  exit(EXIT_SUCCESS);
	return 0;
}
