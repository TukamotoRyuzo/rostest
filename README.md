# rostest
接客ロボットのリポジトリです。  
navigationを実機とシミュレータで行えるようにすることが目標です。  
yp-spurで動く差動二輪ロボットを想定し、ros_controlを用いてgazebo、実機を切り替えながら動作させたいと思っています。  
ros_controlとyp-spurの連携に関してはネット上にあまり情報を見かけないので何かの参考になるかもしれません。（今取り組んでいるところです）  
 
シミュレータでの起動は下記コマンドを実行すればできます。  
roslaunch my_robo_description gazebo.launch  
roslaunch my_robo_2dnav move_base.launch  
rosrun rviz rviz  

実機でのnavigationはまだできません。  

