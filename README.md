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

