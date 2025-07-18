"""
Joypad launch file for twist-based velocity control.

July 18, 2025
"""

from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode


def generate_launch_description():
    ld = LaunchDescription()

    container = ComposableNodeContainer(
        name='joy_container',
        namespace='',
        package='dua_app_management',
        executable='dua_component_container_mt',
        emulate_tty=True,
        output='both',
        log_cmd=True,
        composable_node_descriptions=[
            ComposableNode(
                package='joy',
                plugin='joy::GameController',
                name='game_controller',
                namespace='',
                parameters=[
                    {
                        'device_id': 0,
                        'deadzone': 0.05,
                        'autorepeat_rate': 20.0,
                        'sticky_buttons': False,
                        'coalesce_interval_ms': 1
                    }
                ],
                remappings=[
                    ('joy',              '/joy'),
                    ('joy/set_feedback', '/joy/set_feedback')
                ]
            ),
            ComposableNode(
                package='teleop_twist_joy',
                plugin='teleop_twist_joy::TeleopTwistJoy',
                name='teleop_twist_joy',
                namespace='',
                parameters=[
                    {
                        'require_enable_button': True,
                        'enable_button': 10,
                        'enable_turbo_button': 9,
                        'axis_linear.x': 3,
                        'axis_linear.y': 2,
                        'axis_linear.z': 1,
                        'scale_linear.x': 2.0,
                        'scale_linear.y': 2.0,
                        'scale_linear.z': 0.0,
                        'scale_linear_turbo.x': 4.0,
                        'scale_linear_turbo.y': 4.0,
                        'scale_linear_turbo.z': 0.0,
                        'axis_angular.yaw': 0,
                        'scale_angular.yaw': 0.45,
                        'scale_angular_turbo.yaw': 0.9,
                        'publish_stamped_twist': False
                    }
                ],
                remappings=[
                    ('joy',     '/joy'),
                    ('cmd_vel', '/cmd_vel')
                ]
            )
        ]
    )
    ld.add_action(container)

    return ld
