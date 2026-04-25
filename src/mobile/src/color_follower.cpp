#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>
#include "mobile/color_utils.hpp"


class SimpleColorDetector : public rclcpp::Node
{
public:
    SimpleColorDetector() : Node("simple_color_detector")
    {
        // Подписываемся на топик с изображением с камеры робота
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 10,
            std::bind(&SimpleColorDetector::imageCallback, this, std::placeholders::_1));

        // Публикуем команды движения (линейная и угловая скорость)
        cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        // Постоянная линейная скорость (м/с), можно менять для экспериментов
        constant_linear_ = 0.3;

        // Сразу отправляем команду «вперёд», чтобы робот начал движение
        sendStraightCommand();

        RCLCPP_INFO(this->get_logger(), "Started. Detecting 7 cubes only.");
    }

private:
    // Отправка команды на движение прямо с постоянной скоростью
    void sendStraightCommand()
    {
        geometry_msgs::msg::Twist twist;
        twist.linear.x = constant_linear_;
        twist.angular.z = 0.0;   // без поворота
        cmd_pub_->publish(twist);
    }

    
 

    /**
     * Callback: вызывается при получении каждого нового кадра с камеры.
     * Обрабатывает изображение, находит самый крупный цветной объект в центре,
     * распознаёт его цвет и печатает в лог.
     */
    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        // Преобразуем ROS-сообщение в матрицу OpenCV (BGR)
        cv_bridge::CvImagePtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge error: %s", e.what());
            return;
        }

        // Переводим в цветовое пространство HSV – удобнее для анализа цвета
        cv::Mat hsv;
        cv::cvtColor(cv_ptr->image, hsv, cv::COLOR_BGR2HSV);

        // Бинарная маска: оставляем пиксели с насыщенностью > 80 и яркостью > 50
        // (отсекаем почти белые, серые и тёмные области)
        cv::Mat mask;
        cv::inRange(hsv, cv::Scalar(0, 80, 50), cv::Scalar(180, 255, 255), mask);

        // Ищем внешние контуры найденных цветных пятен
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Выбираем самый большой контур (предполагаем, что это целевой куб)
        double max_area = 0;
        std::vector<cv::Point> largest_contour;

        for (const auto& c : contours) {
            double area = cv::contourArea(c);
            if (area > max_area) {
                max_area = area;
                largest_contour = c;
            }
        }

        // Минимальная площадь контура, чтобы не реагировать на шум
        if (max_area > 500) {
            cv::Rect bbox = cv::boundingRect(largest_contour);
            int center_x = cv_ptr->image.cols / 2;              // центр кадра по горизонтали
            int obj_center = bbox.x + bbox.width / 2;           // центр объекта

            // Проверяем, находится ли объект примерно в центре изображения
            // (в пределах четверти ширины кадра от центра)
            if (std::abs(obj_center - center_x) < cv_ptr->image.cols / 4) {
                // Создаём маску только для этого контура и вычисляем средний HSV-цвет
                cv::Mat contour_mask = cv::Mat::zeros(hsv.size(), CV_8UC1);
                cv::drawContours(contour_mask, std::vector<std::vector<cv::Point>>{largest_contour},
                                 -1, 255, cv::FILLED);

                cv::Scalar mean_hsv = cv::mean(hsv, contour_mask);
                std::string color_name = getColor(mean_hsv[0], mean_hsv[1], mean_hsv[2]);

                // Печатаем название цвета только при смене (чтобы избежать спама в логах)
                if (color_name != "NO_COLOR" && color_name != last_color_) {
                    RCLCPP_INFO(this->get_logger(), "%s", color_name.c_str());
                    last_color_ = color_name;
                }
            }
        }

        // После каждой обработки кадра продолжаем движение вперёд
        sendStraightCommand();
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    double constant_linear_;
    std::string last_color_;   // запоминаем предыдущий цвет, чтобы не дублировать вывод
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleColorDetector>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}