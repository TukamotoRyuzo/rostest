#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ros/ros.h>

#define RCVBUFSIZE 32
	
void DieWithError(char *errorMessage)  /* Error handling function */
{
	printf(errorMessage);
	exit(0);
}

int main( int argc, char *argv[] )
{

	ros::init(argc, argv, "sirial_tutorial_msg_publisher");
// ROSシステムとの通信のためのノードハンドルを宣言
	ros::NodeHandle nh;
	int sock;
	struct sockaddr_in echoServAddr;
	unsigned short echoServPort;
	char *servIP;
	char echoBuffer[RCVBUFSIZE];
	unsigned int echoStringLen;
	int bytesRcvd, totalBytesRcvd;
	char *echoString;

	char test_recv[1] = {0,};

	if( (argc < 3 ) || (argc > 4 ) ) {
		fprintf( stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0] );
		exit( 1 );
	}

	servIP = argv[1];
	echoString = argv[2];

	if( argc == 4 )
		echoServPort = atoi( argv[3] );
	else
		echoServPort = 7;

	if( (sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
		DieWithError( "socket() failed" );

	memset( &echoServAddr, 0, sizeof( echoServAddr ) );
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr( servIP );
	echoServAddr.sin_port = htons( echoServPort );

	if( connect( sock,(struct sockaddr *) &echoServAddr, sizeof(echoServAddr) ) < 0 )
		DieWithError( "connect() failed" );

	echoStringLen = strlen(echoString);


	while(1)
	{
	int e;
	printf("scanf now");
	scanf("%d", &e);


	if( send( sock, &e, 4, 0 ) < 0 )
		DieWithError( "send() error" );



	if(e == 0)
		break;

	sleep(1);

	printf( "\n" );

//	exit( 0 );
	}
}
