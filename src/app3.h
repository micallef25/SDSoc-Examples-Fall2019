#ifndef _APP3_H_
#define _APP3_H_
#include "testbench.h"
#include "sds_utils.h"
#include <stdint.h>

void application3_hw( hls::stream<unsigned char> in_stream[NUM_INSTANCES], unsigned char out_buff[NUM_ELEMENTS], uint32_t length );

#endif
