#ifndef _COMPUTE_H_
#define _COMPUTE_H_
#include <stdint.h>
#include "testbench.h"
#include "sds_utils.h"

#define EOF_BIT 1 << 10
#define NUM_INSTANCES 2
#define END_TRANSFER_INSTANCE_1 1 << 8
#define END_TRANSFER_INSTANCE_2 2 << 8
#define END_TRANSFER_ALL 3 << 8

// consider what other pragmas ay be useful for your assignment
// perhaps over riding the compiler chosen DMA channel is in your best interest?
// look here: https://www.xilinx.com/html_docs/xilinx2019_1/sdsoc_doc/sds-pragmas-nmc1504034362475.html#dzz1504034363418
// we looked at data motion reports in homework 6 what did you observe or notice?
#pragma SDS data copy(input[0:length], output[0:length])
#pragma SDS data mem_attribute( input:PHYSICAL_CONTIGUOUS, output:PHYSICAL_CONTIGUOUS )
#pragma SDS data access_pattern( input:SEQUENTIAL, output:SEQUENTIAL )
void compute_hw( const unsigned char input[NUM_ELEMENTS], unsigned char output[NUM_ELEMENTS],uint32_t length);

#endif
