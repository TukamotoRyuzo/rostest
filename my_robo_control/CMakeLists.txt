cmake_minimum_required(VERSION 2.8.3)
project(my_robo_control)
find_package(catkin REQUIRED COMPONENTS
  controller_manager
  hardware_interface
  roscpp
)

catkin_package(
   INCLUDE_DIRS include
#  LIBRARIES my_robo_control
   CATKIN_DEPENDS controller_manager hardware_interface roscpp transmission_interface
#  DEPENDS system_lib
)

find_path(ypspur_INCLUDE_DIRS ypspur.h
  HINTS /usr/local/include/ /usr/include/ /opt/local/include)

find_path(ypspur_LIB_DIR libypspur.so
  HINTS /usr/local/lib /usr/lib /opt/local/lib)

find_library(ypspur_LIBRARIES ypspur
  HINTS /usr/local/lib /usr/lib /opt/local/lib)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(my_robo_control src/my_robo_control.cpp src/my_robo_hw.cpp)
add_executable(head_control src/head_control.cpp)
add_executable(my_robo_head_control src/my_robo_head_control.cpp)

target_link_libraries(my_robo_control
  ${catkin_LIBRARIES}
  ${ypspur_LIBRARIES}
)
target_link_libraries(head_control
  ${catkin_LIBRARIES}
)
target_link_libraries(my_robo_head_control
  ${catkin_LIBRARIES}
)

install(TARGETS my_robo_control
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
