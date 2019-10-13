#include "testbench.h"
#include "compute_hw.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_PACKETS 8

#ifdef __SDSCC__
#include <sds_lib.h>
#endif

void create_data(unsigned char* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		data[i] = rand() % 255;
	}
}

unsigned char* create_packet()
{
	//
#ifdef __SDSCC__
	unsigned char* input = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS );
#else
	unsigned char* input = (unsigned char*)malloc( sizeof(unsigned char)* NUM_ELEMENTS );
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
		if(in[i] != out[i])
			return 1;
	}
	return 0;
}


int main()
{
	unsigned char* input[NUM_PACKETS];
	unsigned char* output[NUM_PACKETS];
	int pass = 0;
	for(int i = 0; i < NUM_PACKETS; i++)
	{
#ifdef __SDSCC__
		output[i] = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS );
#else
		output[i] = (unsigned char*)malloc( sizeof(unsigned char)* NUM_ELEMENTS );
#endif
		if(output[i] == NULL)
		{
			std::cout << "aborting " <<std::endl;
			return 1;
		}
		input[i] = create_packet();
	}

	sds_utils::perf_counter hw_ctr;

	std::cout << "Starting test run"  << std::endl;

	hw_ctr.start();

	//
	for(int i =0; i < 4; i+=2)
	{
// we can verify if our resource pragma actually worked
// in sds.rpt you ca nsee the resource utilization and compare it to a build that is not using it
#pragma SDS async(1);
#pragma SDS resource(1);
		compute_hw(input[i],output[i], NUM_ELEMENTS);
#pragma SDS async(2);
#pragma SDS resource(2);
		compute_hw(input[i+1],output[i+1], NUM_ELEMENTS);
	}

	//
	for(int i =4; i < NUM_PACKETS; i+=2)
	{
#pragma SDS wait(1);
#pragma SDS async(1);
#pragma SDS resource(1);
		compute_hw(input[i],output[i], NUM_ELEMENTS);
#pragma SDS wait(2);
#pragma SDS async(2);
#pragma SDS resource(2);
		compute_hw(input[i+1],output[i+1], NUM_ELEMENTS);
	}

	//
	for(int i =0; i < 4; i+=2)
	{
#pragma SDS wait(1);
#pragma SDS wait(2);
	}

	hw_ctr.stop();

	std::cout << "Average number of CPU cycles in hardware: " << hw_ctr.avg_cpu_cycles() << std::endl;

	//
	for(int i =0; i < NUM_PACKETS; i++)
		pass |= check_data( input[i],output[i],NUM_ELEMENTS );

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
#else
	for(int i = 0; i < NUM_PACKETS; i++)
		{
			sds_free(output[i]);
			sds_free(input[i]);
		}
#endif
	return 0;
}
