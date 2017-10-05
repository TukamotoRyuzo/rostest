#include <ros/ros.h>
#include <ros/package.h>
#include <angles/angles.h>
#include <my_robo_control/my_robo_hw.h>
#include <iostream> // for debug
#include <ypspur.h>

int MyRobo::open() const
{
    int ret = Spur_init();

    // set_wheel_vel, accelでタイヤ軸の角速度・各加速度を指定し、wheel_ang, velで角度指令・速度指令値を与える。
    YP_set_wheel_vel(13.0, 13.0);
    YP_set_wheel_accel(13.0, 13.0);
    
    return ret;
}

void MyRobo::stop() const
{
    YP_wheel_vel(0, 0);
    Spur_stop();
    ros::Duration(1).sleep();
    Spur_free();
}

/*
コンストラクタではJointStateInterfaceとVelocityJointInterfaceを使うための初期設定が記述されています。
cmd_[0]とcmd_[1]にはそれぞれright_wheel_jointとleft_wheel_jointへの速度の指令値がコントローラ(ここではDiffDriveController)によって更新されており、
read関数ではこれを元に実機の車輪を駆動します。
また、実機からのセンサデータを元にwrite関数でpos_とvel_を更新することでJointStateInterfaceを介してコントローラに伝達されます。
DiffDriveControllerはこの情報からnav_msgs::Odometryメッセージのパブリッシュとtfの発行を行います。
*/

MyRobo::MyRobo()
{
    if (open() < 0)
        ROS_ERROR("cannot open spur.\n");

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
// →ypspur_rosとつなぐのではなく、yp-spur(T-Frog?)と直接やり取りしなければならないっぽい。
// ↓に全部書いてあった・・・・・・
// cf.https://github.com/open-rdc/icart/blob/indigo-devel/icart_mini_driver/src/icart_mini_driver_node.cpp

void MyRobo::read(ros::Time time, ros::Duration period)
{
    if (cmd_[0] || cmd_[1])
    	ROS_INFO_STREAM("Commands for joints: " << cmd_[0] << ", " << -cmd_[1]);
    	
    int ret = YP_wheel_vel(cmd_[0], -cmd_[1]);
}

void MyRobo::write(ros::Time time, ros::Duration period)
{
    double yp_vel[2];
    yp_vel[0] = 0;
    yp_vel[1] = 0;
    YP_get_wheel_vel(&yp_vel[0], &yp_vel[1]);
    yp_vel[1] = -yp_vel[1];
    
    if (yp_vel[0] || yp_vel[1])
        ROS_INFO_STREAM("YPSpur vel: " << yp_vel[0] << ", " << -yp_vel[1]);

    for (unsigned int i = 0; i < 2; ++i)
    {
        pos_[i] += yp_vel[i] * getPeriod().toSec();
        vel_[i] = yp_vel[i];
    }
}

