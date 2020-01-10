

#include <spmm_block.h>


u32 spmm_block(
		DATA_TYPE *values,
		u32       *colIndices,
		u32       *rowPtr,
		DATA_TYPE *x,
		u32        no_vectors,

		DATA_TYPE *y,
		u32        rows,
		u32        cols,
		u32        nnz,
		u32        begin,
		u32        end

		) {


	spmm_mohammad(
			rowPtr,

			colIndices,
			colIndices,
			colIndices,
			colIndices,

			values,
			values,
			values,
			values,

			y,
			y,
			y,
			y,

			x,
			no_vectors,

			cols,
			rows,
			nnz,

			begin,
			end,
			rowPtr[begin]


			);

	return 0;


}
