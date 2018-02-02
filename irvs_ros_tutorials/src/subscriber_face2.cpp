#include "ros/ros.h"
#include "std_msgs/String.h"
#include "pimouse_vision_control/test1.h"
#include "std_msgs/Int8.h"
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>
#include <string>
int x_point[5], y_point[5], x_length[5], y_length[5];
int sample_count = 0;
float angle = 0;
int neck_angle, head_angle;

ros::Publisher twist_pub;

// 顔位置の補正処理
// 過去5回の顔位置の平均を取る
void msgCallback(const pimouse_vision_control::test1::ConstPtr& msg)
{	
	const int sample_times = 5;
	const int difference = 15;

	ROS_INFO("a = [%ld]", msg->a);
	ROS_INFO("b = [%ld]", msg->b);
	ROS_INFO("c = [%ld]", msg->c);
	ROS_INFO("d = [%ld]", msg->d);
	ROS_INFO("sample_count = %d\n", sample_count);
	
	x_point[sample_count] = msg->a;
	y_point[sample_count] = msg->b;
	x_length[sample_count] = msg->c;
	y_length[sample_count] = msg->d;

	if (++sample_count >= sample_times)
	{
		sample_count = 0;
		int x_point_sum = 0, y_point_sum = 0, x_length_sum = 0, y_length_sum = 0;
		
		// 平均を求める
		for (int count = 0; count < sample_times; count++)
		{
			x_point_sum = x_point_sum + x_point[count];
			y_point_sum = y_point_sum + y_point[count];
			x_length_sum = x_length_sum + x_length[count];
			y_length_sum = y_length_sum + y_length[count];
		}

		x_point_sum = x_point_sum / sample_times;
		y_point_sum = y_point_sum / sample_times;
		x_length_sum = x_length_sum / sample_times;
		y_length_sum = y_length_sum / sample_times;

		printf("x_point_sum = %d\n", x_point_sum);
		printf("y_point_sum = %d\n", y_point_sum);
		printf("x_point_length = %d\n", x_length_sum);
		printf("y_point_length = %d\n", y_length_sum);
		
		// 顔が大きく移動していないことを確認する処理
		if (x_point_sum > x_point[0] - difference && x_point_sum < x_point[0] + difference &&
		    y_point_sum > y_point[0] - difference && y_point_sum < y_point[0] + difference &&
		    x_length_sum > x_length[0] - difference && x_length_sum < x_length[0] + difference &&
		    y_length_sum > y_length[0] - difference && y_length_sum < y_length[0] + difference &&
		    x_length_sum > 40)
		{
			// カメラの視野角
			const float camera_angle = 60.0f;
	
			// カメラの横のピクセル数
			const float side_pixel = 320.0f;

			// 顔の中心点のx座標を出す
			float face_central_x = x_point_sum + (y_length_sum / 2.0f);

			// 顔の中心点のx座標を-1.0〜1.0にスケーリングする
			face_central_x = face_central_x / side_pixel * 2.0f - 1.0f;
			
			// 視野角が60の場合、-30〜30にする。
			angle = face_central_x * camera_angle / 2.0f;
		}

		x_point_sum = 0;
		y_point_sum = 0;
		x_length_sum = 0;
		y_length_sum = 0;	
	}
}

void msgCallback2(const std_msgs::Int8::ConstPtr& msg)
{
	// 首の横の最大角度
	const float max_angle = 30.0f;
	
	// 現在の首の角度からangleだけ回転させる。
	float rotation_now = neck_angle + angle;
	
	// clamp
	if (rotation_now < -max_angle)
		rotation_now = -max_angle;
		
	else if (rotation_now > max_angle)
		rotation_now = max_angle;
	
	
	std::string msgs = "CMN";
	char start[256] = "CMHD20";
	char down[256] = "CMHD15";
	char up[256] = "CMHD0";
	char up2[256] = "CMHU5";
	char front[256] = "CMNL0";
	std_msgs::String String;
			

    if (rotation_now < 0) 
    {
	    rotation_now = -rotation_now;
	    msgs += "L" + std::to_string((int)rotation_now);
    }
    else if (rotation_now >= 0) 
    {
	    msgs += "R" + std::to_string((int)rotation_now);		
	}
	
	printf("%s\n", msgs.c_str());
	String.data = msgs.c_str();
	twist_pub.publish(String);
//	ROS_INFO("seep");
//	ros::Duration(2).sleep();
//	ROS_INFO("wake");
/*
	printf("%s\n", down);
	String.data = down;
	twist_pub.publish(String);
	sleep(3);

	printf("%s\n", up2);
	String.data = up2;
	twist_pub.publish(String);
	sleep(2);

	printf("%s\n", front);
    String.data = front;
	twist_pub.publish(String);
    sleep(2);
 */
}

// 角度を更新し続ける
void msgCallback3(const std_msgs::String::ConstPtr& msg)
{	
	static int i = 0;
	const char* s = msg->data.c_str();

	if (s[0] == 'H')
		head_angle = s[1] == 'U' ? atoi(s + 2) : -atoi(s + 2);

	else if (s[0] == 'N')
		neck_angle = s[1] == 'L' ? atoi(s + 2) : -atoi(s + 2);
	
	if (i++ % 10 == 0)
	ROS_INFO("head_angle = %d, neck_angle = %d\n", head_angle, neck_angle);
}

int main(int argc, char **argv)
{
	ros::init(argc,argv,"test1011_subscliber_node");
	ros::NodeHandle nh;
	ros::Subscriber ros_tutorial_sub = nh.subscribe("/test1",1, msgCallback);
	ros::Subscriber ros_tutorial_sub2 = nh.subscribe("/ojigi",1, msgCallback2);
	ros::Subscriber ros_tutorial_sub3 = nh.subscribe("/robotics_st",100, msgCallback3);
    twist_pub = nh.advertise<std_msgs::String>("/robotics_cmd", 1);
	ros::spin();
	return 0;
}
