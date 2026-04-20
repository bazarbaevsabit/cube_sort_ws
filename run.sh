#!/bin/bash
xhost +si:localuser:root

docker run -it --rm \
    --net host \
    --ipc host \
    -e DISPLAY=$DISPLAY \
    -e XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -v /home/color-follower-ros2/src:/home/work \
    --device /dev/dri:/dev/dri \
    -p 2222:22 \
    --name gazebo_test \
    ros2full:v1 \
    /bin/bash -c "
            cd /home/work
            cd project_w/src
            ros2 pkg create --build-type ament_cmake mobile
            cd /home/work/project_w
            colcon build
            source install/setup.bash
            exec /bin/bash
        "

xhost -local:docker
