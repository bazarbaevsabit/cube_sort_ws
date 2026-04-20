###🎨 Color Follower (ROS2 + Gazebo)

**Данный проект создан для помощи новичкам в разворачивании окружения для ROS2**

Внутри проекта реализован робот, который едет вдоль цветных кубов и выводит название распознанного цвета в консоль.

**Стек:**
- Ubuntu 24.04  
- ROS2 Jazzy  
- Gazebo Harmonic  
- Docker

Запускать можно двумя способами:

**Способ 1: Docker (рекомендуемый)**


 git clone https://github.com/bazarbaevssabit/color-follower-ros2.git  
 cd color-follower-ros2  
 chmod +x run.sh  
 ./run.sh  


После запуска ты окажешься внутри контейнера в папке `/home/work/project_w`.

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
	    -v /home/ros2/1/cont/src:/home/work \  и прописать вместо  /home/ros2/1/cont/src свой путь к скаченным файлам.
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

-<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/c2021761-14d4-4067-a1ae-430768591553" />
-<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/4207cba6-b3f2-42c6-8017-51df030e2f7d" />
-<img width="1286" height="270" alt="image" src="https://github.com/user-attachments/assets/0f015f08-5392-4401-aa4e-3b32688e9f49" />


*📝 Лицензия*

MIT


