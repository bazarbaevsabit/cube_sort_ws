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

# Библиотека с общей функцией распознавания цвета
add_library(color_utils SHARED src/color_utils.cpp)
target_include_directories(color_utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(color_utils
  rclcpp
  sensor_msgs
  geometry_msgs
  cv_bridge
  image_transport
)
target_link_libraries(color_utils ${OpenCV_LIBRARIES})

# Основной исполняемый файл – нода
add_executable(color_follower src/color_follower.cpp)
ament_target_dependencies(color_follower
  rclcpp
  sensor_msgs
  geometry_msgs
  cv_bridge
  image_transport
)
target_link_libraries(color_follower color_utils ${OpenCV_LIBRARIES})
target_include_directories(color_follower PRIVATE include)

install(TARGETS
  color_follower
  color_utils
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  world
  config
  description
  DESTINATION share/${PROJECT_NAME}
)

# Блок тестов (включает линтеры + unit-тесты)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_color test/test_color.cpp)
  target_include_directories(test_color PRIVATE include)
  target_link_libraries(test_color color_utils)
endif()

ament_package()