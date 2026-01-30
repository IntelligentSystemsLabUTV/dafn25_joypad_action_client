import os
from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch_ros.actions import Node 

# TODO: assert whether this is enough
def generate_launch_description():
    ld = LaunchDescription()

    config_file = os.path.join(
        get_package_share_directory('joypad_actionclient'),
        'config',
        'joypad_actionclient_parameters.yaml'
    )

    node = Node(
        package='joypad_actionclient',
        name='joypad_actionclient',
        executable='joypad_actionclient',
        parameters=[config_file]
    )

    ld.add_action(node)
    return ld
