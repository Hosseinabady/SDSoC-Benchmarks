#ifndef __SPMM_H__
#define __SPMM_H__




typedef float DATA_TYPE;
typedef unsigned int u32;

void spmm_openmp(
	DATA_TYPE    *values,
	u32          *colIndices,
	u32          *rowPtr,
	DATA_TYPE    *x,
	u32           no_vectors,
	DATA_TYPE    *y,
	u32           rows,
	u32           cols,
	u32           nnz,
	u32           begin,
	u32           end
	);


#endif

