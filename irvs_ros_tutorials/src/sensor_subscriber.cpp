// ROSメインヘッダーファイル
// ROSプログラミングを行う際に必要となるROSファイルのインクルードを行う。
// 後述するROS_INFO関数などを使用できるようになる。
#include "ros/ros.h"
// msgTutorialメッセージファイルのヘッダー
// CMakelists.txtでビルド後に自動的に生成されるように設定した
// メッセージファイルのヘッダーをインクルードする。
#include "irvs_ros_tutorials/msgTutorial.h"
#include "std_msgs/String.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Int8MultiArray.h"

ros::Publisher twist_pub, android_pub, neck_angle_pub;

//メッセージを受信したときに動作するコールバック関数を定義
// irvs_ros_tutorialsパッケージのmsgTutorialメッセージを受信する
//3Dセンサ

int before_wave_right2 = 0;
int before_wave_left2 = 0;
bool remain_battery_is_low = false;

void msgCallback(const std_msgs::String::ConstPtr& msg)
{
	int limit = 50;
	std_msgs::Int8 Int8;
	char wave_right[4];
	char wave_left[4];
	int wave_right2;
	int wave_left2;
	
	wave_right[3] = '\0';
	wave_left[3] = '\0';

	int i = 0;
	char test[30];
	strcpy(test,msg->data.c_str());

    //受信したメッセージを表示する。
    //ROS_INFO("recieve msg_ddd: %s", msg->data.c_str());
    //ROS_INFO("recieve msg_ss: %s", test);
	
	if(test[0] == 'S')
	{
		//printf("success\n");
		for (i = 0; i < 3 ; i++)
		{
			wave_right[i] = test[i+1]; 
		}
		for (i = 0; i < 3 ; i++)
		{
			wave_left[i] = test[i+4];
		}
		//printf("test = %s\n",wave_right);
		//printf("test = %s\n",wave_left);

		wave_right2 = atoi(wave_right);
		wave_left2 = atoi(wave_left);
		//printf("test = %d\n",wave_right2);
		//printf("test = %d\n",wave_left2);

		if(before_wave_right2 > wave_right2 + limit || before_wave_left2 > wave_left2 + limit ||
		   before_wave_right2 < wave_right2 - limit || before_wave_left2 < wave_left2 - limit)
		{
			//printf("over\n");		
		}

		else
		{
			if(before_wave_right2 > wave_right2 + 1 || before_wave_left2 > wave_left2 + 1)
			{
				printf("stop_wave\n");
				Int8.data = 6;
				twist_pub.publish(Int8);		
			}
			//printf("before_right = %d\n",before_wave_right2);
			//printf("before_left = %d\n",before_wave_left2);
		}
		
		before_wave_right2 = wave_right2;
		before_wave_left2 = wave_left2;
		
		if (test[7] == '1')
		{
			printf("stop\n");		
		}
		
		if (test[8] == '1')
		{
			printf("shutdown\n");
			
			// 権限がないとshutdownできないぞ・・・
			system("sudo poweroff");	
		}

    	//ROS_INFO("remain battery: [%c]", test[9]);
    
		// バッテリー残量に応じて目的地をpublishする。
		if (test[9] == 'H' && remain_battery_is_low)
		{
			ROS_INFO("remain battery is high! return to wait point.");
			remain_battery_is_low = false;
		    std_msgs::Int8 table;
		    table.data = 0;
			android_pub.publish(table);			
		}

		else if (test[9] == 'M')
		{
		}

		else if (test[9] == 'L' && !remain_battery_is_low)
		{
			ROS_INFO("remain battery is a little! go to save point.");
			remain_battery_is_low = true;
		    std_msgs::Int8 table;
		    table.data = 7;
			android_pub.publish(table);			
		}
	}
	else
	{
		//printf("not\n");
	}
}

//購読者ノードのメイン関数
int main(int argc, char **argv)
{
    ros::init(argc, argv, "sensor_subscriber");
    ros::NodeHandle nh;
    ros::Subscriber ros_tutorial_sub = nh.subscribe("/robotics_st",100, msgCallback);
    twist_pub = nh.advertise<std_msgs::Int8>("/emergency_call", 1);
    android_pub = nh.advertise<std_msgs::Int8>("/android", 1);
	neck_angle_pub = nh.advertise<std_msgs::Int8MultiArray>("/neck_andle", 1);
    ros::spin();
    return 0;
}
