#ifndef _APP2_H_
#define _APP2_H_
#include "testbench.h"
#include "sds_utils.h"

void application2_hw( hls::stream<unsigned char> &in_stream, hls::stream<unsigned char> &out_stream, uint32_t length );

#endif
