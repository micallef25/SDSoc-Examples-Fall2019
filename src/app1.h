#ifndef _APP1_H_
#define _APP1_H_

#include "testbench.h"
#include "sds_utils.h"


#pragma SDS data copy(in_buff[0:NUM_ELEMENTS])
#pragma SDS data access_pattern(in_buff:SEQUENTIAL)
#pragma SDS data mem_attribute(in_buff:PHYSICAL_CONTIGUOUS)
void application1_hw( const char in_buff[NUM_ELEMENTS], hls::stream<char> &out_stream,uint32_t length );

#endif
