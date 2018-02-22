cmake_minimum_required(VERSION 2.8.3)
project(my_robo_description)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
)

catkin_package(
#  INCLUDE_DIRS include
   LIBRARIES my_robo_description
   CATKIN_DEPENDS roscpp
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(robot_delete_node src/main.cpp)
target_link_libraries(robot_delete_node ${catkin_LIBRARIES})
