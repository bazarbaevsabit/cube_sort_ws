cmake_minimum_required(VERSION 3.8)
project(mobile)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()


add_executable(color_follower src/color_follower.cpp)

ament_target_dependencies(color_follower
  rclcpp
  sensor_msgs
  geometry_msgs
  cv_bridge
  image_transport
)
target_link_libraries(color_follower ${OpenCV_LIBRARIES})
target_include_directories(color_follower PRIVATE ${OpenCV_INCLUDE_DIRS})

install(TARGETS
  color_follower
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  world
  config
  description
  DESTINATION share/${PROJECT_NAME}
)


ament_package()
