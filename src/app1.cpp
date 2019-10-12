#include "app1.h"


void application1_hw( const unsigned char in_buff[NUM_ELEMENTS], hls::stream<unsigned char> out_stream[NUM_INSTANCES],uint32_t length )
{
	unsigned char index = 0;
	// new pragma but what does it do? How is this helpful?
	#pragma HLS LOOP_TRIPCOUNT min=NUM_ELEMENTS max=NUM_ELEMENTS 
	for( uint32_t i = 0; i < length; i++)
	{
		// read from DMA stream input stream
		unsigned char in_data = in_buff[i];

		// write to application stream
		out_stream[index].write(in_data);

		//select next application to write to
		index = (index == (NUM_INSTANCES-1) ) ? 0 : index+1;
	}
}
