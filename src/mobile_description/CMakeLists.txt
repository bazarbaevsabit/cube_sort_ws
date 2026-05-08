cmake_minimum_required(VERSION 3.8)
project(mobile_description)

find_package(ament_cmake REQUIRED)

# Устанавливаем папки launch, rviz и urdf в share
install(DIRECTORY launch rviz2 urdf
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
