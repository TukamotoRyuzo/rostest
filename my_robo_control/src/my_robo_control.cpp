#include <ros/ros.h>
#include <controller_manager/controller_manager.h>
#include <my_robo_control/my_robo_hw.h>
#include <ypspur.h>

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
        }

        cm.update(now, dt);
        rate.sleep();
    }
    
    spinner.stop();
    myrobo.stop();
    
    return 0;
}

