#include "testbench.h"
#include "compute_hw.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __SDSCC__
#include <sds_lib.h>
#endif

void create_data(char* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; length++)
	{
		data[i] = i;
	}
}

int check_data( char* in, char* out, uint32_t length)
{
	for( uint32_t i = 0; i < length; i++)
	{
		if(in[i] != out[i])
			return 1;
	}
	return 0;
}


int main()
{
	char* input;
	char* output;
	//
	input = (char*)sds_alloc( sizeof(char)* NUM_ELEMENTS );

	//
	output = (char*)sds_alloc( sizeof(char)* NUM_ELEMENTS );

	//
	create_data( input, NUM_ELEMENTS );

	//
	compute_hw(&input[0],&output[0], NUM_ELEMENTS);

	//
	int pass = check_data( input,output,NUM_ELEMENTS );

	//
	if(pass){
		std::cout << "TEST FAILED " << std::endl;
	}
	else{
		std::cout << "TEST PASSED " << std::endl;
	}

	sds_free(output);
	sds_free(input);
	return 0;
}
