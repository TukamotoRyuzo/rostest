#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Int8.h>

// geometry_msgs::Poseのコンストラクタ（がないので作った）
geometry_msgs::Pose makePose(float px, float py, float pz, float qx, float qy, float qz, float qw)
{
    geometry_msgs::Pose p;
    p.position.x = px;
    p.position.y = pz;
    p.position.z = pz;
    p.orientation.x = qx;
    p.orientation.y = qy;
    p.orientation.z = qz;
    p.orientation.w = qw;
    return p;
}

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

MoveBaseClient* gac;
ros::Publisher* gpub;

// msgをsubscribeした時に呼びだされるコールバック
void tableNumberCallback(const std_msgs::Int8::ConstPtr& msg)
{
    ROS_INFO("I heard: [%d]", msg->data);
    
    if (msg->data < 0 || msg->data >= 10)
    {
        ROS_INFO("subscribed number is out of range! (0 <= data < 10)");
        return;
    }
          
    geometry_msgs::Pose p[10] = 
    {
        makePose(1, 0, 0, 0, 0, 1, 1),
        makePose(2, 0, 0, 0, 0, 1, 1),
        makePose(3, 0, 0, 0, 0, 1, 1),
        makePose(4, 0, 0, 0, 0, 1, 1),
        makePose(5, 0, 0, 0, 0, 1, 1),
        makePose(0, 0, 1, 0, 0, 1, 1),
        makePose(0, 0, 2, 0, 0, 1, 1),
        makePose(0, 0, 3, 0, 0, 1, 1),
        makePose(0, 0, 4, 0, 0, 1, 1),
        makePose(0, 0, 5, 0, 0, 1, 1),
    };
    
    move_base_msgs::MoveBaseGoal goal;

    // we'll send a goal to the robot
    goal.target_pose.header.frame_id = "base_footprint";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose = p[msg->data];

    ROS_INFO("Sending goal");
    gac->sendGoal(goal);
    gac->waitForResult();

    if (gac->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    {
        // 成功したら移動完了メッセージを送る。
        ROS_INFO("Hooray, the base moved 1 meter forward");
        std_msgs::Int8 m;
        m.data = 0;
        gpub->publish(m);
    }
    else
    {
        ROS_INFO("The base failed to move for some reason");
        std_msgs::Int8 m;
        m.data = -1;
        gpub->publish(m);
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "simple_goal_generator");

    // tell the action client that we want to spin a thread by defaulta
    // action clientに対してデフォルトでスレッドをspinさせたいことを伝える
    MoveBaseClient ac("move_base", true);

    ROS_INFO("[SimpleGoalGenerator]: wait for action server to come up.");
    
    // wait for the action server to come up
    // action serverが起動するのを待つ
    while (!ac.waitForServer(ros::Duration(5.0)))
        ROS_INFO("Waiting for the move_base action server to come up");
   
    ROS_INFO("[SimpleGoalGenerator]: server come up! wait for table number.");
    
    gac = &ac;
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<std_msgs::Int8>("navigate_result", 10);
    ros::Subscriber sub = n.subscribe("table_number", 10, tableNumberCallback);    
    gpub = &pub;
    ros::spin();
    
    return 0;
}
