#ifndef _APP2_H_
#define _APP2_H_
#include "testbench.h"
#include "sds_utils.h"

void application2_hw( hls::stream<uint8_t> &in_stream, hls::stream<uint8_t> &out_stream, uint32_t length );

#endif
