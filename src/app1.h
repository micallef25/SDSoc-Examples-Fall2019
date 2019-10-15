#ifndef _APP1_H_
#define _APP1_H_

#include "testbench.h"
#include "sds_utils.h"
#include "compute_hw.h"

void application1_hw( const unsigned short in_buff[NUM_ELEMENTS], hls::stream<unsigned short> out_stream[NUM_INSTANCES], uint32_t length );

#endif
