// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __SDSCC__
#include <ff.h>
#include <sds_lib.h>
#endif
#include <iostream>
#include "server.h"


#define PORT	 8080
#define CHUNKSIZE 4096 // tuneable but must match client
#define HEADER 2
#define DONE_BIT (1 << 7)


/*
* checks for a system call error if so exits the application
* code supplied by Andre Dehon from previous homeworks
*/
void Check_error(int Error, const char * Message)
{
  if (Error)
  {
    //fputs(Message, stderr);
    perror(Message);
    exit(EXIT_FAILURE);
  }
}

/*
* checks for a sys call error if so then exits
* code supplied by Andre Dehon from previous homeworks
*/
void Exit_with_error(void)
{
  perror(NULL);
  exit(EXIT_FAILURE);
}


/*
* allocates an unsigned char via alloc or malloc
* code supplied by Andre Dehon from previous homeworks
*/
unsigned char * Allocate(int Size)
{
  unsigned char * Frame = (unsigned char *)
#ifdef __SDSCC__
                          sds_alloc(Size);
#else
                          malloc(Size);
#endif
  Check_error(Frame == NULL, "Could not allocate memory.\n");

  return Frame;
}

/*
* Frees alloc'd or malloc'd data
* code supplied by Andre Dehon from previous homeworks
*/
void Free(unsigned char * Frame)
{
#ifdef __SDSCC__
  sds_free(Frame);
#else
  free(Frame);
#endif
}


/*
* loads data from sd card or file
* code supplied by Andre Dehon from previous homeworks
*/
uint32_t Load_data(unsigned char* Input_data)
{
	uint64_t Size = 7200000;
	unsigned int Bytes_read;
#ifdef __SDSCC__
  FIL File;
  // FIle name must be small please note this!!!
  FRESULT Result = f_open(&File, "p.txt", FA_READ);
  Check_error(Result != FR_OK, "Could not open input file.");

  Result = f_read(&File, Input_data, Size, &Bytes_read);
  Check_error(Result != FR_OK, "Could not read input file.");

  Check_error(f_close(&File) != FR_OK, "Could not close input file.");

#endif

  return Bytes_read;
}

/*
* Stores an output buffer to sd card or file name
* code supplied by Andre Dehon from previous homeworks
*/
void Store_data(const char * Filename,unsigned char * Data, uint32_t Size)
{
#ifdef __SDSCC__
  FIL File;
  unsigned int Bytes_written;

  FRESULT Result = f_open(&File, Filename, FA_WRITE | FA_CREATE_ALWAYS);
  Check_error(Result != FR_OK, "Could not open output file.");

  Result = f_write(&File, Data, Size, &Bytes_written);
  Check_error(Result != FR_OK || Bytes_written != Size, "Could not write output file.");

  Check_error(f_close(&File) != FR_OK, "Could not close output file.");
#else


  FILE * File = fopen(Filename, "wb");
  if (File == NULL)
    Exit_with_error();

  if (fwrite(Data, 1, Size, File) != Size)
    Exit_with_error();

  if (fclose(File) != 0)
    Exit_with_error();
#endif
}


// basic
int ESE532_Server::setup_server()
{
	input_data = (unsigned char*)sds_alloc( sizeof(unsigned char) * 29000 );
	if(input_data == NULL)
	{
		std::cout << "aborting packet" <<std::endl;
		return 1;
	}

	FATFS FS;
	Check_error(f_mount(&FS, "0:/", 0) != FR_OK, "Could not mount SD-card");

	printf("setting up sever...\n");
	// read data from file
	int bytes_read = Load_data(input_data);
        
	// reconcile how to break up the file
	int loops = bytes_read / CHUNKSIZE;
	int remainder = bytes_read % CHUNKSIZE;
	int n;
	
	table_size = loops;
	// account for remainder
	if(remainder)
	{
		table_size++;
	}
        
	
	// make our pointer table
	table = (unsigned char**)sds_alloc( sizeof(unsigned char*) * table_size );
	printf("table constructed of %d packets and %d bytes \n",table_size, bytes_read);
	if(table == NULL)
	{
		std::cout << "aborting packet" <<std::endl;
		return 1;
	}

    // send chunk size bytes
	for( n = 0; n < loops-1; n++)
	{
		unsigned char* packet = (unsigned char*)sds_alloc( sizeof(unsigned char) * (HEADER + CHUNKSIZE) );
		if(packet == NULL)
		{
			std::cout << "aborting packet" <<std::endl;
			return 1;
		}
		//
		memcpy(&packet[HEADER],&input_data[n*CHUNKSIZE],CHUNKSIZE);

		//
		char high = CHUNKSIZE >> 8;
		char low = CHUNKSIZE & 0xFF;

		//
		packet[0] = low;
		packet[1] = high;

		table[n] = packet;
	}

	// if no remainder send the last chunk with done bit
	if(remainder == 0){

		unsigned char* packet = (unsigned char*)sds_alloc( sizeof(unsigned char) * (HEADER + CHUNKSIZE) );
		if(packet == NULL)
		{
			std::cout << "aborting packet" <<std::endl;
			return 1;
		}
		//
		memcpy(&packet[HEADER],&input_data[n*CHUNKSIZE],CHUNKSIZE);

		//
		char high = CHUNKSIZE >> 8;
		char low = CHUNKSIZE & 0xFF;

		//
		packet[0] = low;
		packet[1] = high | DONE_BIT;

		table[n] = packet;
		n++;
	}
	// if remainder then send last chunk as normal
	else{

		unsigned char* packet = (unsigned char*)sds_alloc( sizeof(unsigned char) * (HEADER + CHUNKSIZE) );
		if(packet == NULL)
		{
			std::cout << "aborting packet" <<std::endl;
			return 1;
		}
		//
		memcpy(&packet[HEADER],&input_data[n*CHUNKSIZE],CHUNKSIZE);

		//
		char high = CHUNKSIZE >> 8;
		char low = CHUNKSIZE & 0xFF;

		//
		packet[0] = low;
		packet[1] = high;

		table[n] = packet;

		n++;
	}

	// send remaining pieces
	if(remainder){

		unsigned char* packet =  (unsigned char*)sds_alloc( sizeof(unsigned char) * (CHUNKSIZE + HEADER) );
		if(packet == NULL)
		{
			std::cout << "aborting packet" <<std::endl;
			return 1;
		}
		//
		memcpy(&packet[HEADER],&input_data[loops*CHUNKSIZE],remainder);

		//
		char high = CHUNKSIZE >> 8;
		char low = CHUNKSIZE & 0xFF;

		//
		packet[0] = low;
		packet[1] =  high | DONE_BIT;

		table[n] = packet;
		n++;
	}
	packet_counter = 0;
	printf("server setup complete!\n");

	return 0;
}

int ESE532_Server::get_packet(unsigned char* buffer)
{
	// get next packet and copy into passed in buffer
	memcpy(buffer,table[packet_counter],(CHUNKSIZE + HEADER));
	packet_counter++;
	return (CHUNKSIZE + HEADER);
}


