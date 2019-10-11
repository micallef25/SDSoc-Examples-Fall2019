#include "app2.h"


void application2_hw( hls::stream<uint8_t> &in_stream, hls::stream<uint8_t> &out_stream,uint32_t length )
{
	for( uint32_t i = 0; i < length; i++)
	{
		// read from DMA stream
		uint8_t in_data = in_stream.read();

		// write to application stream
		out_stream.write(in_data);
	}
}
