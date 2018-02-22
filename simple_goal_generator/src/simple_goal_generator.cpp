#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>
#include <std_srvs/Empty.h>
#include <tf/tf.h>

#include <tf/transform_listener.h>
#include <costmap_2d/costmap_2d_ros.h>
#include <clear_costmap_recovery/clear_costmap_recovery.h>
// geometry_msgs::Poseのコンストラクタ（がないので作った）
geometry_msgs::Pose makePose(float px, float py, float yaw)
{
    geometry_msgs::Pose p;
    p.position.x = px;
    p.position.y = py;
    p.position.z = 0.0;
    p.orientation = tf::createQuaternionMsgFromYaw(yaw);
    return p;
}

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

MoveBaseClient* gac;
ros::Publisher* gpub;
ros::Publisher* gpub_ojigi;
ros::Publisher* gpub_neck_down;
ros::ServiceClient g_clear_costmaps_service;

// msgをsubscribeした時に呼びだされるコールバック
void tableNumberCallback(const std_msgs::Int8::ConstPtr& msg)
{
    ROS_INFO("I heard: [%d]", msg->data);
    
    if (msg->data < 0 || msg->data >= 7)
    {
        ROS_INFO("subscribed number is out of range! (0 <= data < 7)");
        return;
    }
    
    /*
    // b205用
    geometry_msgs::Pose p[6] = 
    {
        makePose(-3.064, -0.005, -0.384),// 待機場所
        makePose(-2.535, 1.542, 1.187),  // 受付場所
        makePose(-1.072, -0.531, 1.189), // table0
        makePose(0.537, -0.907, 1.155), // table1
        makePose(1.844, -1.611, 1.134 ), // table2
        makePose(3.501, -2.150, 1.151), // table3
    };
    */
    
    // 食堂用
    geometry_msgs::Pose p[7] = 
    {
        makePose(-0.914, 1.475, 0.765),// 待機場所
        makePose(-3.263, 4.091, 2.321),  // 受付場所
        makePose(4.453, 0.381, 0.782), // table0
        makePose(4.126, 4.488, 0.785), // table1(counter)
        makePose(3.136, 1.794, 0.815), // table2
        makePose(1.515, 4.115, 0.748), // table3
        makePose(0.091, 5.587, 0.856), // table4
    };
    
    move_base_msgs::MoveBaseGoal goal;

    // we'll send a goal to the robot
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose = p[msg->data];
	
	ROS_INFO("Setting Goal: Frame:%s Position(%4f %4f %4f), Orientation(%4f %4f %4f %4f)", 
		goal.target_pose.header.frame_id.c_str(),
		goal.target_pose.pose.position.x,
		goal.target_pose.pose.position.y,
		goal.target_pose.pose.position.z,
		goal.target_pose.pose.orientation.x,
		goal.target_pose.pose.orientation.y,
		goal.target_pose.pose.orientation.z,
		goal.target_pose.pose.orientation.w);
		
	do {
		std_srvs::Empty empty_msg;
		g_clear_costmaps_service.call(empty_msg);

        //tf::TransformListener tf(ros::Duration(10));
		//costmap_2d::Costmap2DROS global_costmap("global_costmap", tf);
		//costmap_2d::Costmap2DROS local_costmap("local_costmap", tf);
		//clear_costmap_recovery::ClearCostmapRecovery ccr;
		//ccr.initialize("my_clear_costmap_recovery", &tf, &global_costmap, &local_costmap);
		//ccr.runBehavior();
		
	std_msgs::String neck_down;
	neck_down.data = "CMHD20";
	gpub_neck_down->publish(neck_down);
	gac->sendGoal(goal);
        // waitForResultしている間に他のサブスクライバが動けるか。
        // →動けない。プリエンプションさせるノードが別に必要。
        // →作った。
        gac->waitForResult();
        auto state = gac->getState();
        printf("state = %s", state.toString().c_str());
        
        // 成功したら終了
        if (state == actionlib::SimpleClientGoalState::SUCCEEDED)
            break;

        // 処理目標が処理されている間、クライアント側のキャンセル要求により処理が中断された 
        // ここは人を検知した時の処理が入る。
        else if (state == actionlib::SimpleClientGoalState::PREEMPTED)
        {
            ROS_INFO("evasion start!");
            
            // 人回避中。回避が終わるまで待つ。
            // ここに人を回避するコードを書いてください。
            // 一番近い待機ポイントに移動するようなコードはどうか？
            
            ROS_INFO("evasion end!");
        }
        
        // なんらかの理由で失敗
        else
        {
            ROS_INFO("The base failed to move for some reason");
            std_msgs::Int8 m;
            m.data = -1;
            gpub->publish(m);
            return;
        }     
    } while (true);
    
    ROS_INFO("Hooray! Hi Hi Hi.");
    std_msgs::Int8 m;
    m.data = 0;
    gpub->publish(m);
    
    // 受付場所に行くのに成功したらお辞儀メッセージをpublishする。
    if (msg->data == 1)
        gpub_ojigi->publish(m);
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
    ros::Publisher pub = n.advertise<std_msgs::Int8>("android_communication", 10);
    ros::Publisher pub_ojigi = n.advertise<std_msgs::Int8>("ojigi", 10);
    ros::Subscriber sub = n.subscribe("android", 10, tableNumberCallback);
    ros::Publisher pub_neck_down = n.advertise<std_msgs::Int8>("robotics_cmd", 10);
    g_clear_costmaps_service = n.serviceClient<std_srvs::Empty>("move_base/clear_costmaps");
    gpub = &pub;
    gpub_ojigi = &pub_ojigi;
    gpub_neck_down = &pub_neck_down;
    ros::spin();
    
    return 0;
}
