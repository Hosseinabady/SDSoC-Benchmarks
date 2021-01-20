#pragma once

#include <ap_int.h>

#define DATA_TYPE     int
#define DATA_TYPE_OUT int
typedef unsigned int  u32;

typedef ap_uint<128>  u128;
#define DATA_TYPE_WB  u128

#define NO_THREAD     4

struct wb_int_data{
	int a;
	int b;
	int c;
	int d;
};

struct wb_u32_data{
	u32 a;
	u32 b;
	u32 c;
	u32 d;
} ;



#define NNZ (1024*1024)
#define N    4096  // ROW size
#define M    4096  // COL size
#define P    4096  // # of vectors

