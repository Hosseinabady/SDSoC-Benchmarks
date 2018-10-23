#ifndef __SPMV_MOHAMMAD__
#define __SPMV_MOHAMMAD__

#include "ap_int.h"

const static int II = 4;

typedef ap_uint<64> u64;

typedef float DATA_TYPE;
typedef unsigned int u32;
const static int ROW_SIZE_MAX = (1024*128);
const static int COL_SIZE_MAX = ROW_SIZE_MAX;
const static int NNZ_MAX =  (1024*1024*16);


void spmv_mohammad(
		int rowPtr[ROW_SIZE_MAX+1],
		int columnIndex[NNZ_MAX],
		DATA_TYPE values[NNZ_MAX],
		DATA_TYPE y[ROW_SIZE_MAX],
		DATA_TYPE x[COL_SIZE_MAX],
		int row_size,
		int col_size,
		int nnz);


void spmv_openmp(
		DATA_TYPE *z,
		DATA_TYPE *data,
		int       *colind,
		int       *row_ptr,
		DATA_TYPE *x,
		int        row_size,
		int        col_size,
		int        data_size);
#endif
