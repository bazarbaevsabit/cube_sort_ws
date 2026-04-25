import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

def generate_launch_description():
    # Получаем путь к установленному пакету mobile (в нём лежат world, description и т.д.)
    mobile_pkg_dir = get_package_share_directory('mobile')

    # Пути к основным файлам
    sdf_file = os.path.join(mobile_pkg_dir, 'world', 'test_world.sdf')          # мир Gazebo с кубами и роботом
    urdf_file = os.path.join(mobile_pkg_dir, 'description', 'robot.urdf')       # URDF-описание робота (для RViz)
    rviz_config_path = os.path.join(mobile_pkg_dir, 'config', 'view_robot.rviz')# готовая конфигурация RViz

    # Читаем URDF, чтобы передать его в robot_state_publisher
    with open(urdf_file, 'r') as infp:
        robot_desc = infp.read()

    # ============ Запуск Gazebo ============
    # Используем готовый лаунч-файл из пакета ros_gz_sim,
    # передавая ему путь к нашему world-файлу.
    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py')]),
        launch_arguments={'gz_args': sdf_file}.items()
    )

    # ============ Мост ROS 2 <-> Gazebo ============
    # Этот узел "перекидывает" данные между Gazebo и ROS2.
    # Слева — топик ROS, справа — топик Gazebo.
    # Синтаксис: /рос_топик@тип_сообщения_ROS[тип_сообщения_Gazebo
    #   ']' в конце означает, что мы публикуем в Gazebo (команды),
    #   '[' в конце — что мы подписываемся на Gazebo (данные с сенсоров).
    bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '/lidar@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan',       # лидар -> ROS
            '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock',            # время симуляции -> ROS
            '/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist',          # команды движения в Gazebo
            '/model/vehicle_blue/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry',  # одометрия -> ROS
            '/camera/image_raw@sensor_msgs/msg/Image@gz.msgs.Image'    # изображение с камеры -> ROS
        ],
        output='screen'
    )

    # ============ Публикация модели робота ============
    # robot_state_publisher берёт URDF и публикует его в /robot_description.
    # Это нужно RViz и другим нодам, чтобы знать геометрию робота.
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='both',
        parameters=[{'robot_description': robot_desc}]
    )

    # ============ Публикация состояний сочленений ============
    # joint_state_publisher публикует положение всех нефиксированных сочленений.
    # В нашем случае колёса вращаются, данные берутся из Gazebo через bridge.
    joint_state_publisher = Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
        name='joint_state_publisher'
    )

    # ============ RViz ============
    # Запускаем готовую визуализацию с лидаром, моделью и камерой.
    rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_config_path]
    )

    # ============ Наш узел — детектор цвета ============
    # Этот узел подписывается на изображение с камеры, определяет цвет куба
    # и публикует команды на движение вперёд.
    color_follower = Node(
        package='mobile',
        executable='color_follower',
        name='color_follower',
        output='screen'
    )

    # Возвращаем описание запуска: все перечисленные узлы стартуют вместе.
    # Порядок не важен, launch-система сама разберётся с зависимостями.
    return LaunchDescription([
        color_follower,
        gz_sim,
        bridge,
        robot_state_publisher,
        joint_state_publisher,
        rviz
    ])