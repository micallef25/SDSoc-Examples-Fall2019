#ifndef _COMPUTE_H_
#define _COMPUTE_H_
#include <stdint.h>
#include "testbench.h"
#include "sds_utils.h"

#pragma SDS data copy(input[0:NUM_ELEMENTS], output[0:NUM_ELEMENTS])
#pragma SDS data mem_attribute( input:PHYSICAL_CONTIGUOUS, output:PHYSICAL_CONTIGUOUS )
#pragma SDS data access_pattern( input:SEQUENTIAL, output:SEQUENTIAL )
void compute_hw( const char input[NUM_ELEMENTS], char output[NUM_ELEMENTS], uint32_t length);

#endif
