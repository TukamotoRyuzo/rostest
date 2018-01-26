# rostest
接客ロボットのリポジトリです。  
navigationを実機とシミュレータで行えます。  
yp-spurで動く差動二輪ロボットを想定し、ros_controlを用いてgazebo、実機を切り替えながら動作します。  
ros_controlとyp-spurの連携に関してはネット上にあまり情報を見かけないので何かの参考になるかもしれません。  
 
* シミュレータでのnavigation
1. `roslaunch my_robo_description spawn_world_b205.launch`  
2. `roslaunch my_robo_control control_base.launch`  
3. `roslaunch my_robo_description spawn_model_b205.launch`  
4. `roslaunch my_robo_2dnav move_base_b205.launch`  
5. `rosrun rviz rviz`  

* ロボットの初期位置を元に戻してnavigationをやり直す
1. `move_base_b205.launch`を起動しているterminalでCtrl+C  
2. `control_base.launch`を起動しているterminalをCtrl+C  
3. `roslaunch my_robo_description spawn_model_b205.launch` でロボットモデルをrespawn  
4. `roslaunch my_robo_control control_base.launch` でコントローラを読み込み直す  
5. `roslaunch my_robo_2dnav move_base_b205.launch` でnaigation再開  
このようにするとgazeboを再起動する必要がないのでちょっとだけストレスが減ります。  
  
* 実機でのnavigation
1. `roslaunch robot_launcher setup_robot.launch`  
2. `roslaunch my_robo_control control_real.launch`  
3. `roslaunch my_robo_control run.launch`  
4. `roslaunch my_robo_2dnav move_base_B205.launch`  
5. `roslaunch simple_goal_generator goal_generator.launch`  
6. `roslaunch robot_launcher setup_okada.launch`  

* もしくはこれらを一括で起動  
`roslaunch robot_launcher navigation_real.launch`  

* テーブル番号の指定  
`rostopic pub -1 /android std_msgs/Int8 [テーブル番号]`  

* 走行命令  
`rostopic pub -1 /diff_drive_controller/cmd_vel geometry_msgs/Twist -- '[2.0, 0.0, 0.0]' '[0.0, 0.0, 1.8]'`  

* 首回転命令  
`rostopic pub -1 /robotics_cmd std_msgs/String 'CMNL30'`  


## 構成

* my_robo_2dnav  
  `navigation`の起動ファイルとコストマップ用のパラメータファイル。

* my_robo_control  
  コントローラのパラメータファイルとコントローラの起動ファイル。`diff_drive_controller`とか。
  あとは実機での動作に必要な`RobotHW`の起動ノード。
  
* my_robo_description  
  ロボットの形状、位置関係定義。`urdf`と、`gazebo`シミュレータの起動ファイル。
  
* robot_launcher  
  ここにある起動ファイルですべての動作を行える。
  
* simple_goal_generator  
  `actionlib`を利用してゴール座標を`navigation`に送る。

* pimouse_vision_control  
  顔認識。顔のある位置をpublish。
  
* android  
  androidタブレットとの通信ノード。
  
* irvs_ros_tutorials  
  ロボットに搭載されている`Arduino`から送られてくるセンサメッセージの受信ノード。  
  顔の位置を見つけてお辞儀をするノード。  
  パッケージ名がチュートリアルのままなのは気にしない。
  
* udev  
  `udev`を用いてUSBデバイスの番号を固定するためのもの。udev/copy.shを実行すれば良い。
