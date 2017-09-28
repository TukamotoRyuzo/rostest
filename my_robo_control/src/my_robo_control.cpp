#include <ros/ros.h>
#include <controller_manager/controller_manager.h>
#include <my_robo_control/my_robo_hw.h>

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "my_robo_control");
    ros::NodeHandle nh;

    MyRobo myrobo;
    controller_manager::ControllerManager cm(&myrobo, nh);

    ros::Rate rate(1.0 / myrobo.getPeriod().toSec());
    ros::AsyncSpinner spinner(1);
    spinner.start();

    while(ros::ok())
    {
        ros::Time now = myrobo.getTime();
        ros::Duration dt = myrobo.getPeriod();

        myrobo.read(now, dt);
        cm.update(now, dt);
        myrobo.write(now, dt);
        rate.sleep();
    }
    
    spinner.stop();

    return 0;
}

