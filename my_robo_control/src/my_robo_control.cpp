#include <ros/ros.h>
#include <ros/package.h>
#include <angles/angles.h>
#include <my_robo_control/my_robo_hw.h>
#include <controller_manager/controller_manager.h>
#include <iostream> // for debug
#include <ypspur.h>
#include <string>
#include <std_msgs/String.h>

ros::Publisher pub;

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
    for (int i = 0; i < 4; i++)
        pos_[i] = vel_[i] = eff_[i] = cmd_[i] = 0.0;

    hardware_interface::JointStateHandle state_handle_1("right_wheel_joint", &pos_[0], &vel_[0], &eff_[0]);
    hardware_interface::JointStateHandle state_handle_2("left_wheel_joint",  &pos_[1], &vel_[1], &eff_[1]);
    hardware_interface::JointStateHandle state_handle_3("head_joint", &pos_[2], &vel_[2], &eff_[2]);
    hardware_interface::JointStateHandle state_handle_4("neck_joint",  &pos_[3], &vel_[3], &eff_[3]);
    
    jnt_state_interface_.registerHandle(state_handle_1);
    jnt_state_interface_.registerHandle(state_handle_2);
    jnt_state_interface_.registerHandle(state_handle_3);
    jnt_state_interface_.registerHandle(state_handle_4);

    registerInterface(&jnt_state_interface_);

    hardware_interface::JointHandle vel_handle_1(jnt_state_interface_.getHandle("right_wheel_joint"), &cmd_[0]);
    hardware_interface::JointHandle vel_handle_2(jnt_state_interface_.getHandle("left_wheel_joint"),  &cmd_[1]);
    hardware_interface::JointHandle eff_handle_1(jnt_state_interface_.getHandle("head_joint"), &cmd_[2]);
    hardware_interface::JointHandle eff_handle_2(jnt_state_interface_.getHandle("neck_joint"), &cmd_[3]);
    
    jnt_vel_interface_.registerHandle(vel_handle_1);
    jnt_vel_interface_.registerHandle(vel_handle_2);
    jnt_pos_interface_.registerHandle(eff_handle_1);
    jnt_pos_interface_.registerHandle(eff_handle_2);

    registerInterface(&jnt_vel_interface_); 
    registerInterface(&jnt_pos_interface_);
    
    if (open() < 0)
        ROS_ERROR("cannot open spur.\n");
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
    
    std::string hn[2] = { "CMH", "CMN" };
    std::string rl[2][2] = { { "U", "D" }, { "R", "L" } };
    
    for (int i = 2; i < 3; i++)
        if (cmd_[i])
        {
            // cmdは絶対角度で示された目標値(radian)
            // この値から何度回転するべきかを求める。つまりdiffラジアンだけ回転させる。
            double diff = cmd_[i] - pos_[i];
      
            if (int(diff * 180.0 / M_PI) == 0)
                continue;
 
            int angle = cmd_[i] * 180.0 / M_PI;

            std_msgs::String msg;
            std::string dir;
            
            if (angle < 0)
            {
                angle = -angle;
                dir = rl[i-2][1];
            }
            else
            {
                dir = rl[i-2][0];
            }
            
            
            int a = angle;
            std::string out = hn[i-2] + dir + std::to_string(angle);    
            msg.data = hn[i-2] + dir + std::to_string(a);    
            std::cout << out << std::endl;
            pub.publish(msg);
            
            // 指令値を送ったらすぐに首が傾いたことにする。
            pos_[i] += diff;
        }
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
    
    // writeは頭の角度を教えなければならないがどうしようか。
    // マイコンから角度を取得することもできるみたいだが、readで指令値を送ってから何秒立ったかで角度を知ることもできる。
    // Serviceでマイコンから角度をもらってもいいんですけどね
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "my_robo_control");
    ros::NodeHandle nh;
    pub = nh.advertise<std_msgs::String>("robotics_cmd", 10);
    MyRobo myrobo;
    controller_manager::ControllerManager cm(&myrobo, nh);

    ros::Rate rate(1.0 / myrobo.getPeriod().toSec());
    ros::AsyncSpinner spinner(1);
    spinner.start();

    while (ros::ok())
    {
        ros::Time now = myrobo.getTime();
        ros::Duration dt = myrobo.getPeriod();

        if(YP_get_error_state() == 0)
        {
            myrobo.read(now, dt);
            myrobo.write(now, dt);
        }
        else
        {
            ROS_WARN("Disconnected T-frog driver");
            myrobo.stop();
            
            while (myrobo.open() < 0)
            {
                ROS_WARN("try to connect T-frog driver");
                ros::Duration(1).sleep();
            }
            
            ROS_INFO("connected T-frog driver");
        }

        cm.update(now, dt);
        rate.sleep();
    }
    
    spinner.stop();
    myrobo.stop();
    return 0;
}

