#pragma once

#include "spmm_widebus.h"

void spmm_widebus(
		u32           rowPtr[N+1],

		DATA_TYPE_WB  values[NNZ],
		DATA_TYPE_WB  columnIndex[NNZ],

		DATA_TYPE     x[M],
		DATA_TYPE     y[N],

		u32 no_vector,
		u32 col_size,
		u32 row_size,
		u32 nnz);
