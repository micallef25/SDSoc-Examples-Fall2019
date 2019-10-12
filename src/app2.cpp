#include "app2.h"


void application2_hw( hls::stream<unsigned char> &in_stream, hls::stream<unsigned char> &out_stream,uint32_t length )
{
	unsigned char lookup_table[8096];
	for(uint32_t i =0; i < 8096; i++)
	{
		lookup_table[i] = i;
	}
	unsigned char done_flag = 0;

	// new pragma but what does it do? How is this helpful?
	#pragma HLS LOOP_TRIPCOUNT min=NUM_ELEMENTS/NUM_INSTANCES max=NUM_ELEMENTS/NUM_INSTANCES 
	for( uint32_t i = 0; i < length/NUM_INSTANCES; i++)
	{
	//while( !done_flag )
	//{
#pragma HLS pipeline II=1
		// read from DMA stream
		unsigned char in_data = in_stream.read();

		// do something to use some resources...
		in_data = in_data + 1;

		//
		in_data = lookup_table[in_data];

		// check done bit
		done_flag = in_data >> 8;

		// write to application stream
		out_stream.write(in_data);
	}
}
