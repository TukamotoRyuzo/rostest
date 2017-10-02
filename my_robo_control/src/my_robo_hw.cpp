#include <ros/ros.h>
#include <ros/package.h>
#include <angles/angles.h>
#include <my_robo_control/my_robo_hw.h>
#include <iostream> // for debug
#include <math.h>

/*
コンストラクタではJointStateInterfaceとVelocityJointInterfaceを使うための初期設定が記述されています。
cmd_[0]とcmd_[1]にはそれぞれright_wheel_jointとleft_wheel_jointへの速度の指令値がコントローラ(ここではDiffDriveController)によって更新されており、
read関数ではこれを元に実機の車輪を駆動します。
また、実機からのセンサデータを元にwrite関数でpos_とvel_を更新することでJointStateInterfaceを介してコントローラに伝達されます。
DiffDriveControllerはこの情報からnav_msgs::Odometryメッセージのパブリッシュとtfの発行を行います。
*/
MyRobo::MyRobo()
{
    pos_[0] = 0.0; pos_[1] = 0.0;
    vel_[0] = 0.0; vel_[1] = 0.0;
    eff_[0] = 0.0; eff_[1] = 0.0;
    cmd_[0] = 0.0; cmd_[1] = 0.0;

    hardware_interface::JointStateHandle state_handle_1("right_wheel_joint", &pos_[0], &vel_[0], &eff_[0]);
    jnt_state_interface_.registerHandle(state_handle_1);

    hardware_interface::JointStateHandle state_handle_2("left_wheel_joint", &pos_[1], &vel_[1], &eff_[1]);
    jnt_state_interface_.registerHandle(state_handle_2);

    registerInterface(&jnt_state_interface_);

    hardware_interface::JointHandle vel_handle_1(jnt_state_interface_.getHandle("right_wheel_joint"), &cmd_[0]);
    jnt_vel_interface_.registerHandle(vel_handle_1);

    hardware_interface::JointHandle vel_handle_2(jnt_state_interface_.getHandle("left_wheel_joint"), &cmd_[1]);
    jnt_vel_interface_.registerHandle(vel_handle_2);

    registerInterface(&jnt_vel_interface_);
}

// readとwriteをypspur_rosへのトピックのpublish, subscribeに変更すれば動くか？？？
void MyRobo::read(ros::Time time, ros::Duration period)
{
	//ROS_INFO_STREAM("Commands for joints: " << cmd_[0] << ", " << cmd_[1]);
}

void MyRobo::write(ros::Time time, ros::Duration period)
{
    //Update pos_ and vel_
    pos_[0] = cmd_[0];
}

