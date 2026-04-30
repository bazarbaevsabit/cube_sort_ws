from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess, TimerAction
from launch.substitutions import Command, LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch_ros.parameter_descriptions import ParameterValue

def generate_launch_description():
    pkg_share = FindPackageShare('arm_description')
 
    urdf_file = PathJoinSubstitution([pkg_share, 'urdf', 'arm_basic.urdf'])

    # Аргументы командной строки
    world_name_arg = DeclareLaunchArgument('world_name', default_value='car_world')
    x_arg = DeclareLaunchArgument('x_pose', default_value='0.0')
    y_arg = DeclareLaunchArgument('y_pose', default_value='0.0')
    z_arg = DeclareLaunchArgument('z_pose', default_value='0.0')
    roll_arg = DeclareLaunchArgument('roll', default_value='0.0')
    pitch_arg = DeclareLaunchArgument('pitch', default_value='0.0')
    yaw_arg = DeclareLaunchArgument('yaw', default_value='0.0')
    name_arg = DeclareLaunchArgument('robot_name', default_value='simple_knee_arm')

    world_name = LaunchConfiguration('world_name')
    x = LaunchConfiguration('x_pose')
    y = LaunchConfiguration('y_pose')
    z = LaunchConfiguration('z_pose')
    roll = LaunchConfiguration('roll')
    pitch = LaunchConfiguration('pitch')
    yaw = LaunchConfiguration('yaw')
    robot_name = LaunchConfiguration('robot_name')

    # Мосты ROS ↔ Gazebo (используем полные gz-типы)
    bridges = ExecuteProcess(
        cmd=['ros2', 'run', 'ros_gz_bridge', 'parameter_bridge',
            '/model/simple_knee_arm/joint/knee_joint/cmd_pos@std_msgs/msg/Float64@gz.msgs.Double'
        ],
        output='screen'
    )

    # Robot state publisher
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'robot_description': ParameterValue(
                Command(['cat ', urdf_file]),
                value_type=str
            )
        }]
    )

    spawn_robot = ExecuteProcess(
        cmd=[
            'ros2', 'run', 'ros_gz_sim', 'create',
            '-world', world_name,
            '-topic', '/robot_description',
            '-name', robot_name,
            '-x', x,
            '-y', y,
            '-z', z,
            '-R', roll,
            '-P', pitch,
            '-Y', yaw
        ],
        output='screen'
    )


    spawn_delayed = TimerAction(period=2.0, actions=[spawn_robot])

    return LaunchDescription([
        world_name_arg,
        x_arg, y_arg, z_arg,
        roll_arg, pitch_arg, yaw_arg,
        name_arg,
        robot_state_publisher,
        spawn_delayed,
        bridges,
    ])
