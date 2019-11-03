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

#define NUM_PACKETS 8
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
	int offset = 0;
	int writer = 0;
	int done = 0;
	int length = 0;
	int count = 0;
	ESE532_Server server;

	// allocate alot of space for vmlinuz.tar
	unsigned char* file = (unsigned char*)sds_alloc( sizeof(unsigned char)* 29000 );
	if(file == NULL)
	{
		printf("help\n");
		exit(1);
	}
	printf("Hi Eric!\n");

	// ring buffer of packets
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		input[i] = (unsigned char*)sds_alloc( sizeof(unsigned char)* (NUM_ELEMENTS + HEADER) );
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

#endif
	// 1 resource is about 1.5gbps 2 resources is about 3gbps
	for(int i =0; i < pipe_depth; i+=1)
	{
		server.get_packet(input[i]);
		count++;

		// get packet
		unsigned char* buffer = input[i];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		std::cout << "Length:"  << length << std::endl;

#pragma SDS async(1);
		compute_hw(&buffer[HEADER],&file[offset], length);

		//
		offset+= length;
	}
	hw_ctr.start();
	writer = pipe_depth;

	//last message
	while(!done)
	{
		// reset ring buffer
		if(writer == NUM_PACKETS)
		{
			writer = 0;
		}

		server.get_packet(input[writer]);
		count++;

		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		std::cout << "Length:"  << length << std::endl;


#pragma SDS wait(1);
#pragma SDS async(1);
		compute_hw(&buffer[HEADER],&file[offset], length);

		//
		offset+= length;
		writer++;
	}

	//
	for(int i =0; i < pipe_depth; i++)
	{
#pragma SDS wait(1);
	}

#ifdef __SDSCC__
	hw_ctr.stop();

	std::cout << "Bytes processed: " << offset * sizeof(char) << " Average number of CPU cycles in hardware: " << hw_ctr.avg_cpu_cycles() << std::endl;
#endif

	std::cout << "offset: " << offset << std::endl;

	int fail = check_data(file,server.input_data,offset);

	if(fail)
	{
		printf("test failed \n");
	}
	else
	{
		printf("test passed!!\n");
	}


#ifdef __SDSCC__
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		sds_free(input[i]);
	}
#else
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		sds_free(input[i]);
	}
#endif
	sds_free(file);
	for(int i = 0; i < server.table_size; i++)
	{
		sds_free(server.table[i]);
	}
	sds_free(server.table);
	sds_free(server.input_data);
	return 0;
}
