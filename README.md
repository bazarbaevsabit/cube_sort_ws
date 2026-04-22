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


После запуска ты окажешься внутри контейнера в папке `/home/work/project_w`.

Далее внутри контейнера:


 ros2 launch mobile launch.py


**Способ 2: без Docker (нативно)**

 cd ~/color-follower-ros2/src/project_w  
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

<img width="1920" height="1080" alt="1" src="https://github.com/user-attachments/assets/d34c0282-22fd-496a-b3a5-de5e7424bf39" />  
 <br>
<img width="1920" height="1080" alt="2" src="https://github.com/user-attachments/assets/e70b6359-93c8-48f2-92ef-347d7669a992" />  
<br>
<img width="1346" height="156" alt="3" src="https://github.com/user-attachments/assets/fb38b6b4-d9be-4ebb-b835-486bd415924c" />  
<br>



*📝 Лицензия*  

MIT  


