#include "testbench.h"
#include "compute_hw.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
//#include "server.h"
//#include <unistd.h>
//#include <fcntl.h>

#define NUM_PACKETS 36
#define pipe_depth 4

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
	//unsigned short* file;
	int pass = 0;
	int offset = 0;

	printf("Hi Eric\n");
	// get alot of space to send vmlinuz.tar
	unsigned char* file = (unsigned char*)(malloc(sizeof(unsigned char) * 70000000));
	assert(file != NULL);

	// get alot of space to send vmlinuz.tar
	//char* file = (char*)(malloc(sizeof(char) * 70000000));
	//assert(file != NULL);

	// place in same directory as elf file
//	int fd = open("vmlinuz.tar",O_RDONLY );
//	assert(fd > 0);

	int byte = 1;

	//lastbyte = byte;
	// read fille in to compare
//	byte = read(fd,&file[0],70000000);
//	if(byte < 0)
//	{
//		perror("reading");
//		free(file);
//		exit(0);
//	}

	// lastbyte = byte;
	printf("bytes_read %d\n",byte);

//#ifdef __SDSCC__
//		file = (unsigned short*)sds_alloc( sizeof(unsigned short)* NUM_ELEMENTS * NUM_PACKETS );
//#else
//		file = (unsigned short*)malloc( sizeof(unsigned short)* NUM_ELEMENTS * NUM_PACKETS );
//#endif
//		if(file == NULL)
//		{
//			std::cout << "aborting " <<std::endl;
//			return 1;
//		}

	for(int i = 0; i < NUM_PACKETS; i++)
	{
#ifdef __SDSCC__
		output[i] = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS );
		input[i] = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS );
#else
		output[i] = (unsigned char*)malloc( sizeof(unsigned char)* NUM_ELEMENTS );
#endif
		if(output[i] == NULL)
		{
			std::cout << "aborting " <<std::endl;
			return 1;
		}
		if(input[i] == NULL)
		{
			std::cout << "aborting " <<std::endl;
			return 1;
		}
		//input[i] = get_packet();
	}

	//ESE532_Server server;

	//server.setup_server();

#ifdef __SDSCC__
	sds_utils::perf_counter hw_ctr;

	std::cout << "Starting test run"  << std::endl;

	hw_ctr.start();
#endif
	int count = 0;
	// 1 resource is about 1.5gbps 2 resources is about 3gbps
	for(int i =0; i < pipe_depth; i+=1)
	{
		//server.get_packet(input[i]);
		count++;
		printf("packet: %d\n",count);
#pragma SDS async(1);
#pragma SDS resource(1);
		compute_hw(input[i],output[i], NUM_ELEMENTS);
//		server.get_packet((unsigned char*)input[i+1]);
//#pragma SDS async(2);
//#pragma SDS resource(2);
//		compute_hw(input[i+1],output[i+1], NUM_ELEMENTS);
	}

	int i = pipe_depth;
	int done = 0;
	int writer = 0;;
	//last message
	while(!done)
	{
		// reset ring buffer
		if(i == NUM_PACKETS)
		{
			i = 0;
		}
		if(writer == NUM_PACKETS)
		{
			writer = 0;
		}

		//done = server.get_packet(input[i]);
		count++;

		printf("packet while: %d\n",count);

		if(done)
		{
			break;
		}

#pragma SDS wait(1);
#pragma SDS async(1);
#pragma SDS resource(1);
		compute_hw(input[i],output[i], NUM_ELEMENTS);

		printf("write %d reader %d offset %d \n",writer,i,offset);

		append_data(output[writer],&file[offset],NUM_ELEMENTS);

		offset+= NUM_ELEMENTS;
		writer+=1;
		i+=1;

//		done = server.get_packet((unsigned char*)input[i+1]);
//#pragma SDS wait(2);
//#pragma SDS async(2);
//#pragma SDS resource(2);
//		compute_hw(input[i+1],output[i+1], NUM_ELEMENTS);
		//append_data(output[i+1-pipe_depth],&file[offset],NUM_ELEMENTS);
		//offset+= NUM_ELEMENTS;
	}

	//
	for(int k = i ; k < i + pipe_depth; k+=1)
	{
		if(writer == NUM_PACKETS)
		{
			writer = 0;
		}
//#pragma SDS wait(1);
		printf("out i %d offset %d \n",writer,offset);
		append_data(output[writer],&file[offset],NUM_ELEMENTS);
		writer++;
		offset+= NUM_ELEMENTS;
//#pragma SDS wait(2);
		//append_data(output[ring],&file[offset],NUM_ELEMENTS);
		//offset+= NUM_ELEMENTS;
	}

#ifdef __SDSCC__
	hw_ctr.stop();

	std::cout << "Bytes processed: " << NUM_PACKETS * NUM_ELEMENTS * sizeof(short) << " Average number of CPU cycles in hardware: " << hw_ctr.avg_cpu_cycles() << std::endl;
#endif
	//
	for(int i =0; i < count; i++){
		std::cout << "iter : " << i << std::endl;
		pass |= check_data( input[i],&file[i*NUM_ELEMENTS],NUM_ELEMENTS );
	}
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
		//sds_free(input[i]);
	}
	sds_free(file);
#else
	for(int i = 0; i < NUM_PACKETS; i++)
		{
			sds_free(output[i]);
		//	sds_free(input[i]);
		}
#endif
	return 0;
}
