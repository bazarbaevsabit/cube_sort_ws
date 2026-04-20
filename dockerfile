FROM osrf/ros:jazzy-desktop-full

ENV DEBIAN_FRONTEND=noninteractive

# Все пакеты в одном RUN (быстрее сборка)
RUN apt-get update && apt-get install -y \
    openssh-server \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    gnupg \
    lsb-release \
    sudo \
    nano \
    micro \
    mc \
    ros-jazzy-joint-state-publisher-gui \
    ros-jazzy-xacro \
    ros-jazzy-robot-state-publisher \
    ros-jazzy-ros-gz-bridge \
    ros-jazzy-teleop-twist-keyboard \
    ros-jazzy-rviz2 \
    ros-jazzy-rqt-image-view \
    python3-pip \
    python3-colcon-common-extensions \
    python3-rosdep \
    && echo "PermitRootLogin yes" >> /etc/ssh/sshd_config \
    && echo "root:ros2" | chpasswd \
    && mkdir -p /var/run/sshd \
    && rm -rf /var/lib/apt/lists/*

RUN echo "source /opt/ros/jazzy/setup.bash" >> /root/.bashrc
RUN rosdep update

EXPOSE 22
WORKDIR /home/work
RUN echo "cd /home/work" >> /root/.bashrc

CMD ["/usr/sbin/sshd", "-D"]
ENV DEBIAN_FRONTEND=
