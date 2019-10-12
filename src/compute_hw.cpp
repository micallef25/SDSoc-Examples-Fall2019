#include "testbench.h"
#include "compute_hw.h"
#include "sds_utils.h"
#include "app1.h"
#include "app2.h"
#include "app3.h"
#include <stdint.h>

// our top level function DMA is streamed in and out see header for declaration
void compute_hw( const unsigned char input[NUM_ELEMENTS], unsigned char output[NUM_ELEMENTS], uint32_t length)
{
	static hls::stream<unsigned char> in_stream[NUM_INSTANCES];
	static hls::stream<unsigned char> Temp[NUM_INSTANCES];

// you should consider adjusting your depth to something appropriate for your application
// if your application runs 10x slower think your queue needs to be 10x bigger
#pragma HLS STREAM variable=in_stream[0] depth=2
#pragma HLS STREAM variable=in_stream[1] depth=2
#pragma HLS STREAM variable=Temp[0] depth=8
#pragma HLS STREAM variable=Temp[1] depth=8

// consider why the data flow pragma may be useful we did this in homework 7
#pragma HLS DATAFLOW
// scatter the data to app2 
	application1_hw(&input[0],in_stream,length);

// creates two resources with different streams
// you can see the two distinct resources in debug/_sds/reports/sds_compute_hw.rpt
// this report tells you your resource utilization. It may differ from what Vivado HLS tells you.
#pragma SDS resource(1)
	application2_hw(in_stream[0],Temp[0],length);
#pragma SDS resource(2)
	application2_hw(in_stream[1],Temp[1],length);

// gather the data
	application3_hw(Temp,&output[0],length);


}
