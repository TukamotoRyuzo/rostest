#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int8.h>

ros::Publisher* gpub;

// バッテリー残量を受け取った時に呼びだされるコールバック
void remainBatteryCallback(const std_msgs::Float32::ConstPtr& msg)
{
    ROS_INFO("remain battery: [%f]", msg->data);
    
    // バッテリーが一定以下になっていればバッテリーの場所に移動するように通知する
    if (msg->data < 0.3)
    {
        ROS_INFO("remain battery is a little! go save point.");
        std_msgs::Int8 table;
        table.data = 9;
        gpub->publish(table);
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "monitor_battery");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<std_msgs::Int8>("table_number", 10);
    ros::Subscriber sub = n.subscribe("remain_battery", 10, remainBatteryCallback);    
    gpub = &pub;
    ROS_INFO("[moniter_battery_node]:wake up.");
    ros::spin();
    return 0;
}
