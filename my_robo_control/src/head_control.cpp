 
// ~/catkin_ws/src/armbot2/src/teleop.cpp
#include <ros/ros.h>  // rosで必要はヘッダーファイル
#include <std_msgs/Float64.h>
#include <sensor_msgs/JointState.h>
#include <nav_msgs/Odometry.h>

using namespace std;
std_msgs::Float64 tmp_joint1;
double pos_x, pos_y, pos_z;

void monitorJointState(const sensor_msgs::JointState::ConstPtr& jointstate)
{
  tmp_joint1.data = jointstate->position[0];
}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "head_control");  // ノードの初期化
  ros::NodeHandle nh; // ノードハンドラ  
  ros::Publisher  pub_joint1;  // パブリッシャの作成
  ros::Subscriber sub_joints;  // サブスクライバの作成
  std_msgs::Float64 target_joint1;

  pub_joint1 = nh.advertise<std_msgs::Float64>("/my_robo/head_position_controller/command", 100);
  sub_joints = nh.subscribe("/my_robo/joint_states", 100, monitorJointState);
  target_joint1.data = 0;

  while (ros::ok()) { // このノードが使える間は無限ループ
    char key;  // 入力キーの値

    ROS_INFO("[Input] j: Joint1++, f: Joint1--");
    cin >> key; 
    cout << key << endl;

    switch (key) {
    case 'j': target_joint1.data  +=  5 * M_PI/180.0; break;
    case 'f': target_joint1.data  -=  5 * M_PI/180.0; break;
    default: ROS_INFO("Input j,f");
    }
      
    pub_joint1.publish(target_joint1); // 角度を送信    
    ROS_INFO("Targe: Joint1=%f", target_joint1.data);

    usleep(1000); // 制御に時間がかかるので1秒寝て待つ
    ros::spinOnce();   // コールバック関数を呼ぶ
    ROS_INFO("Tmp:Joint1=%f", tmp_joint1.data);
  }
  
  return 0;
}
