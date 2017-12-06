#include "ros/ros.h"


#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>

#include "std_msgs/String.h"
#include "pimouse_vision_control/test1.h"
#include "std_msgs/Float32.h"

int first = 0;
int x_point_sum,y_point_sum,x_length_sum,y_length_sum = 0;
int  x_point[5],y_point[5],x_length[5],y_length[5];
int i = 0;


ros::Publisher twist_pub;
void msgCallback(const pimouse_vision_control :: test1 :: ConstPtr & msg)
{	
	int test_times = 5;
	int difference =15;
	int count = 0;
	float angle;
	int camera_angle = 60 ;
	int side_pixel = 640;
	float midstream;
	char L[2] = {'L'}; 
	char R[2] = {'R'}; 
	char msgs[256] = {'\0'}; 
	char signal[256] = "CMN"; 
	std_msgs::Float32 Float32;/*ここを変更*/

	if (first ==0)
	{
		for(i =0;i<5;i++)
		{
			x_point[i] = 0;
			y_point[i] = 0;
			x_length[i] = 0;
			y_length[i] = 0;
		}
		first = 1;
		i = 0;
	}
/*			
	ROS_INFO("a = [%d]", msg->a);
	ROS_INFO("b = [%d]", msg->b);
	ROS_INFO("c = [%d]", msg->c);
	ROS_INFO("d = [%d]", msg->d);
*/
	x_point[i] = msg->a;
	y_point[i] = msg->b;
	x_length[i] = msg->c;
	y_length[i] = msg->d;
	printf("i = %d\n",i);

	if (i<test_times-1)
	{
		i=i+1;
		
	}
	else
	{
		i=0;
		for(count = 0;count < test_times;count++)
		{
			x_point_sum = x_point_sum + x_point[count];
			y_point_sum = y_point_sum + y_point[count];
			x_length_sum = x_length_sum + x_length[count];
			y_length_sum = y_length_sum + y_length[count];
		}
/*
		printf("x_point_sum = %d\n",x_point_sum);
		printf("y_point_sum = %d\n",y_point_sum);
		printf("x_point_length = %d\n",x_length_sum);
		printf("y_point_length = %d\n\n\n",y_length_sum);
*/
		x_point_sum = x_point_sum / test_times;
		y_point_sum = y_point_sum / test_times;
		x_length_sum = x_length_sum / test_times;
		y_length_sum = y_length_sum / test_times;

		printf("x_point_sum = %d\n",x_point_sum);
		printf("y_point_sum = %d\n",y_point_sum);
		printf("x_point_length = %d\n",x_length_sum);
		printf("y_point_length = %d\n\n\n",y_length_sum);
	
		if(x_point_sum > x_point[0] - difference && x_point_sum < x_point[0] + difference &&
		   y_point_sum > y_point[0] - difference && y_point_sum < y_point[0] + difference &&
		   x_length_sum > x_length[0] - difference && x_length_sum < x_length[0] + difference &&
		   y_length_sum > y_length[0] - difference && y_length_sum < y_length[0] + difference)
		{
			printf("success\n\n");
			midstream = x_point_sum + (y_length_sum / 2);
//			printf("midstream1 = %f\n",midstream);
			midstream = midstream / side_pixel * 2;
//			printf("midstream2 = %f\n\n",midstream);
			midstream = midstream - 1;
//			printf("midstream3 = %f\n\n",midstream);
			angle = midstream * camera_angle / 2;
			printf("angle = %f\n\n",angle);

			if(angle < 0) 
			{
				angle = angle * -1;
				sprintf(msgs, "%f", angle);
				strcat(signal,L);
				strcat(signal,msgs);
				printf("angle_mozi_left = %s\n",signal);
				
			}
			else if(angle > 0) 
			{
				sprintf(msgs, "%f", angle);
				strcat(signal,R);
				strcat(signal,msgs);
				printf("angle_mozi_right = %s\n",signal);
				
			}
		
			


			Float32.data = angle;/*ここを変更*/
			twist_pub.publish(Float32);/*ここを変更*/

		}
		else
		{
			printf("out\n\n");
		}

		x_point_sum = 0;
		y_point_sum = 0;
		x_length_sum = 0;
		y_length_sum = 0;
		
	}
	
	//sleep(1);

}



int main(int argc, char **argv)
{
	ros::init(argc,argv,"test1011_subscliber_node");

	ros::NodeHandle nh;


	ros::Subscriber ros_tutorial_sub = nh.subscribe("/test1",1, msgCallback);
	twist_pub = nh.advertise<std_msgs::Float32>("/angle", 1);/*ここを変更*/

	//std_msgs::Float32 Float32;


	ros::Rate loop_rate(1);

ros::spin();
	return 0;
}
