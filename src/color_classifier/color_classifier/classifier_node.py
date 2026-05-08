import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from .model import detect_color_hsv

class ColorClassifier(Node):
    def __init__(self):
        super().__init__('color_classifier')
        self.sub = self.create_subscription(Image, '/camera/image_raw', self.callback, 10)
        self.bridge = CvBridge()
        self.get_logger().info("Color classifier ready (HSV mode)")

    def callback(self, msg):
        cv_img = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        color = detect_color_hsv(cv_img)
        self.get_logger().info(f"Detected color: {color}")

def main(args=None):
    rclpy.init(args=args)
    node = ColorClassifier()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
