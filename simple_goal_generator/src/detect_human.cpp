#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Int8.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

MoveBaseClient* gac;

// msgをsubscribeした時に呼びだされるコールバック
void evasionCallback(const std_msgs::Int8::ConstPtr& msg)
{
    ROS_INFO("detect human! preempting...");
    gac->cancelAllGoals();
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "detect_human");
    MoveBaseClient ac("move_base", true);

    ROS_INFO("[EvasionHuman]: wait for action server to come up.");
    
    // wait for the action server to come up
    // action serverが起動するのを待つ
    while (!ac.waitForServer(ros::Duration(5.0)))
        ROS_INFO("Waiting for the move_base action server to come up");
   
    ROS_INFO("[EvasionHuman]: server come up! wait for signal.");
    
    gac = &ac;
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("detect_human", 10, evasionCallback);
    ros::spin();
    
    return 0;
}
