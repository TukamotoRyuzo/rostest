cmake_minimum_required(VERSION 2.8.3)
project(android)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package(
  LIBRARIES android
  CATKIN_DEPENDS roscpp std_msgs
#  DEPENDS system_lib
)

include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)

add_executable(Android_server src/Android_server.cpp)
target_link_libraries(Android_server ${catkin_LIBRARIES})

add_executable(test_client src/test_client.cpp)
target_link_libraries(test_client ${catkin_LIBRARIES})


