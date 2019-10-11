#include "app3.h"
#include <stdint.h>
void application3_hw( hls::stream<char> &in_stream, char out_buff[NUM_ELEMENTS], uint32_t length )
{
	for( uint32_t i = 0; i < length; i++)
	{
		// read from DMA stream
		char in_data = in_stream.read();

		// write to application stream
		out_buff[i] = in_data;
	}
}
