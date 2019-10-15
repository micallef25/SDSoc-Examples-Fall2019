#include "app2.h"


void application2_hw( hls::stream<unsigned short> &in_stream, hls::stream<unsigned short> &out_stream,unsigned short done_bit )
{
	// use some resources to prove that two instances are actually created
	// verified in compute_hw.rpt
	unsigned char lookup_table[8096];
	for(uint32_t i =0; i < 8096; i++)
	{
		lookup_table[i] = i;
	}
	unsigned short done_flag = 0;
//	unsigned short write_data = 0;
	unsigned short debug_ctr = 0;

	// new pragma but what does it do? How is this helpful?
	#pragma HLS LOOP_TRIPCOUNT min=128 max=128
	while( done_flag != done_bit )
	{
//#pragma HLS pipeline II=1
		// read from DMA stream
		unsigned short in_data = in_stream.read();

		// extract your bit
		done_flag = in_data & done_bit;
		
		// useful for tracking in vivado and emulation.
		// since read and writes are blocking we need to make sure
		// that we read from the stream as many times as it was written to.
		debug_ctr++;

		// write your data. even the flag will trickle down
		out_stream.write(in_data);
	}
}
