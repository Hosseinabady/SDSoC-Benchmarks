#include "spmv.h"
#include <hls_stream.h>

void spvm_kernel(
		DATA_TYPE  *values,
		u32        *indices,
		DATA_TYPE  *x,
		DATA_TYPE  *y,
		u32         row_size,
		u32         col_size,
		u32         data_size
) {

#pragma HLS DATAFLOW

	hls::stream<u32>       indices_fifo;
	hls::stream<u32>       row_fifo;
//#pragma HLS STREAM variable=row_fifo depth=500 dim=1
	hls::stream<u32>       col_fifo;
//#pragma HLS STREAM variable=col_fifo depth=500 dim=1
	hls::stream<DATA_TYPE> values_fifo;
	hls::stream<DATA_TYPE> results_fifo;



	u32 tmp_int;
	u32 i, r;
	u32 col_left;
	for (i = 0; i < row_size+data_size; i++) {
#pragma HLS PIPELINE
		indices_fifo << *(indices+i);
	}

	for (i = 0; i < data_size; i++) {
#pragma HLS PIPELINE
		values_fifo << *(values+i);
	}
	col_left = 0;
	for (r = 0; r < row_size+data_size; r++) {
#pragma HLS PIPELINE
		u32  indices = indices_fifo.read();
		if (col_left == 0) {
			col_left = indices;
			row_fifo << col_left;
		} else {
			col_fifo << indices;
			col_left--;
		}
	}



	DATA_TYPE sum=0;

	DATA_TYPE value;
	u32 col;
	DATA_TYPE term[II];


	col_left = 0;
	for (r = 0; r < data_size+row_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left == 0) {
			col_left = row_fifo.read();
			sum = 0;
		} else {

			for (int i = 0; i < II; i++) {
				value = values_fifo.read();
				col   = col_fifo.read();
				term[i] = value * x[col];
			}

			DATA_TYPE sum_tmp = 0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term[i];
			}
			sum  += sum_tmp;


			col_left-=II;

			if (col_left == 0) {
				results_fifo << sum;
			}

		}
	}


	for (i = 0; i < row_size; i++) {
#pragma HLS PIPELINE
		y[i] = results_fifo.read();
	}

}





#pragma SDS data zero_copy(values[0:DATA_LENGTH])
#pragma SDS data mem_attribute(values:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(indices[0:INDICES_LENGTH])
#pragma SDS data mem_attribute(indices:PHYSICAL_CONTIGUOUS)

#pragma SDS data zero_copy(x[0:COLS])
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(y[0:ROWS])
#pragma SDS data mem_attribute(y:PHYSICAL_CONTIGUOUS)

void spmv_accel(
	DATA_TYPE   values[DATA_LENGTH],
	u32         indices[INDICES_LENGTH],
	DATA_TYPE   x[COLS],
	DATA_TYPE   y[ROWS],
	u32         row_size,
	u32         col_size,
	u32         data_size
	) {







	DATA_TYPE              x_local[MAX_COL_SIZE];
	DATA_TYPE              y_local[MAX_ROW_SIZE];

	for (u32 i = 0; i < col_size; i++) {
#pragma HLS PIPELINE
		x_local[i] = *(x+i);
	}


	spvm_kernel(
			values,
			indices,
			x_local,
			y_local,
			row_size,
			col_size,
			data_size);

	for (int i = 0; i < row_size; i++) {
#pragma HLS PIPELINE
		*(y+i) = y_local[i];
	}


}
