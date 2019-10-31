//// Server side implementation of UDP client-server model
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
//#include "sds_lib.h"
//#include "server.h"
//
//
//#define PORT	 8080
//#define CHUNKSIZE 4096 // Tuneable. must match the transmitter side
//
//
//// basic
//int ESE532_Server::setup_server()
//{
//
//	printf("setting up sever...\n");
//
//	//
//	char buffer[CHUNKSIZE];
//	int opt = 1;
//
//
//	// Creating socket file descriptor
//	if ( (sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0 )
//	{
//		perror("socket creation failed");
//		exit(EXIT_FAILURE);
//	}
//
//
//	memset(&servaddr, 0, sizeof(servaddr));
//
//	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
//		perror("sockopt");
//
//
//	// Filling server information
//	//
//	bzero(&servaddr,sizeof(servaddr));
//	servaddr.sin_family = AF_INET; // IPv4
//	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
//	servaddr.sin_port = htons(PORT);
//
//	// Bind the socket with the server address
//	if ( bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
//	{
//		perror("bind failed");
//		exit(EXIT_FAILURE);
//	}
//
//	server_len = sizeof(servaddr);
//
//	printf("server setup complete!\n");
//	//int bytes_read;
//
//	// receive first packet and start timer
//	//bytes_read = recvfrom(sockfd, (void *)buffer, CHUNKSIZE,0, ( struct sockaddr *) &servaddr,&server_len);
//
//	//start clock
////	uint64_t cnt = sds_clock_counter();
////	int pakcets_received = 0;
////
////	// hard coded since i know how many packets are beign sent we have
////	while( pakcets_received != 16864 )
////	{
////		bytes_read = recvfrom(sockfd, (void *)buffer, CHUNKSIZE,0, ( struct sockaddr *) &servaddr,	&len);
////		pakcets_received++;
////		//printf("count %d n %d\n",count,n);
////
////		if( bytes_read < 0 )
////		{
////			perror("receiver");
////		}
////	}
////
////	// end clock
////	uint64_t cnt1 = sds_clock_counter();
////
////	unsigned long diff = cnt1 - cnt;
////
////	printf("cylces to complete %llu\n",diff);
//
//
//	return 0;
//}
//
//int ESE532_Server::get_packet(unsigned char* buffer)
//{
//
//	if(packets_read != 19658)
//	{
//		int bytes_read = recvfrom(sockfd, (void *)buffer, CHUNKSIZE,0, ( struct sockaddr *) &servaddr,	&server_len);
//		printf("byttess_read \n",bytes_read);
//		packets_read++;
//		// crash
//		if( bytes_read < 0 )
//		{
//			perror("recvfrom failed!");
//			//assert(bytes_read > 0);
//		}
//
//		return bytes_read;
//	}
//	return 0;
//}
//
//
