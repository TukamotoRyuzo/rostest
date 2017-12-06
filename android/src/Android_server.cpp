#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "ros/ros.h"
#include "std_msgs/Int16.h"

#include <thread>

#define MAXPENDING 5    /* Maximum outstanding connection requests */

	int clntSock; 
//	char test_recv[1] = {0,};

int recv_start = 0;

ros::Publisher twist_pub;

void DieWithError(char *errorMessage)  /* Error handling function */
{
	printf(errorMessage);
	exit(0);
}

/**/
//void func(int func_argc,char *func_argv[]){
void func(int func_argc,unsigned short echoServPort){
/**/
	printf("func\n");
	int sock;
	unsigned int echoStringLen;
	char *echoString;


	int send_msg2 = 1;
	char test_recv[1] = {0,};


    int servSock;                    /* Socket descriptor for server */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
//    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */


 /*   if (func_argc != 2)    
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", func_argv[0]);
        exit(1);
    }
*/
//    echoServPort = atoi(func_argv[1]);  /* First arg:  local port */
/*    echoServPort = atoi(func_argv[1]);

    printf("echoServPort=%d\n",echoServPort);
*/
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

	if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
                               &clntLen)) < 0)
	{
            DieWithError("accept() failed");
	}

	printf("echoServPort=%d\n",echoServPort);



	//while(ros::ok())
	while(1)
	{
		printf("test1\n");
		if(recv(clntSock,test_recv,4,0) <= 0 )
			DieWithError((char*)"accept");

		printf("test_recv2: %d\n", test_recv[0]);
		sleep(1);	
	}
}



void msgCallback(const std_msgs :: Int16 :: ConstPtr & msg)
{
	ROS_INFO("recieve msg = %d",msg->data);
	int send_msg = msg->data;

	if(clntSock <= 0)
	{
		printf("clntSock not\n");
	}
	else
	{

		if( send( clntSock, &send_msg, 4, 0 ) < 0 )
					DieWithError( "send() error" );
	}
	printf("subscribe end\n");
}

void HandleTCPClient(int clntSocket);   /* TCP client handling function */

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "android_communication");
    ros::NodeHandle nh;
    std_msgs::Int16 Int16;
    
    twist_pub = nh.advertise<std_msgs::Int16>("/android", 1);

	unsigned short Port;
	if (argc != 2)    
	    {
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	    }
	Port = atoi(argv[1]);

    printf("Port=%d\n",Port);

	std::thread th(func, argc, Port);
//	ros::Subscriber ros_tutorial_sub = nh.subscribe("/android_communication",1, msgCallback);
 //   std::thread th(func, argc, *argv);
	
	
/**/	    
    ros::Rate rate(1);
    while (ros::ok()) 

    {
	ros::Subscriber ros_tutorial_sub = nh.subscribe("/android_communication",1, msgCallback);
//	printf("test_recv_main: %s\n", test_recv[0]);


//        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
/*	Int16.data = test_recv[0];
	twist_pub.publish(Int16);
*/

	rate.sleep();
	ros::spinOnce();
    }
    /* NOT REACHED */
}
