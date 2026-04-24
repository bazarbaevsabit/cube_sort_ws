#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>

class SimpleColorDetector : public rclcpp::Node
{
public:
    SimpleColorDetector() : Node("simple_color_detector")
    {
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 10,
            std::bind(&SimpleColorDetector::imageCallback, this, std::placeholders::_1));
        cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        constant_linear_ = 0.3;
        sendStraightCommand();
        RCLCPP_INFO(this->get_logger(), "Started. Detecting 7 cubes only.");
    }

private:
    void sendStraightCommand()
    {
        geometry_msgs::msg::Twist twist;
        twist.linear.x = constant_linear_;
        twist.angular.z = 0.0;
        cmd_pub_->publish(twist);
    }

    std::string getColor(float h, float s, float v)
    {
        if (s < 80) return "NO_COLOR";

        if ((h >= 0 && h < 10) || (h >= 170 && h <= 180)) return "RED";
        if (h >= 10 && h < 25)  return "ORANGE";
        if (h >= 25 && h < 35)  return "YELLOW";
        if (h >= 35 && h < 85)  return "GREEN";
        if (h >= 85 && h < 100) return "CYAN";
        if (h >= 100 && h < 130) return "BLUE";
        if (h >= 130 && h < 160) return "PURPLE";

        return "NO_COLOR";
    }

    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        cv_bridge::CvImagePtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge error: %s", e.what());
            return;
        }

        cv::Mat hsv;
        cv::cvtColor(cv_ptr->image, hsv, cv::COLOR_BGR2HSV);

        cv::Mat mask;
        cv::inRange(hsv, cv::Scalar(0, 80, 50), cv::Scalar(180, 255, 255), mask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        double max_area = 0;
        std::vector<cv::Point> largest_contour;

        for (const auto& c : contours) {
            double area = cv::contourArea(c);
            if (area > max_area) {
                max_area = area;
                largest_contour = c;
            }
        }

        if (max_area > 500) {
            cv::Rect bbox = cv::boundingRect(largest_contour);
            int center_x = cv_ptr->image.cols / 2;
            int obj_center = bbox.x + bbox.width/2;

            if (std::abs(obj_center - center_x) < cv_ptr->image.cols / 4) {
                cv::Mat contour_mask = cv::Mat::zeros(hsv.size(), CV_8UC1);
                cv::drawContours(contour_mask, std::vector<std::vector<cv::Point>>{largest_contour}, -1, 255, cv::FILLED);

                cv::Scalar mean_hsv = cv::mean(hsv, contour_mask);
                std::string color_name = getColor(mean_hsv[0], mean_hsv[1], mean_hsv[2]);

                if (color_name != "NO_COLOR" && color_name != last_color_) {
                    RCLCPP_INFO(this->get_logger(), "%s", color_name.c_str());
                    last_color_ = color_name;
                }
            }
        }

        sendStraightCommand();
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    double constant_linear_;
    std::string last_color_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleColorDetector>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}