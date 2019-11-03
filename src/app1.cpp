#include "app1.h"


void application1_hw( const unsigned char in_buff[NUM_ELEMENTS], hls::stream<unsigned short> out_stream[NUM_INSTANCES],uint32_t length )
{
	unsigned char index = 0;
	unsigned short in_data = 0;
	unsigned short i = 0;

	// new pragma but what does it do? How is this helpful?
	loop1:for( i = 0; i < length; i++)
	{
	#pragma HLS loop_tripcount min=200 max=512
	#pragma HLS pipeline II=1
		// read from DMA input stream
		in_data = in_buff[i];

		// write to application stream
		out_stream[index].write(in_data);

		//select next application to write to
		index = (index == (NUM_INSTANCES-1) ) ? 0 : index+1;

	}
	// signal to stop the system each instance gets its own bit
	out_stream[0].write(END_TRANSFER_INSTANCE_1);
	out_stream[1].write(END_TRANSFER_INSTANCE_2);
}
