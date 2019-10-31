#include "app3.h"


void application3_hw( hls::stream<unsigned short> in_stream[NUM_INSTANCES], unsigned char out_buff[NUM_ELEMENTS], unsigned short done_bit )
{
	unsigned char index = 0;
	unsigned short in_data = 0;
	unsigned short done_flag = 0;
	unsigned short i =0;

	// think of how static variables or static tables can help you build your LZW dictionary if a file is broken into many packets
	//
	static unsigned short debug_ctr = 0;

	// new pragma but what does it do? How is this helpful?
	while( done_flag != done_bit  )
	{
//	#pragma HLS LOOP_TRIPCOUNT min=256 max=256
//	#pragma HLS pipeline II=1
		// read from DMA stream
		in_data = in_stream[index].read();

		//read data from other queue. We can maintain order this way
		index = (index == (NUM_INSTANCES-1) ) ? 0 : index+1;

		// extract our flag
		done_flag |= in_data & done_bit;

		// if actually data we will write it to the output buffer
		if( (done_flag != END_TRANSFER_INSTANCE_1) && (done_flag != END_TRANSFER_INSTANCE_2) && (done_flag != done_bit) )
		{
			//in_data = (in_data == EOF_BIT) ? debug_ctr : in_data;

			// write to application stream
			out_buff[i] = in_data;
			// index to buffer write
			i++;
		}

		// same concept as in app2.cpp
		debug_ctr++;
	}
}
