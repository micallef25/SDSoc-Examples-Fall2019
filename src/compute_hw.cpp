#include "testbench.h"
#include "compute_hw.h"
#include "sds_utils.h"
#include "app1.h"
#include "app2.h"
#include "app3.h"
#include <stdint.h>

void compute_hw( const char input[NUM_ELEMENTS], char output[NUM_ELEMENTS], uint32_t length)
{
	static hls::stream<char> in_stream;
	static hls::stream<char> Temp;

#pragma HLS STREAM variable=in_stream depth=2
//#pragma HLS STREAM variable=in_stream[1] depth = 2;
#pragma HLS STREAM variable=Temp depth=8

#pragma HLS DATAFLOW
	application1_hw(&input[0],Temp,length);
	//application2_hw(in_stream,Temp,length);
	application3_hw(Temp,&output[0],length);


}
