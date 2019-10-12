#ifndef _APP2_H_
#define _APP2_H_
#include "testbench.h"
#include "sds_utils.h"
#include "compute_hw.h"

void application2_hw( hls::stream<unsigned short> &in_stream, hls::stream<unsigned short> &out_stream, unsigned short done_bit );

#endif
