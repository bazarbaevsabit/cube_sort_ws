***🎨 Color Follower (ROS2 + Gazebo)***

**Данный проект создан для помощи новичкам в разворачивании окружения для ROS2**

Внутри проекта реализован робот, который едет вдоль цветных кубов и выводит название распознанного цвета в консоль.

**Стек:**
- Ubuntu 24.04  
- ROS2 Jazzy  
- Gazebo Harmonic  
- Docker

Запускать можно двумя способами:

**Способ 1: Docker (рекомендуемый)**


 git clone https://github.com/bazarbaevsabit/color-follower-ros2.git  
 cd color-follower-ros2  
 docker build -t ros2_full:v1 .  
 chmod +x run.sh  
 ./run.sh  


После запуска ты окажешься внутри контейнера в папке `/home/work`.

Далее внутри контейнера:


 ros2 launch mobile launch.py


**Способ 2: без Docker (нативно)**

 cd ~/color-follower-ros2/src/mobile  
 colcon build  
 source install/setup.bash  
 ros2 launch mobile launch.py  


**Внутри проекта:**

- `run.sh` – скрипт для запуска Docker-контейнера с пробросом графики  
	внутри данного файла необходимо изменить строку:  
	    -v /home/user/color-follower-ros2/src:/home/work \  и прописать вместо  /home/user/color-follower-ros2/src свой путь к скаченным файлам.  
- `Dockerfile` – сборка образа `ros2_full:v1`  
- `worlds/` – файлы мира Gazebo с цветными кубами   
- `config/` – конфигурация для RVIZ2  
- `description/` – urdf-файл робота  
- `launch/` – лаунч файл для запуска проекта  
- `worlds/` – файлы миров Gazebo с цветными кубами   
- `src/` – нода для запуска робота, и его работы, в ноде описано, каким образом робот делает детекцию цвета куба  




**🔧 Если образ `ros2_full:v1` отсутствует (рекомендую перед запуском) необходимо собрать его из Dockerfile:**  


 docker build -t ros2_full:v1 .  

**📸 Пример работы**  



 <br>
<img width="1000" height="563" alt="4" src="https://github.com/user-attachments/assets/95348996-51de-4547-bf6c-68e6a0350de0" />
<br>
<img width="1920" height="1080" alt="2" src="https://github.com/user-attachments/assets/92d34c14-4b7d-40c0-a090-cf934af1baef" />
<br>
<img width="1804" height="156" alt="3" src="https://github.com/user-attachments/assets/9a2f50d2-c70e-4ce9-a872-9c1921780635" />

*📝 Лицензия*  

MIT  


