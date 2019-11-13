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
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)

#ifdef __SDSCC__
#include <sds_lib.h>
#include "sds_utils.h"
#endif

// globals for thread sharing
// the key word volatile is important!
volatile int go =0;
int offset =0;
unsigned char* file;
unsigned char* file2;

typedef long long int u64;
unsigned char* ocm8_vptr;
unsigned char* sds_map;
int ocm_fd;

//int init_ocm()
//{
//   unsigned int ocm_size = 0x40000;
//   off_t ocm_pbase = 0xFFFC0000; // physical base address
//   ocm8_vptr = NULL;
////   int fd;
//
//   // Map the OCM physical address into user space getting a virtual address for it
//   if ((ocm_fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1) {
//	   //if (( ocm8_vptr = (unsigned char*)mmap((void*)ocm_pbase, ocm_size, PROT_READ|PROT_WRITE, MAP_SHARED, ocm_fd, 0)) == NULL)
//	   if (( ocm8_vptr = (unsigned char*)mmap(NULL, ocm_size, PROT_READ|PROT_WRITE, MAP_SHARED, ocm_fd, ocm_pbase)) == NULL)
//	   {
//		   printf("mmap failed\n");
//	   }
//	   sds_map = (unsigned char*)sds_mmap((void*)ocm_pbase, ocm_size, (void*)ocm8_vptr);
//
//	   printf("%p\n",sds_map);
//	   printf("%p\n",ocm8_vptr);
//      // Write to the memory that was mapped, use devmem from the command line of Linux to verify it worked
//      // it could be read back here also
//
//	   //ocm64_vptr[0] = 0xDEADBEEFFACEB00C;
//     // close(fd);
//   }
//   return 0;
// }

void handle_input(int argc, char* argv[],int* chunksize)
{
	int x;
	extern char *optarg;
	extern int optind, optopt, opterr;


	while ((x = getopt(argc, argv, ":c:")) != -1)
	{
		switch(x)
		{
		case 'c':
		    *chunksize = atoi(optarg);
			printf("chunkisize is set to %d optarg\n",*chunksize);
			break;
    	case ':':
        	printf("-%c without parameter\n", optopt);
        	break;
    	}
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

// our thread will execute here
// please note that print statements do not work on cores != 0
void * write_data(void * arg)
{
	go = 0;
	while(go == 0)
	{
		// busy wait so we can see the thread in top
	}
	// write file to root and you can use diff tool on board
	int outfd = open("p.dat", O_CREAT | O_WRONLY);
	if(outfd < 0)
		perror("opening");

	// testing that child thread on separate core can do transfers to fpga
	compute_hw(&file[0],&file2[0],offset-2048);

	//
	int bytes_written = write(outfd,(unsigned char*)&file[0],offset);

	printf("write file with %d\n",bytes_written);
	if(bytes_written < 0)
		perror("writing");

	close(outfd);
//	close(ocm_fd);
	pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
	unsigned char* input[NUM_PACKETS];
	int writer = 0;
	int done = 0;
	int length = 0;
	int count = 0;
	ESE532_Server server;
	int s;
	cpu_set_t cpuset;


	// default is 2k
	int chunksize = CHUNKSIZE;

	// set chunksize if decalred through command line
	handle_input(argc,argv,&chunksize);

	// specify this set to only go on core2
	// valid cores for our assignmnet our 0 - 3
	CPU_ZERO(&cpuset);
	CPU_SET(2, &cpuset);

	// Thread id
	pthread_t threadId;

	// Create a thread that will write our data to disk
	int err = pthread_create(&threadId, NULL, write_data, NULL);
	// Check if thread is created sucessfuly
	if (err)
	{
		std::cout << "Thread creation failed : " << strerror(err);
		return err;
	}
	else
		std::cout << "Thread Created with ID : " << threadId << std::endl;

	// here is where we tell the thread to exist
	// we filled in cpuset earlier to say only go on cpu2
	s = pthread_setaffinity_np(threadId, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "pthread_setaffinity_np1");


	file = (unsigned char*)sds_alloc( sizeof(unsigned char)* 16000 );
	if(file == NULL)
	{
		printf("help\n");
	}

	file2 = (unsigned char*)sds_alloc( sizeof(unsigned char)* 16000 );
	if(file2 == NULL)
	{
		printf("help\n");
	}


	printf("Hi Eric!\n");

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
	server.setup_server( chunksize );


#ifdef __SDSCC__
	sds_utils::perf_counter hw_ctr;

	std::cout << "Starting test run"  << std::endl;

#endif
	// 1 resource is about 1.5gbps 2 resources is about 3gbps
	for(int i =0; i < pipe_depth; i+=1)
	{
		server.get_packet(input[i]);
		count++;
		//printf("packet: %d\n",count);

		// get packet
		unsigned char* buffer = input[i];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		// printing takes time so be weary of transfer rate
		printf("length: %d offset %d\n",length,offset);

#pragma SDS async(1);
		compute_hw(&buffer[HEADER],&file[offset], length);

		//
		offset+= length;
	}

	hw_ctr.start();
	writer = pipe_depth;
	std::cout << "filled pipeline" << std::endl;

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
		//printf("packet while: %d\n",count);


		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		//printf("length: %d\n",length);

#pragma SDS wait(1);
#pragma SDS async(1);
		compute_hw(&buffer[HEADER],&file[offset], length);

		//
		offset+= length;
		writer++;
	}


	// flush the pipe
	for(int i =0; i < pipe_depth; i++)
	{
#pragma SDS wait(1);
	}

#ifdef __SDSCC__
	hw_ctr.stop();

	std::cout << "Bytes processed: " << offset * sizeof(char) << " Average number of CPU cycles in hardware: " << hw_ctr.avg_cpu_cycles() << std::endl;
#endif

	std::cout << "Waiting for thread to exit" << std::endl;

	// signal to thread
	go = 1;

	// Wait for thread to exit
	err = pthread_join(threadId, NULL);

	// check if joining is sucessful
	if (err)
	{
		std::cout << "Failed to join Thread : " << strerror(err) << std::endl;
		return err;
	}

	for(int i = 0; i < NUM_PACKETS; i++)
	{
		sds_free(input[i]);
	}

	sds_free(file);
	return 0;
}


