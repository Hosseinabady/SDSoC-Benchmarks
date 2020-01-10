//#include "spmm_openmp.h"
//
//
//void spmm_openmp(
//	DATA_TYPE    *values,
//	u32          *colIndices,
//	u32          *rowPtr,
//	DATA_TYPE    *x,
//	u32           no_vectors,
//	DATA_TYPE    *y,
//	u32           rows,
//	u32           cols,
//	u32           nnz,
//	u32           begin,
//	u32           end
//	)
//{
//
//
//	u32 nvc, i, j, ckey;
//
//	DATA_TYPE tmp;
//	{
//#pragma omp parallel for private(ckey,j, nvc)
//		for (nvc = 0; nvc < no_vectors; nvc++) {
//			for (i = begin; i < end; i++) {
//				y[nvc*cols+i] = 0;
//				for (ckey = rowPtr[i]; ckey < rowPtr[i + 1]; ckey++) {
//					j = colIndices[ckey];
//					tmp = values[ckey] * x[nvc*cols+j];
//					y[nvc*rows+i] += tmp;
//				}
//			}
//		}
//	}
//}
