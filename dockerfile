FROM osrf/ros:jazzy-desktop-full
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ros-jazzy-joint-state-publisher \
    ros-jazzy-robot-state-publisher \
    ros-jazzy-ros-gz-bridge \
    ros-jazzy-rviz2 \
    python3-pip \
    python3-colcon-common-extensions \
	python3-rosdep \
    && rm -rf /var/lib/apt/lists/*


WORKDIR /home/work
ENV DEBIAN_FRONTEND=
CMD ["/bin/bash"]
