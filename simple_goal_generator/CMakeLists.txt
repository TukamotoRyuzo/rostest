cmake_minimum_required(VERSION 2.8.3)
project(simple_goal_generator)
## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)
find_package(catkin REQUIRED COMPONENTS
  actionlib
  move_base_msgs
  roscpp
)

catkin_package(
  #INCLUDE_DIRS include
  LIBRARIES simple_goal_generator
  CATKIN_DEPENDS actionlib move_base_msgs roscpp
  DEPENDS system_lib
)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(simple_goal_generator_node src/simple_goal_generator.cpp)
add_executable(monitor_battery_node src/monitor_battery.cpp)
add_executable(detect_human_node src/detect_human.cpp)

target_link_libraries(simple_goal_generator_node ${catkin_LIBRARIES})
target_link_libraries(monitor_battery_node ${catkin_LIBRARIES})
target_link_libraries(detect_human_node ${catkin_LIBRARIES})
