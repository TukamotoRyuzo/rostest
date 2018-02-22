#include <ros/ros.h>
#include <gazebo_msgs/SpawnModel.h>
#include <gazebo_msgs/DeleteModel.h>

// gazebo上に今存在しているロボットをdeleteしてspawnし直すだけのノード。
// .launchファイルからrosserviceをcallするための綺麗な方法が見つからなかったので作った。
int main(int argc, char** argv)
{
    ros::init(argc, argv, "robot_respawner");
    ros::NodeHandle n;
    
    // delete model
    ros::ServiceClient client = n.serviceClient<gazebo_msgs::DeleteModel>("gazebo/delete_model");
    gazebo_msgs::DeleteModel delete_model;
    delete_model.request.model_name = "my_robo";
    client.call(delete_model);
    //client = n.serviceClient<gazebo_msgs::SpawnModel>("gazebo/spawn_urdf_model");
    
    return 0;
}
