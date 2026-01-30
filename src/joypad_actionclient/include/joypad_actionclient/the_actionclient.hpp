#include <joypad_actionclient/arm_client.hpp>
#include <joypad_actionclient/disarm_client.hpp>
#include <joypad_actionclient/takeoff_client.hpp>
#include <joypad_actionclient/landing_client.hpp>

#include <joypad_actionclient/joy_subscriber.hpp>
#include <params_manager.hpp>

class JoypadActionclientNode : public rclcpp::Node
{
	public:
		explicit JoypadActionclientNode(const rclcpp::NodeOptions & opts = rclcpp::NodeOptions());

		std::shared_ptr<JoySub> sub_node;

		std::shared_ptr<ArmClient> 			arm_client_node;
		std::shared_ptr<DisarmClient> 	disarm_client_node;
		std::shared_ptr<TakeoffClient> 	takeoff_client_node;
		std::shared_ptr<LandingClient> 	landing_client_node;

		std::shared_ptr<params_manager::Manager> params_manager;
		void init_parameters();
		
	// TODO: define any other attributes (variables) or methods (functions) we might want to define for this node (e.g. callback send goals for every managed action client?)
	// you can also make them private
};
