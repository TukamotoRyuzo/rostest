#include <ros/ros.h>
#include <ros/package.h>
#include <angles/angles.h>
#include <my_robo_control/my_robo_hw.h>
#include <iostream> // for debug
#include <math.h>

MyRobo::MyRobo()
{
    // connect and register the joint state interface
    hardware_interface::JointStateHandle state_handle_1("right_wheel_joint", &pos_[0], &vel_[0], &eff_[0]);
    jnt_state_interface.registerHandle(state_handle_1);
    hardware_interface::JointStateHandle state_handle_2("left_wheel_joint", &pos_[1], &vel_[1], &eff_[1]);
    jnt_state_interface.registerHandle(state_handle_2);

    registerInterface(&jnt_state_interface);

    // connect and register the joint velocity interface
    hardware_interface::JointHandle vel_handle_1(velocity_joint_interface.getHandle("right_wheel_joint"), &cmd_[0]);
    velocity_joint_interface.registerHandle(vel_handle_1);
    hardware_interface::JointHandle vel_handle_2(velocity_joint_interface.getHandle("left_wheel_joint"), &cmd_[1]);
    velocity_joint_interface.registerHandle(vel_handle_2);
    
    registerInterface(&velocity_joint_interface);
}

// readとwriteをypspur_rosへのトピックのpublish, subscribeに変更すれば動くか？？？
void MyRobo::read(ros::Time time, ros::Duration period)
{
}

void MyRobo::write(ros::Time time, ros::Duration period)
{
    //pos_[0] = cmd_[0];
}

