#ifndef _APP3_H_
#define _APP3_H_
#include "testbench.h"
#include "sds_utils.h"
#include "compute_hw.h"

void application3_hw( hls::stream<unsigned short> in_stream[NUM_INSTANCES], unsigned short out_buff[NUM_ELEMENTS], unsigned short length );

#endif
