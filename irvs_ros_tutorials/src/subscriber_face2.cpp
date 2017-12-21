#include "ros/ros.h"


#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>

#include "std_msgs/String.h"
#include "pimouse_vision_control/test1.h"
#include "std_msgs/Int16.h"
#include "std_msgs/String.h"

int first = 0;
int x_point_sum,y_point_sum,x_length_sum,y_length_sum = 0;
int  x_point[5],y_point[5],x_length[5],y_length[5];
int i = 0;
float angle = 0;
float before_angle = 0;

ros::Publisher twist_pub;
void msgCallback(const pimouse_vision_control :: test1 :: ConstPtr & msg)
{	
	int test_times = 5;
	int difference =15;
	int count = 0;
	
	int camera_angle = 60 ;
	int side_pixel = 640;
	
	float midstream;
	
	std_msgs::String String;
	char restart[256] = "CMHU0";

//	std_msgs::Float32 Float32;/*ここを変更*/
	
	if (first ==0)
	{
		for(i =0;i<5;i++)
		{
			x_point[i] = 0;
			y_point[i] = 0;
			x_length[i] = 0;
			y_length[i] = 0;
		}
		String.data = restart;/*ここを変更*/
//		twist_pub.publish(String);
		first = 1;
		i = 0;
		printf("first\n");
	}
			
	ROS_INFO("a = [%d]", msg->a);
	ROS_INFO("b = [%d]", msg->b);
	ROS_INFO("c = [%d]", msg->c);
	ROS_INFO("d = [%d]", msg->d);

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
		   y_length_sum > y_length[0] - difference && y_length_sum < y_length[0] + difference &&
		   x_length_sum > 70 )
		{
			printf("success\n\n");
			midstream = x_point_sum + (y_length_sum / 2);
//			printf("midstream1 = %f\n",midstream);
			midstream = midstream / side_pixel * 2;
//			printf("midstream2 = %f\n\n",midstream);
			midstream = midstream - 1;
//			printf("midstream3 = %f\n\n",midstream);
			angle = midstream * camera_angle / 2;
			printf("before_angle = %f\n\n",before_angle);
			printf("face_angle = %f\n\n",angle);
			angle = before_angle + angle;
			printf("new_angle = %f\n\n",angle);
			    
			
			
		
			
		//	String.data = signal;/*ここを変更*/
		//	twist_pub.publish(String);/*ここを変更*/
			
		//	sleep(5);


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
	
	

}

void msgCallback2(const std_msgs :: Int16 :: ConstPtr & msg)
{	
	char msgs[256] = {'\0'}; 
	char signal[256] = "CMN"; 
	char L[2] = {'L'}; 
	char R[2] = {'R'}; 
	char down[256] = "CMHD15";
	char up[256] = "CMHD0";
	char front[256] = "CMNL0";
	std_msgs::String String;

    
	if(msg->data == 1)
	{
	    if(angle < -30 || angle > 30)
	    {
	        printf("angle is over\n");
	        angle = before_angle;
	    }
	    else
	    {
		    //before_angle = angle;
		    before_angle = 0;
		}
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
    
		String.data = signal;
		twist_pub.publish(String);
		sleep(2);

		String.data = down;
		twist_pub.publish(String);
		sleep(2);

		String.data = up;
		twist_pub.publish(String);
	    sleep(1);
	    
	    String.data = front;
		twist_pub.publish(String);
	    sleep(2);
	    
    }
}

int main(int argc, char **argv)
{
	ros::init(argc,argv,"test1011_subscliber_node");

	ros::NodeHandle nh;


	ros::Subscriber ros_tutorial_sub = nh.subscribe("/test1",1, msgCallback);
	ros::Subscriber ros_tutorial_sub2 = nh.subscribe("/android_communication",1, msgCallback2);
//	twist_pub = nh.advertise<std_msgs::Float32>("/angle", 1);/*ここを変更*/
    twist_pub = nh.advertise<std_msgs::String>("/my_robo/robotics_cmd", 1);/*ここを変更*/
	//std_msgs::Float32 Float32;


	ros::Rate loop_rate(1);

ros::spin();
	return 0;
}
