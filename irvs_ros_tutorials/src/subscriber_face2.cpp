#include "ros/ros.h"
#include "std_msgs/Int8.h"
#include "std_msgs/String.h"
#include "pimouse_vision_control/test1.h"

#include <deque>
#include <mutex>
#include <thread>
#include <string>

int x_point[5], y_point[5], x_length[5], y_length[5];
int sample_count = 0;
float angle = 0;
int neck_angle, head_angle;
std::deque<std::string> neck_rotate_msgs;
std::mutex mutex;

// 顔位置の補正処理
// 過去5回の顔位置の平均を取る
void msgCallback(const pimouse_vision_control::test1::ConstPtr& msg)
{	
	const int sample_times = 5;
	const int difference = 15;

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

// 顔のある場所に首を動かすメッセージを生成する。
std::string neckRotateMsgForFaceTracking()
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
	
	std::string msg = "CMN";

    if (rotation_now < 0) 
    {
	    rotation_now = -rotation_now;
	    msg += "L" + std::to_string((int)rotation_now);
    }
    else if (rotation_now >= 0) 
    {
	    msg += "R" + std::to_string((int)rotation_now);		
	}
	
	return msg;
}

// お辞儀
void msgCallback2(const std_msgs::Int8::ConstPtr& msg)
{
	std::unique_lock<std::mutex> lk(mutex);
	neck_rotate_msgs.push_back(neckRotateMsgForFaceTracking());
	neck_rotate_msgs.push_back("CMHD15");
	neck_rotate_msgs.push_back("CMHU0");
	neck_rotate_msgs.push_back("CMNL0");
	neck_rotate_msgs.push_back("CMHD20");
}

// 顔追跡
void msgCallback4(const std_msgs::Int8::ConstPtr& msg)
{
	std::unique_lock<std::mutex> lk(mutex);
	neck_rotate_msgs.push_back(neckRotateMsgForFaceTracking());
}

// 首を回転させるために常に動いているworkerスレッド
void neckRotateWorker(ros::Publisher* neck_pub)
{   
	std_msgs::String rot;
	ros::Rate rate(1);
	
	while (ros::ok())
	{
		rate.sleep();
		
		// ロック
		std::unique_lock<std::mutex> lk(mutex);
		
		while (neck_rotate_msgs.size())
		{
			rot.data = neck_rotate_msgs.front();
			neck_rotate_msgs.pop_front();
			neck_pub->publish(rot);
			ROS_INFO("rotate msgs: %s", rot.data.c_str());
			lk.unlock();
			ros::Duration(1).sleep();
			lk.lock();
		}
	}
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
	
	//if (i++ % 500 == 0)
	//ROS_INFO("head_angle = %d, neck_angle = %d\n", head_angle, neck_angle);
}

int main(int argc, char **argv)
{
	ros::init(argc,argv,"test1011_subscliber_node");
	ros::NodeHandle nh;
	ros::Subscriber sub1 = nh.subscribe("/test1",1, msgCallback);
	ros::Subscriber sub2 = nh.subscribe("/ojigi",1, msgCallback2);
	ros::Subscriber sub3 = nh.subscribe("/robotics_st",100, msgCallback3);
	ros::Subscriber sub4 = nh.subscribe("/track_face", 1, msgCallback4);
	ros::Publisher neck_pub = nh.advertise<std_msgs::String>("/robotics_cmd", 1);
    std::thread th(neckRotateWorker, &neck_pub);
	ros::spin();
	return 0;
}
