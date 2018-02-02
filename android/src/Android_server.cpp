#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "ros/ros.h"
#include "std_msgs/Int8.h"

#include <thread>

#define MAXPENDING 5    /* Maximum outstanding connection requests */

int clntSock; 
int recv_start = 0;
ros::Publisher twist_pub;

void DieWithError(const char *errorMessage)  /* Error handling function */
{
	puts(errorMessage);
	exit(0);
}

void func(int func_argc,unsigned short echoServPort){
	printf("func\n");
	int sock;
	unsigned int echoStringLen;
	char *echoString;
	std_msgs::Int8 Int8;
	char test_recv[4] = {0,};

    int servSock;                    /* Socket descriptor for server */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
        DieWithError("socket() failed");
	}
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
	{
        DieWithError("bind() failed");
	}

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
	{
        DieWithError("listen() failed");
	}

	while (true) 
	{
		if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
		{
		    DieWithError("accept() failed");
		}

		printf("echoServPort=%d\n",echoServPort);
		printf("accept clntSock = %d\n",clntSock);

		while (1)
		{
			printf("test1\n");
			if(recv(clntSock,test_recv,4,0) <= 0 )
			{
				printf("accept\n");
				close(clntSock);
				break;
			}	
			printf("recv clntSock = %d\n",clntSock);
			printf("test_recv2: %d\n", test_recv[3]);

			Int8.data = test_recv[3];
			twist_pub.publish(Int8);
			sleep(1);	
		}
		
		// ソケットが閉じられたら0にする
		clntSock = 0;
	}
}

void msgCallback(const std_msgs :: Int8 :: ConstPtr & msg)
{
	ROS_INFO("recieve msg = %d",msg->data);
	int send_msg = (int)msg->data;

	// ソケットが閉じているときは送らない
	if (clntSock <= 0)
	{
		printf("clntSock not\n");
	}
	else
	{
		if (send(clntSock, &send_msg, 4, 0) < 0)
			DieWithError( "send() error" );
	}
	
	printf("send clntSock = %d\n",clntSock);
	printf("subscribe end\n");
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "android_communication");
    ros::NodeHandle nh;
    twist_pub = nh.advertise<std_msgs::Int8>("/android", 1);

	if (argc != 2)    
	{
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}

	unsigned short Port = atoi(argv[1]);
    printf("Port=%d\n",Port);
    twist_pub = nh.advertise<std_msgs::Int8>("/android", 1);
	std::thread th(func, argc, Port);
	ros::Subscriber ros_tutorial_sub = nh.subscribe("/android_communication",1, msgCallback);    
    ros::Rate rate(1);
    while (ros::ok()) 
    {
		rate.sleep();
		ros::spinOnce();
    }
    /* NOT REACHED */
}
