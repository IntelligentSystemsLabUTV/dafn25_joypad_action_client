import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    pkg_share = get_package_share_directory('joypad_actionclient')
    params_file = os.path.join(pkg_share, 'config', 'joypad_actionclient_parameters.yaml')

    # NOTE: la struttura è la stessa di alcuni ros2_examples
    # Per dettagli leggere components.md 
    container = ComposableNodeContainer(
        name='joypad_action_container',
        namespace='joypad_actionclient',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            ComposableNode(
                package='joypad_actionclient',
                plugin='joypad_actionclient::JoyActionClient',
                name='joypad_actionclient',
                parameters=[params_file],
                extra_arguments=[{'use_intra_process_comms': True}]
            )
        ],
        output='screen',
    )

    return LaunchDescription([
        container
    ])
