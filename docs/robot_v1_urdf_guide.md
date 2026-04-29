### Общая структура: robot_v1.urdf

Файл написан на языке **URDF (Unified Robot Description Format)**. Его основная идея — представить робота в виде **древовидной структуры из звеньев (links) и сочленений (joints)**.

**Звено (`<link>`)** — это любая твёрдая деталь: корпус, колесо, сенсор. У него может быть:
*   **Масса и момент инерции**, чтобы симуляция была физически правдоподобной.
*   **Визуальная модель**, чтобы мы видели робота на экране.
*   **Коллизионная модель** (упрощённая геометрия для расчёта столкновений).

**Сочленение (`<joint>`)** — это связь между двумя звеньями. Оно задаёт, как одно звено может двигаться относительно другого (вращаться, скользить или быть жёстко закреплённым).

Всё вместе это образует кинематическую схему реального робота.

---

### Пошаговый разбор конструкции

#### 1. Шасси (`chassis`) — Основа робота

```xml
<!-- chassis -->
<link name="chassis">
    <inertial>
        <mass value="1.14395"/>
        <inertia ixx="0.119" ixy="0.0" ixz="0.0" iyy="0.405" iyz="0.0" izz="0.476"/>
    </inertial>
    <visual>
        <geometry>
            <box size="2.0 1.0 0.5"/>
        </geometry>
        <material name="blue">
            <color rgba="0 0 1 1"/>
        </material>
    </visual>
    <collision>
        <geometry>
            <box size="2.0 1.0 0.5"/>
        </geometry>
    </collision>
</link>
```
*   **Имя:** `chassis`. Это главное, «родительское» звено, к которому будет крепиться всё остальное.
*   **Физическая модель (`<inertial>`)** задана масса `1.14395` кг и тензор инерции. Тензор инерции — это матрица 3x3, которая описывает, насколько трудно раскрутить объект вокруг каждой из осей (x, y, z). Например, большое значение `izz` говорит о том, что робота трудно заставить вращаться волчком. Эти числа физический движок (в симуляторе Gazebo) будет использовать для реалистичного расчёта динамики.
*   **Внешний вид (`<visual>`)** робот будет выглядеть как синий (`0 0 1 1` в формате RGBA) параллелепипед (коробка) с габаритами 2.0 м в длину (вдоль оси X), 1.0 м в ширину (вдоль оси Y) и 0.5 м в высоту (вдоль оси Z).
*   **Столкновения (`<collision>`)** для простоты и ускорения расчётов физический движок будет считать шасси коробкой того же размера. Если бы модель была сложной, здесь использовали бы её упрощённую копию.

#### 2. Ведущие колёса (`left_wheel` и `right_wheel`) — Как робот едет

```xml
<!-- left wheel -->
<link name="left_wheel">
    <inertial>
        <mass value="1"/>
        <inertia ixx="0.043333" ixy="0.0" ixz="0.0" iyy="0.043333" iyz="0.0" izz="0.08"/>
    </inertial>
    <visual>
        <geometry>
            <cylinder radius="0.4" length="0.2"/>
        </geometry>
        <material name="red">
            <color rgba="1 0 0 1"/>
        </material>
    </visual>
    <collision>
        <geometry>
            <cylinder radius="0.4" length="0.2"/>
        </geometry>
    </collision>
</link>
<!-- right wheel -->
<link name="right_wheel">
    <inertial>
        <mass value="1"/>
        <inertia ixx="0.043333" ixy="0.0" ixz="0.0" iyy="0.043333" iyz="0.0" izz="0.08"/>
    </inertial>
    <visual>
        <geometry>
            <cylinder radius="0.4" length="0.2"/>
        </geometry>
        <material name="red"/>
    </visual>
    <collision>
        <geometry>
            <cylinder radius="0.4" length="0.2"/>
        </geometry>
    </collision>
</link>
```
*   Оба колеса имеют массу по 1 кг и выглядят как красные цилиндры (радиус 0.4 м, толщина 0.2 м).
*   **Ключевой момент** Обратите внимание на ось симметрии. По правилам URDF, цилиндр по умолчанию ориентирован вдоль оси Z. Это важно для следующих шагов.

#### 3. Роликовое колесо (`caster`) — Третья точка опоры
```xml
<!-- caster -->
<link name="caster">
    <inertial>
        <mass value="1"/>
        <inertia ixx="0.016" ixy="0.0" ixz="0.0" iyy="0.016" iyz="0.0" izz="0.016"/>
    </inertial>
    <visual>
        <geometry>
            <sphere radius="0.2"/>
        </geometry>
        <material name="green">
            <color rgba="0 1 0 1"/>
        </material>
    </visual>
    <collision>
        <geometry>
            <sphere radius="0.2"/>
        </geometry>
    </collision>
</link>
```
*   Это пассивное колесо-сфера радиусом 0.2 м зеленого цвета. Нужно для устойчивости конструкции, чтобы робот не заваливался на бок.

#### 4. Сенсоры робота
Робот оснащён двумя ключевыми сенсорами, которые описаны как отдельные звенья. 
```xml
<!-- camera link -->
<link name="camera_link">
    <inertial>
        <mass value="0.01"/>
        <inertia ixx="1e-5" ixy="0.0" ixz="0.0" iyy="1e-5" iyz="0.0" izz="1e-5"/>
    </inertial>
    <visual>
        <geometry>
            <box size="0.15 0.05 0.05"/>
        </geometry>
        <material name="black">
            <color rgba="0 0 0 1"/>
        </material>
    </visual>
    <collision>
        <geometry>
            <box size="0.1 0.05 0.05"/>
        </geometry>
    </collision>
</link>
<!-- lidar -->
<link name="vehicle_blue/chassis/gpu_lidar">
        <visual>
            <geometry>
                <cylinder radius="0.1" length="0.1"/>
            </geometry>
            <material name="red"/>
        </visual>
</link>
```

*   **Камера (`camera_link`)** представляет собой маленький чёрный параллелепипед на корпусе. Её описание — это «крепление» для будущего виртуального сенсора.
*   **Лидар (`vehicle_blue/chassis/gpu_lidar`)** ещё один цилиндр красного цвета. Обратите внимание: это **только визуальная метка**. У него нет ни массы, ни коллизионной модели (чтобы не влиять на физику). Вся магия самого лидара будет описана отдельно в разделе плагинов.

---

### 🔗 Соединения (`<joint>`) — Собираем робота воедино

Теперь все детали нужно правильно соединить. Каждое соединение описывает, **кто родитель** (`parent`), **кто потомок** (`child`), **где находится** и **как может двигаться**.

#### 1. Приводные колёса (`left_wheel_joint` и `right_wheel_joint`)

```xml
<joint name="left_wheel_joint" type="continuous">
    <origin xyz="-0.5 0.6 0" rpy="-1.5708 0 0"/>
    <parent link="chassis"/>
    <child link="left_wheel"/>
    <axis xyz="0 0 1"/>
</joint>

<joint name="right_wheel_joint" type="continuous">
    <origin xyz="-0.5 -0.6 0" rpy="-1.5708 0 0"/>
    <parent link="chassis"/>
    <child link="right_wheel"/>
    <axis xyz="0 0 1"/>
</joint>
```
Это самые важные соединения, которые делают нашего робота подвижным. Разберём всё до мелочей:
*   **`type="continuous"`** — ключевое слово. Это означает, что соединение может вращаться бесконечно, как настоящее колесо, без ограничений по углу.
*   **`<origin xyz="-0.5 0.6 0">`** — позиция колеса относительно центра шасси. Смещение -0.5 по X (чуть назад) и 0.6 по Y (влево для левого колеса, вправо для правого).
*   **`<axis xyz="0 0 1">`** — вокруг какой оси вращается колесо. 
*   **`rpy="-1.5708 0 0">`**! Это углы **roll-pitch-yaw** в радианах. Значение -1.5708 рад — это -90 градусов вокруг оси X.
     <br>Физический смысл: соединение позволяет колесу вращаться вокруг своей оси. За счёт начального поворота на -90 градусов колесо разворачивается так, чтобы при вращении вокруг своей оси (Z) оно катится вперёд/назад по оси X.

#### 2. Роликовое колесо (`caster_joint`) и Сенсоры (`camera_joint`, `lidar_joint`)

```xml
<joint name="caster_joint" type="continuous">
    <origin xyz="0.8 0 -0.2" rpy="0 0 0"/>
    <parent link="chassis"/>
    <child link="caster"/>
</joint>

<joint name="camera_joint" type="fixed">
    <origin xyz="1.0 0 0.25" rpy="0 0 0"/>
    <parent link="chassis"/>
    <child link="camera_link"/>
</joint>
<joint name="lidar_joint" type="fixed">
    <parent link="chassis"/>
    <child link="vehicle_blue/chassis/gpu_lidar"/>
    <origin xyz="0.8 0 0.3" rpy="0 0 0"/>
</joint>
```
Как видите, всё крепится к `chassis`. Обратите внимание на типы:
*   **`type="continuous"`** — для ролика, чтобы он мог свободно вращаться.
*   **`type="fixed"`** — для камеры и лидара, чтобы они были жёстко закреплены на корпусе.


*   Ролик: спереди (`0.8`) по X и снизу (`-0.2`) по Z, чтобы касаться пола.
*   Камера: на передней части робота (`1.0` по X) и на высоте `0.25`.
*   Лидар: на передней части робота, но выше камеры (`0.35` по Z).

---

### Блоки для симулятора Gazebo 

Для того, чтобы робот мог ехать, получать команды и «видеть» мир, нужны плагины. Блоки `<gazebo>` — это инструкции специально для этого симулятора.

#### 1. Плагин дифференциального привода (`gz-sim-diff-drive-system`)

```xml
<gazebo>
    <plugin filename="gz-sim-diff-drive-system" name="gz::sim::systems::DiffDrive">
        <robot_base_frame>chassis</robot_base_frame>
        <odom_frame_id>odom</odom_frame_id>
        <left_joint>left_wheel_joint</left_joint>
        <right_joint>right_wheel_joint</right_joint>
        <wheel_separation>1.2</wheel_separation>
        <wheel_radius>0.4</wheel_radius>
        <odom_publish_frequency>20</odom_publish_frequency>
        <max_linear_acceleration>1</max_linear_acceleration>
        <min_linear_acceleration>-1</min_linear_acceleration>
        <max_angular_acceleration>2</max_angular_acceleration>
        <min_angular_acceleration>-2</min_angular_acceleration>
        <max_linear_velocity>0.5</max_linear_velocity>
        <min_linear_velocity>-0.5</min_linear_velocity>
        <max_angular_velocity>3.14</max_angular_velocity>
        <min_angular_velocity>-3.14</min_angular_velocity>
        <topic>/cmd_vel</topic>
        
    </plugin>
</gazebo>
```
Этот плагин и есть «мозг» движения. 
*   **`name="gz::sim::systems::DiffDrive"`** это имя плагина в новой версии симулятора **Gazebo **. Он реализует модель **дифференциального привода** два ведущих колеса, скорость и направление движения задаются разницей их скоростей.
*   **`<left_joint>` и `<right_joint>`** указываем, какие именно наши сочленения отвечают за левое и правое колесо. Плагин будет управлять их вращением.
*   **`<wheel_separation>1.25</wheel_separation>`** расстояние между колёсами (колея). 
*   **`<wheel_radius>0.3</wheel_radius>`** радиус колеса. Нужен для пересчёта угловой скорости вращения колеса в линейную скорость робота (`v = ω * R`).
*   **Ограничения скоростей и ускорений** заданы для реалистичности. Например, `max_linear_velocity = 0.5 м/с`, а `max_angular_velocity = 3.14 рад/с` (почти 180 град/с).
*   **`<topic>/cmd_vel</topic>`** это имя топика ROS, на который плагин будет подписан в ожидании команд. Ваша будущая программа управления должна будет публиковать в этот топик сообщения типа `geometry_msgs/Twist`.
*   **`<odom_frame_id>odom</odom_frame_id>`** плагин будет автоматически вычислять, где находится робот в мире (одометрию), и публиковать соответствующую трансформацию (TF).

#### 2. Плагин лидара (gpu_lidar) — Как робот «видит» мир

```xml
<!-- gpu_lidar на chassis -->
<gazebo reference="chassis">
    <sensor name="gpu_lidar" type="gpu_lidar">
        <pose>0.8 0 0.3 0 0 0</pose>
        <topic>lidar</topic>
        <frame_name>vehicle_blue/chassis</frame_name>
        <update_rate>10</update_rate>
        <ray>
            <scan>
                <horizontal>
                    <samples>640</samples>
                    <resolution>1</resolution>
                    <min_angle>-1.396263</min_angle>
                    <max_angle>1.396263</max_angle>
                </horizontal>
                <vertical>
                    <samples>1</samples>
                    <resolution>0.01</resolution>
                    <min_angle>0</min_angle>
                    <max_angle>0</max_angle>
                </vertical>
            </scan>
            <range>
                <min>0.08</min>
                <max>10.0</max>
                <resolution>0.01</resolution>
            </range>
        </ray>
        <always_on>1</always_on>
        <visualize>true</visualize>
    </sensor>
</gazebo>
```
*   **`reference="chassis"`**: Плагин привязан к звену шасси.
*   **`<pose>`**: Задаёт положение сенсора относительно этого звена.
*   **`type="gpu_lidar"`**: Указывает, что для расчётов будет использоваться видеокарта (GPU), что в разы быстрее, чем на центральном процессоре.
*   **`<ray>` и `<scan>`**: Настройки лидара. Он работает по принципу Time-of-Flight (ToF), испуская лучи и замеряя время их возврата.
*   **`<horizontal>`**: Сканирует в горизонтальной плоскости сектор от `-1.396263` до `+1.396263` радиан (примерно от -80 до +80 градусов), делая **640 замеров** (то есть угловое разрешение около 0.25 градуса).
*   **`<vertical>`**: Сканирует в вертикальной плоскости всего 1 срез (`samples=1`), то есть это 2D-лидар.
*   **`<range>`**: Минимальная (`0.08` м) и максимальная (`10.0` м) дальность, на которой лидар «видит» препятствия.
*   **`<topic>lidar</topic>`**: В этот топик будут публиковаться данные типа `sensor_msgs/LaserScan`.
*   **`<always_on>1</always_on>` и `<visualize>true</visualize>`**: Лидар включен всегда и его лучи можно увидеть в окне симулятора 
#### 3. Плагин камеры 

```xml
<!-- camera на camera_link -->
<gazebo reference="camera_link">
    <sensor name="camera_sensor" type="camera">
        <pose>0 0 0 0 0 0</pose>
        <topic>/camera/image_raw</topic>
        <update_rate>30</update_rate>
        <always_on>1</always_on>
        <visualize>1</visualize>
        <camera>
            <horizontal_fov>1.047</horizontal_fov>
            <image>
                <width>640</width>
                <height>480</height>
                <format>R8G8B8</format>
            </image>
            <clip>
                <near>0.1</near>
                <far>100</far>
            </clip>
        </camera>
    </sensor>
</gazebo>

```
*   **`reference="camera_link"`** плагин привязан к звену камеры, поэтому её положение в пространстве будет определено положением звена `camera_link`.
*   **`<horizontal_fov>1.047</horizontal_fov>`** угол обзора по горизонтали. 1.047 радиан — это **60 градусов** (классический угол для многих камер).
*   **`<image>`** задаёт разрешение картинки (640x480 пикселей) и формат пикселя `R8G8B8` (обычное цветное изображение).
*   **`<clip>`** задаёт плоскости отсечения: объекты ближе 0.1 м и дальше 100 м не будут рендериться.
*   **`<topic>/camera/image_raw</topic>`** сюда будут публиковаться сырые данные изображения (`sensor_msgs/Image`).

###  Итог
Это мобильный робот с дифференциальным приводом:
1.  Шасси - представляет собой  "параллелепипед" синего цвета размером 2x1x0.5 метра.
2.  Движение  осуществляется за счёт двух больших красных колёс. Кинематика просчитывается плагином `DiffDrive` на основе команд, поступающих в топик `/cmd_vel`.
3.  Опорой для устойчивости служит маленькое зелёное роликовое колесо спереди.
4.  На борту есть 2D-лидар (640 лучей, дальность 10 м) и цветная камера (640x480, 60°).

