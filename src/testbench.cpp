#include "testbench.h"
#include "compute_hw.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __SDSCC__
#include <sds_lib.h>
#endif

void create_data(unsigned char* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		data[i] = i;
	}
}

int check_data( unsigned char* in, unsigned char* out, uint32_t length)
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
	unsigned char* input;
	unsigned char* output;

#ifdef __SDSCC__
	//
	input = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS );

	//
	output = (unsigned char*)sds_alloc( sizeof(unsigned char)* NUM_ELEMENTS );
#else
	//
	input = (unsigned char*)malloc( sizeof(unsigned char)* NUM_ELEMENTS );

	//
	output = (unsigned char*)malloc( sizeof(unsigned char)* NUM_ELEMENTS );
#endif

	if( input == NULL || output == NULL)
	{
		std::cout << "ERROR ALLOC" <<std::endl;
		return 0;
	}
	//
	create_data( input, NUM_ELEMENTS );

	//
	compute_hw(&input[0],&output[0], NUM_ELEMENTS);

	//
	int pass = check_data( input,output,NUM_ELEMENTS );

#ifdef __SDSCC__
	if(pass){
		std::cout << "TEST FAILED " << std::endl;
	}
	else{
		std::cout << "TEST PASSED " << std::endl;
	}

	sds_free(output);
	sds_free(input);
#else
	free(output);
	free(input);
#endif
	return 0;
}
