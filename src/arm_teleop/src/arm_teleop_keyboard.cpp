#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string>

class ArmTeleopKeyboard : public rclcpp::Node
{
public:
  ArmTeleopKeyboard()
  : Node("arm_teleop_keyboard")
  {
    publisher_ = this->create_publisher<std_msgs::msg::Float64>(
      "/model/simple_knee_arm/joint/knee_joint/cmd_pos", 10);

    // Сохраняем текущие настройки терминала
    tcgetattr(STDIN_FILENO, &original_termios_);
    // Устанавливаем неканонический режим (без буферизации строк, без эха)
    termios raw = original_termios_;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    RCLCPP_INFO(this->get_logger(),
                "Управление: нажмите 1 - поднять плечо, 2 - опустить, Esc - выход.");

    // Таймер с частотой 50 Гц для опроса клавиш
    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(20),
      std::bind(&ArmTeleopKeyboard::timer_callback, this));
  }

  ~ArmTeleopKeyboard()
  {
    // Восстановление исходных настроек терминала
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_);
    RCLCPP_INFO(this->get_logger(), "Нода завершена.");
  }

private:
  void timer_callback()
  {
    // Проверяем, есть ли данные на стандартном вводе
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    timeval tv = {0, 0};  // неблокирующий опрос
    int ret = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
    if (ret > 0 && FD_ISSET(STDIN_FILENO, &fds))
    {
      char c;
      if (read(STDIN_FILENO, &c, 1) == 1)
      {
        auto msg = std_msgs::msg::Float64();
        bool publish = false;
        if (c == '1')
        {
          msg.data = 1.0;   // поднять плечо (укажите нужное значение)
          publish = true;
          RCLCPP_INFO(this->get_logger(), "Плечо вверх");
        }
        else if (c == '2')
        {
          msg.data = 0.0;   // опустить плечо
          publish = true;
          RCLCPP_INFO(this->get_logger(), "Плечо вниз");
        }
        else if (c == '\x1b')  // Esc
        {
          RCLCPP_INFO(this->get_logger(), "Завершение работы по Esc...");
          rclcpp::shutdown();
          return;
        }

        if (publish) {
          publisher_->publish(msg);
        }
      }
    }
  }

  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  termios original_termios_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ArmTeleopKeyboard>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
