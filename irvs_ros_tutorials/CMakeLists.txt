cmake_minimum_required (VERSION 2.8.3)
project(irvs_ros_tutorials)
## catkinビルドに必要なコンポーネントパッケージの設定
##これらのパッケージが存在しない場合、catkinビルドの途中でエラーが出る。

find_package(catkin REQUIRED COMPONENTS roscpp std_msgs message_generation)
##メッセージファイルの設定
add_message_files(FILES msgTutorial.msg)

##サービスファイルの設定
add_service_files(FILES srvTutorial.srv)

##  add_message_filesで使用するメッセージの依存関係を設定
##これらのパッケージが存在しない場合、catkinビルドの途中でエラーが出る。
generate_messages(DEPENDENCIES std_msgs)

##インクルードディレクトリ、ライブラリ、catkinビルド、システムに依存するパッケージの指定
catkin_package(
INCLUDE_DIRS include
LIBRARIES irvs_ros_tutorials
CATKIN_DEPENDS roscpp std_msgs
DEPENDS system_lib
)
##インクルードディレクトリの設定
include_directories(include ${catkin_INCLUDE_DIRS})


add_executable(subscriber_face2 src/subscriber_face2.cpp)
target_link_libraries(subscriber_face2 ${catkin_LIBRARIES})
add_dependencies(subscriber_face2
subscriber_face2_cpp)

add_executable(sensor_subscriber src/sensor_subscriber.cpp)
target_link_libraries(sensor_subscriber ${catkin_LIBRARIES})
add_dependencies(sensor_subscriber
sensor_subscriber_cpp)
