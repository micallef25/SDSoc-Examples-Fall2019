#include "testbench.h"
#include "compute_hw.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "server.h"
#include <unistd.h>
#include <fcntl.h>

#define NUM_PACKETS 36
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)

#ifdef __SDSCC__
#include <sds_lib.h>
#endif

void create_data(unsigned short* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		data[i] = rand() % 255;
	}
}

void append_data( unsigned char* data, unsigned char* to_append, uint32_t length )
{
	for(uint32_t i = 0; i < length; i++)
	{
		to_append[i] = data[i];
	}
}

unsigned short* get_packet()
{
	//
#ifdef __SDSCC__
	unsigned short* input = (unsigned short*)sds_alloc( sizeof(unsigned short)* NUM_ELEMENTS );
#else
	unsigned short* input = (unsigned short*)malloc( sizeof(unsigned short)* NUM_ELEMENTS );
#endif
	if(input == NULL)
	{
		std::cout << "aborting " <<std::endl;
		exit(1);
	}
	//
	create_data( input, NUM_ELEMENTS );
	return input;
}


int check_data( unsigned char* in, unsigned char* out, uint32_t length)
{
	for( uint32_t i = 0; i < length; i++)
	{
		if(in[i] != out[i]){
		std::cout << "test failed index " << i << std::endl;
			return 1;
		}
	}
	return 0;
}


int main()
{
	unsigned char* input[NUM_PACKETS];
	unsigned char* output[NUM_PACKETS];
	int offset = 0;
	int writer = 0;
	int done = 0;
	int reader = 0;
	int length = 0;
	int count = 0;
	ESE532_Server server;

	unsigned char* file = (unsigned char*)malloc( sizeof(unsigned char)* 70000000 );

	printf("Hi Eric\n");

	for(int i = 0; i < NUM_PACKETS; i++)
	{
#ifdef __SDSCC__
		//output[i] = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS + HEADER );
		input[i] = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS + HEADER );
#else
		output[i] = (unsigned char*)malloc( sizeof(unsigned char)* NUM_ELEMENTS + HEADER );
#endif
//		if(output[i] == NULL)
//		{
//			std::cout << "aborting " <<std::endl;
//			return 1;
//		}
		if(input[i] == NULL)
		{
			std::cout << "aborting " <<std::endl;
			return 1;
		}
	}

	//
	server.setup_server();

#ifdef __SDSCC__
	sds_utils::perf_counter hw_ctr;

	std::cout << "Starting test run"  << std::endl;

	hw_ctr.start();
#endif
	// 1 resource is about 1.5gbps 2 resources is about 3gbps
	for(int i =0; i < pipe_depth; i+=1)
	{
		server.get_packet(input[i]);
		count++;
		printf("packet: %d\n",count);

		// get packet
		unsigned char* buffer = input[i];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;

//#pragma SDS async(1);
//#pragma SDS resource(1);
		compute_hw(&buffer[HEADER],&file[offset], length);
		offset+= length;
	}

	writer = pipe_depth;
	reader = pipe_depth;

	//last message
	while(!done)
	{
		// reset ring buffer
		if(reader == NUM_PACKETS)
		{
			reader = 0;
		}
		if(writer == NUM_PACKETS)
		{
			writer = 0;
		}


		server.get_packet(input[writer]);
		count++;
		printf("packet while: %d\n",count);


		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;

//#pragma SDS async(1);
//#pragma SDS resource(1);
		compute_hw(&buffer[HEADER],&file[offset], length);

		//
		offset+= length;
		writer+=1;
		reader+=1;
	}

	int temp_reader = reader;

	//
//	for(int k = temp_reader ; k < temp_reader + pipe_depth; k+=1)
//	{
//		if(reader == NUM_PACKETS)
//		{
//			reader = 0;
//		}
//#pragma SDS wait(1);
//		unsigned char* buffer = output[reader];
//
//		printf("reader %d offset %d \n",reader,offset);
//
//		//
//		done = buffer[1] & DONE_BIT_L;
//		length = buffer[0] | (buffer[1] << 8);
//		length &= ~DONE_BIT_H;
//
//		//
//		memcpy(&file[offset],&buffer[HEADER],length);
//
//		//
//		offset+= length;
//
//		//
//		reader++;
//	}

#ifdef __SDSCC__
	hw_ctr.stop();

	std::cout << "Bytes processed: " << NUM_PACKETS * NUM_ELEMENTS * sizeof(short) << " Average number of CPU cycles in hardware: " << hw_ctr.avg_cpu_cycles() << std::endl;
#endif
	//
	int outfd = open("compress.dat", O_CREAT);

	//
	int bytes_written = write(outfd,file,offset);

	printf("write file with %d\n",bytes_written);


#ifdef __SDSCC__
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		//sds_free(output[i]);
		sds_free(input[i]);
	}
#else
	for(int i = 0; i < NUM_PACKETS; i++)
		{
			sds_free(output[i]);
			sds_free(input[i]);
		}
#endif
	free(file);
	return 0;
}
