# rostest
接客ロボットのリポジトリです。  
navigationを実機とシミュレータで行えるようにすることが目標です。  
yp-spurで動く差動二輪ロボットを想定し、ros_controlを用いてgazebo、実機を切り替えながら動作させたいと思っています。  
ros_controlとyp-spurの連携に関してはネット上にあまり情報を見かけないので何かの参考になるかもしれません。（今取り組んでいるところです）  
 
シミュレータでの起動は下記コマンドを実行すればできます。  
roslaunch my_robo_description gazebo.launch  
roslaunch my_robo_2dnav move_base.launch  
rosrun rviz rviz  

実機での起動は下記コマンドを実行すればできます。
roslaunch robot_launcher setup_robot.launch  
roslaunch my_robo_control control_real.launch  
roslaunch my_robo_2dnav move_base.launch  
roslaunch simple_goal_generator goal_generator.launch  
rostopic pub -1 /table_number std_msgs/Int8 [テーブル番号]  
もしくはrostopic pub -1 /my_robo/diff_drive_controller/cmd_vel geometry_msgs/Twist -- '[2.0, 0.0, 0.0]' '[0.0, 0.0, 1.8]'  
など  

## 構成

* my_robo_2dnav  
  `navigation`の起動ファイルとコストマップ用のパラメータファイル。

* my_robo_control  
  コントローラのパラメータファイルとコントローラの起動ファイル。`diff_drive_controller`とか。
  あとは実機での動作に必要と思われる`RobotHW`の起動ノード。
  `RobotHW`の使い方がまだわからない。
  
* my_robo_description  
  ロボットの形状、位置関係定義。`urdf`と、`gazebo`シミュレータの起動ファイル。
  
* robot_launcher  
  将来的にはここにある起動ファイルですべての動作を行えるようにする。
  
* simple_goal_generator  
  `actionlib`を利用してゴール座標を`navigation`に送る。
  また、バッテリー残量を監視し、必要に応じて充電場所座標を`navigation`に送る。

* udev  
  `udev`を用いてUSBデバイスの番号を固定するためのもの。udev/copy.shを実行すれば良い。
