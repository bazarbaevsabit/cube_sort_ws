#!/bin/bash
xhost +si:localuser:root

docker run -it --rm \
    --net host \
    --ipc host \
    -e DISPLAY=$DISPLAY \
    -e XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -v /home/user/my_project/color-follower-ros2/src:/home/work \
    --device /dev/dri:/dev/dri \
    -p 2222:22 \
    --name gazebo_test \
    ros2_full:v1 \
    /bin/bash -c "
            mkdir -p /home/work/prg_w/src
            cd /home/work/prg_w/src
            ros2 pkg create --build-type ament_cmake mobile
            cp -r /home/work/mobile/src/* /home/work/prg_w/src/mobile/
            cd /home/work/prg_w/src
            colcon build
            source install/setup.bash
            ros2 launch mobile launch.py
            exec /bin/bash
        "

xhost -local:docker
