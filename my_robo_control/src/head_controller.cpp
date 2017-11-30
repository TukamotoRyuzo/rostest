 
// ~/catkin_ws/src/armbot2/src/teleop.cpp
#include "ros/ros.h"  // rosで必要はヘッダーファイル
#include "std_msgs/Float64.h"
#include "sensor_msgs/JointState.h"
#include "nav_msgs/Odometry.h"

using namespace std;
std_msgs::Float64 tmp_joint1, tmp_joint2;
double pos_x, pos_y, pos_z;

void monitorJointState(const sensor_msgs::JointState::ConstPtr& jointstate)
{
  tmp_joint1.data = jointstate->position[0];
  tmp_joint2.data = jointstate->position[1]; 
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "head_control");
  ros::NodeHandle nh;
  ros::Publisher  pub_joint1, pub_joint2; // パブリッシャの作成
  ros::Subscriber sub_joints, sub_sensor; // サブスクライバの作成

  std_msgs::Float64 target_joint1, target_joint2;

  pub_joint1 = nh.advertise<std_msgs::Float64>("/my_robo/head_position_controller/command", 100);
  pub_joint2 = nh.advertise<std_msgs::Float64>("/my_robo/neck_position_controller/command", 100);
  sub_joints = nh.subscribe("/my_robo/joint_states", 100, monitorJointState);

  target_joint1.data = 0;
  target_joint2.data = 0;

  ros::Rate rate(1.0);
  double add_radian_h = 5 * M_PI / 180.0;
  double add_radian_n = 5 * M_PI / 180.0;

  while (ros::ok())
  {
    target_joint1.data += add_radian_h;
    target_joint2.data += add_radian_n;
    
    if (abs(target_joint1.data) >= 10 * M_PI / 180.0)
        add_radian_h = -add_radian_h;
    if (abs(target_joint2.data) >= 30 * M_PI / 180.0)
        add_radian_n = -add_radian_n;

    pub_joint1.publish(target_joint1);
    pub_joint2.publish(target_joint2);
    ros::spinOnce();
    ROS_INFO("Targe: Joint1=%f Joint2=%f", target_joint1.data, target_joint2.data);
    rate.sleep();
  }
  
  return 0;
}

