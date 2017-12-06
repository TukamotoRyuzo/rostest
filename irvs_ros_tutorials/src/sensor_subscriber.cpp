// ROSメインヘッダーファイル
// ROSプログラミングを行う際に必要となるROSファイルのインクルードを行う。
//後述するROS_INFO関数などを使用できるようになる。
#include "ros/ros.h"
// msgTutorialメッセージファイルのヘッダー
// CMakelists.txtでビルド後に自動的に生成されるように設定した
//メッセージファ
//イルのヘッダーをインクルードする。
#include "irvs_ros_tutorials/msgTutorial.h"
#include "std_msgs/String.h"
#include "std_msgs/Int16.h"




ros::Publisher twist_pub;
//メッセージを受信したときに動作するコールバック関数を定義
// irvs_ros_tutorialsパッケージのmsgTutorialメッセージを受信する
//3Dセンサ

int before_wave_right2 = 0;
int before_wave_left2 = 0;

void msgCallback(const std_msgs::String::ConstPtr& msg)//ここ変更
{
	
int distance;

	int limit = 5;



	int count =0;
	int danger_distance = 3; //ここ変更
	int signal = 1; //ここ変更
	std_msgs::Int16 Int16;
	char wave_right[4];
	char wave_left[4];
	int wave_right2;
	int wave_left2;

	
	
	wave_right[3] = '\0';
	wave_left[3] = '\0';

	int i = 0;
	
	char test[30];
	strcpy(test,msg->data.c_str());
	std::stringstream ss;
	ss << msg->data.c_str() << count;

//受信したメッセージを表示する。
ROS_INFO("recieve msg_ddd: %s", msg->data.c_str());
ROS_INFO("recieve msg_ss: %s", test);
	
	if(test[0] == 'S')
	{
		printf("success\n");
		for (i = 0; i < 3 ; i++)
		{
			wave_right[i] = test[i+1]; 
		}
		for (i = 0; i < 3 ; i++)
		{
			wave_left[i] = test[i+4];
		}
		printf("test = %s\n",wave_right);
		printf("test = %s\n",wave_left);

		wave_right2 = atoi(wave_right);
		wave_left2 = atoi(wave_left);
		printf("test = %d\n",wave_right2);
		printf("test = %d\n",wave_left2);

		if(before_wave_right2 > wave_right2 + limit || before_wave_left2 > wave_left2 + limit ||
		   before_wave_right2 < wave_right2 - limit || before_wave_left2 < wave_left2 - limit)
		{
			printf("over\n");		

		}

		else
		{
			if(before_wave_right2 > wave_right2 + 1 || before_wave_left2 > wave_left2 + 1)
			{
				printf("stop_wave\n");
				Int16.data = 6;
				twist_pub.publish(Int16);		
			}
			printf("before_right = %d\n",before_wave_right2);
			printf("before_left = %d\n",before_wave_left2);
			before_wave_right2 = wave_right2;
			before_wave_left2 = wave_left2;

		}
		


		if (test[7] == '1')
		{
			printf("stop\n");
			Int16.data = 1;
			twist_pub.publish(Int16);
			
		}
		if (test[8] == '1')
		{
			printf("shutdown\n");
			Int16.data = 2;
			twist_pub.publish(Int16);
			
		}

		if (test[9] == 'H')
		{
			printf("high\n");
			Int16.data = 3;
			twist_pub.publish(Int16);
			
		}

		else if (test[9] == 'M')
		{
			printf("middle\n");
			Int16.data = 4;
			twist_pub.publish(Int16);
			
		}

		else if (test[9] == 'L')
		{
			printf("low\n");
			Int16.data = 5;
			twist_pub.publish(Int16);
			
		}
	}
	else
	{
		printf("not\n");
	}
}



//購読者ノードのメイン関数
int main(int argc, char **argv)
{
	printf("init\n");
//ノード名の初期化
ros::init(argc, argv, "sensor_subscriber");
// ROSシステムとの通信のためのノードのハンドルを宣言
ros::NodeHandle nh;
//購読者ノードの宣言
// irvs_ros_tutorialsパッケージのmsgTutorialメッセージファイルを
//利用した購読者ros_tutorial_subを宣言する。
//トピック名をros_tutorial_msgとし、購読者キュー( queue )の
//サイズを100に設定する。
//購読者キューには、配信者から送信されてくるメッセージが蓄積される。

//ros::Subscriber ros_tutorial_sub = nh.subscribe("ros_tutorial_msg",
//100, msgCallback);

	printf("init2\n");

ros::Subscriber ros_tutorial_sub = nh.subscribe("robotic_cmd",100, msgCallback);

printf("init3\n");

twist_pub = nh.advertise<std_msgs::Int16>("/emergency_call", 1);

//メッセージが受信されるまで待機し、受信が行われた場合、
//コールバック関数を実行する。
ros::spin();
return 0;
}
