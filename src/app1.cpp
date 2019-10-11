#include "app1.h"


void application1_hw( const char in_buff[NUM_ELEMENTS], hls::stream<char> &out_stream,uint32_t length )
{
	for( uint32_t i = 0; i < length; i++)
	{
		// read from DMA stream
		char in_data = in_buff[i];

		// write to application stream
		out_stream.write(in_data);
	}
}
