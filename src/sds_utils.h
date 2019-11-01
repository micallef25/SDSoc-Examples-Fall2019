/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#ifndef SDS_UTILS_H_
#define SDS_UTILS_H_
#include <stdint.h>
#include <hls_stream.h>

#ifdef __SDSCC__
//#include <ap_int.h>
#include "sds_lib.h"
#endif

// advanced precision data types are not compiling at this moment
//https://forums.xilinx.com/t5/SDSoC-Environment-and-reVISION/quot-hls-half-h-quot-Error-no-member-named-in-namespace-std/td-p/810297
//#define EMPTY 0xFFFF
//typedef ap_uint<256> uint256_t;
//typedef ap_uint<1> uint1_t;
//typedef ap_uint<4> uint4_t;
//typedef ap_uint<257> uint257_t;
//typedef ap_uint<33> uint33_t;
//typedef ap_uint<9> uint9_t;
//ap_uint<10> temp;

//
//#define EOF_BITS 10
//#define EOF_MASK (1 << (EOF_BITS-1))
//#define EOC_BITS 9
//#define EOC_MASK (1 << (EOC_BITS-1))


#ifdef __SDSCC__

namespace sds_utils {
    class perf_counter
    {

    private:
        uint64_t tot, cnt, calls;

    public:
        perf_counter() : tot(0), cnt(0), calls(0) {};
        inline void reset() { tot = cnt = calls = 0; }
        inline void start() { cnt = sds_clock_counter(); calls++; };
        inline void stop() { tot += (sds_clock_counter() - cnt); };
        inline uint64_t avg_cpu_cycles() {return (tot / calls); };
        inline uint64_t cpu_cycles() {return tot;};
    };
}

#endif

#endif
