import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Image
from nav_msgs.msg import Odometry
from cv_bridge import CvBridge
import cv2
import os
import math
from rclpy.qos import qos_profile_sensor_data
class DatasetCollector(Node):
    def __init__(self):
        super().__init__('dataset_collector')
        # Укажите ВАШ топик cmd_vel (проверьте через ros2 topic list)
        self.cmd_pub = self.create_publisher(Twist, '/cmd_vel', 10)
        # Подпишитесь на ВАШ топик одометрии (замените /odom, если нужно)
        self.odom_sub = self.create_subscription(Odometry, '/odom', self.odom_callback, 10)
        self.image_sub = self.create_subscription(Image, '/camera/image_raw', self.image_callback, 10)
        self.bridge = CvBridge()

        self.targets = [
            {'x': 2.0, 'color': 'red'},
            {'x': 4.5, 'color': 'green'},
            {'x': 7.0, 'color': 'blue'}
        ]
        self.samples_per_target = 50
        self.speed_linear = 0.2
        self.tolerance = 0.15
        self.angle_step = 0.4
        self.angle_samples = 5
        self.save_dir = os.path.expanduser('~/dataset')

        self.current_x = 0.0
        self.current_yaw = 0.0
        self.phase = 'DRIVE'
        self.target_index = 0
        self.captured = 0
        self.angle_done = 0
        self.rotate_direction = 1

        # Создаём папки заранее с проверкой прав
        for t in self.targets:
            folder = os.path.join(self.save_dir, t['color'])
            os.makedirs(folder, exist_ok=True)
            # Тестовая запись, чтобы убедиться в правах
            test_file = os.path.join(folder, 'test_write.txt')
            try:
                with open(test_file, 'w') as f:
                    f.write('ok')
                os.remove(test_file)
                self.get_logger().info(f'Folder {folder} is writable')
            except Exception as e:
                self.get_logger().error(f'Cannot write to {folder}: {e}')

        self.create_timer(0.1, self.control_loop)
        self.create_timer(2.0, self.print_status)  # периодический статус
        self.get_logger().info('Dataset collector started')

   
    def odom_callback(self, msg):
        self.get_logger().info(f'ODOM received: x={msg.pose.pose.position.x:.2f}', throttle_duration_sec=1.0)
        self.current_x = msg.pose.pose.position.x

    def image_callback(self, msg):
        if self.phase != 'WAIT':
            self.get_logger().info(f'Image received but phase={self.phase} (not saving)')
            return
        cv_img = self.bridge.imgmsg_to_cv2(msg, 'bgr8')
        color = self.targets[self.target_index]['color']
        folder = os.path.join(self.save_dir, color)
        os.makedirs(folder, exist_ok=True)
        filename = f"{len(os.listdir(folder)):04d}.jpg"
        cv2.imwrite(os.path.join(folder, filename), cv_img)
        self.captured += 1
        self.get_logger().info(f"Saved {color}/{filename} ({self.captured}/{self.samples_per_target})")
        if self.captured >= self.samples_per_target:
            self.captured = 0
            self.phase = 'DRIVE'
            self.target_index += 1
            self.angle_done = 0

    def control_loop(self):
        if self.target_index >= len(self.targets):
            self.get_logger().info('Dataset collection complete!')
            cmd = Twist()
            self.cmd_pub.publish(cmd)
            rclpy.shutdown()
            return

        target_x = self.targets[self.target_index]['x']
        cmd = Twist()

        if self.phase == 'DRIVE':
            error = target_x - self.current_x
            self.get_logger().info(f'DRIVE: target_x={target_x}, current_x={self.current_x:.2f}, error={error:.2f}')
            if abs(error) < self.tolerance:
                self.phase = 'ROTATE'
                self.get_logger().info(f'Reached {target_x}, now rotating')
            else:
                cmd.linear.x = self.speed_linear if error > 0 else -self.speed_linear

        elif self.phase == 'ROTATE':
            self.get_logger().info('ROTATE phase')
            cmd.angular.z = 0.3 * self.rotate_direction
            self.angle_done += 0.03
            if abs(self.angle_done) >= self.angle_samples * 2:
                self.phase = 'WAIT'
                self.captured = 0
                self.angle_done = 0
                self.get_logger().info('Starting capture (WAIT phase)')

        self.cmd_pub.publish(cmd)

    def print_status(self):
        self.get_logger().info(f'Status: phase={self.phase}, x={self.current_x:.2f}, target_idx={self.target_index}')

def main(args=None):
    rclpy.init(args=args)
    node = DatasetCollector()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()