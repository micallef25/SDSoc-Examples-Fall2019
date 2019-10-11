#ifndef _APP3_H_
#define _APP3_H_
#include "testbench.h"
#include "sds_utils.h"
#include <stdint.h>
#pragma SDS data copy(out_buff[0:NUM_ELEMENTS])
#pragma SDS data access_pattern(out_buff:SEQUENTIAL)
#pragma SDS data mem_attribute(out_buff:PHYSICAL_CONTIGUOUS)
void application3_hw( hls::stream<char> &in_stream, char out_buff[NUM_ELEMENTS], uint32_t length );

#endif
