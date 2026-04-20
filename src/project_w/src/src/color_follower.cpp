#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>

class UniversalColorDetector : public rclcpp::Node
{
public:
    UniversalColorDetector() : Node("universal_color_detector")
    {
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 10,
            std::bind(&UniversalColorDetector::imageCallback, this, std::placeholders::_1));
        cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        constant_linear_ = 0.3;
        sendStraightCommand();

        RCLCPP_INFO(this->get_logger(), "Node started. Driving straight & detecting any color.");
    }

private:
    void sendStraightCommand()
    {
        geometry_msgs::msg::Twist twist;
        twist.linear.x = constant_linear_;
        twist.angular.z = 0.0;
        cmd_pub_->publish(twist);
    }

    // Определяем название цвета по значению Hue (0..179) и насыщенности/яркости
    std::string getColorName(float hue, float saturation, float value)
    {
        // Серые, белые, чёрные (низкая насыщенность)
        if (saturation < 50) {
            if (value > 200) return "WHITE";
            if (value < 50) return "BLACK";
            return "GRAY";
        }

        // Цветные объекты – используем Hue
        if (hue < 10 || hue >= 170) return "RED";
        if (hue < 25) return "ORANGE";
        if (hue < 35) return "YELLOW";
        if (hue < 85) return "GREEN";
        if (hue < 100) return "CYAN";
        if (hue < 130) return "BLUE";
        if (hue < 160) return "MAGENTA / PURPLE";
        
        return "UNKNOWN";
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
        
        // 1. Выделяем все яркие цветные области (не серые/чёрные)
        cv::Mat colorMask;
        cv::inRange(hsv, cv::Scalar(0, 50, 50), cv::Scalar(180, 255, 255), colorMask);
        
        // 2. Находим контуры
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(colorMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        std::string currentColor = "";
        double maxArea = 500.0; // минимальная площадь для обнаружения

        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            if (area < maxArea) continue;
            
            // Маска для этого контура
            cv::Mat mask = cv::Mat::zeros(hsv.size(), CV_8UC1);
            cv::drawContours(mask, std::vector<std::vector<cv::Point>>{contour}, -1, 255, cv::FILLED);
            
            // Вычисляем средний Hue, Sat, Val внутри контура
            cv::Scalar meanHsv = cv::mean(hsv, mask);
            float meanHue = meanHsv[0];
            float meanSat = meanHsv[1];
            float meanVal = meanHsv[2];
            
            std::string colorName = getColorName(meanHue, meanSat, meanVal);
            if (colorName != "UNKNOWN" && area > maxArea) {
                maxArea = area;
                currentColor = colorName;
            }
        }

        // Вывод только при изменении цвета
        if (!currentColor.empty() && currentColor != lastPrintedColor_) {
            RCLCPP_INFO(this->get_logger(), "Detected color: %s (area: %.0f px)", 
                        currentColor.c_str(), maxArea);
            lastPrintedColor_ = currentColor;
        } else if (currentColor.empty() && !lastPrintedColor_.empty()) {
            lastPrintedColor_ = "";
        }

        sendStraightCommand(); // продолжаем ехать прямо
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    double constant_linear_;
    std::string lastPrintedColor_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<UniversalColorDetector>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}