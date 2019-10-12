#include "app3.h"


void application3_hw( hls::stream<unsigned char> in_stream[NUM_INSTANCES], unsigned char out_buff[NUM_ELEMENTS], uint32_t length )
{
	unsigned char index = 0;
	// new pragma but what does it do? How is this helpful?
	#pragma HLS LOOP_TRIPCOUNT min=NUM_ELEMENTS max=NUM_ELEMENTS 
	for( uint32_t i = 0; i < length; i++)
	{
		// read from DMA stream
		unsigned char in_data = in_stream[index].read();

		//read data from other queue. We can maintain order this way
		index = (index == (NUM_INSTANCES-1) ) ? 0 : index+1;

		// write to application stream
		out_buff[i] = in_data;
	}
}
