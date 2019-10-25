#include "testbench.h"
#include "compute_hw.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>

#define NUM_PACKETS 36
#define pipe_depth 10

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

void append_data( unsigned short* data, unsigned short* to_append, uint32_t length )
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


int check_data( unsigned short* in, unsigned short* out, uint32_t length)
{
	for( uint32_t i = 0; i < length; i++)
	{
		if(in[i] != out[i])
			return 1;
	}
	return 0;
}


int main()
{
	unsigned short* input[NUM_PACKETS];
	unsigned short* output[NUM_PACKETS];
	unsigned short* file;
	int pass = 0;
	int offset = 0;

#ifdef __SDSCC__
		file = (unsigned short*)sds_alloc( sizeof(unsigned short)* NUM_ELEMENTS * NUM_PACKETS );
#else
		file = (unsigned short*)malloc( sizeof(unsigned short)* NUM_ELEMENTS * NUM_PACKETS );
#endif
		if(file == NULL)
		{
			std::cout << "aborting " <<std::endl;
			return 1;
		}

	for(int i = 0; i < NUM_PACKETS; i++)
	{
#ifdef __SDSCC__
		output[i] = (unsigned short*)sds_alloc( sizeof(unsigned short)* NUM_ELEMENTS );
#else
		output[i] = (unsigned short*)malloc( sizeof(unsigned short)* NUM_ELEMENTS );
#endif
		if(output[i] == NULL)
		{
			std::cout << "aborting " <<std::endl;
			return 1;
		}
		input[i] = get_packet();
	}

	sds_utils::perf_counter hw_ctr;

	//unsigned short* temp = input[NUM_PACKETS-1];
	//temp[NUM_ELEMENTS-1] = EOF_BIT; // EOF BIT

	std::cout << "Starting test run"  << std::endl;

	hw_ctr.start();

	//
	for(int i =0; i < pipe_depth; i++)
	{
		//input[i] = get_packet();
#pragma SDS async(1);
#pragma SDS resource(1);
		compute_hw(input[i],output[i], NUM_ELEMENTS);
		//std::cout << "starting pipe " << i << std::endl;
//#pragma SDS async(2);
//#pragma SDS resource(2);
//		compute_hw(input[i+1],output[i+1], NUM_ELEMENTS);
	}

	//
	for(int i =pipe_depth; i < NUM_PACKETS; i++)
	{
		//input[i] = get_packet();
#pragma SDS wait(1);
#pragma SDS async(1);
#pragma SDS resource(1);
		compute_hw(input[i],output[i], NUM_ELEMENTS);
		//append_data(output[i-pipe_depth], &file[offset],NUM_ELEMENTS);
		offset += NUM_ELEMENTS;
		//std::cout << "running pipe " << i << "appending: "<< i-pipe_depth << std::endl;
//#pragma SDS wait(2);
//#pragma SDS async(2);
//#pragma SDS resource(2);
//		compute_hw(input[i+1],output[i+1], NUM_ELEMENTS);
	}

	//
	for(int i =NUM_PACKETS-pipe_depth ; i < NUM_PACKETS; i++)
	{
		//std::cout << "flushing pipe " << i << "appeding: " << i << std::endl;
#pragma SDS wait(1);
		//append_data(output[i],&file[offset],NUM_ELEMENTS);
		offset += NUM_ELEMENTS;
//#pragma SDS wait(2);
	}

	hw_ctr.stop();

	std::cout << "Bytes processed: " << NUM_PACKETS * NUM_ELEMENTS * sizeof(short) << " Average number of CPU cycles in hardware: " << hw_ctr.avg_cpu_cycles() << std::endl;

//	unsigned short* tempout = output[NUM_PACKETS-1];
//	unsigned short* tempin = input[NUM_PACKETS-1];
//	//temp[NUM_ELEMENTS-1] = EOF_BIT; // EOF BIT
//
//	std::cout << "debug_ctr: "  << tempout[NUM_ELEMENTS-1] << std::endl;
//	std::cout << "eof: "  << tempin[NUM_ELEMENTS-1] << std::endl;

	//
	for(int i =0; i < NUM_PACKETS; i++)
		pass |= check_data( input[i],&file[i*NUM_ELEMENTS],NUM_ELEMENTS );

#ifdef __SDSCC__
	if(pass){
		std::cout << "TEST FAILED " << std::endl;
	}
	else{
		std::cout << "TEST PASSED " << std::endl;
	}
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		sds_free(output[i]);
		sds_free(input[i]);
	}
	sds_free(file);
#else
	for(int i = 0; i < NUM_PACKETS; i++)
		{
			sds_free(output[i]);
			sds_free(input[i]);
		}
#endif
	return 0;
}
