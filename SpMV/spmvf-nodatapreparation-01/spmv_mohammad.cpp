/* File: spmv_mohammad.cpp
 *
 Copyright (c) [2019] [Mohammad Hosseinabady (mohammad@hosseinabady.com)]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
* This file has been written at University of Bristol
* for the ENEAC project (Jose Nunez-Yanez) funded by EPSRC
*
* File name : spmv_mohammad.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 18 February 2019
* blog: https://highlevel-synthesis.com/
*/


#include "spmv_mohammad.h"
#include <hls_stream.h>



void spmv_kernel(
		u32 *rowSize_local_rs,
		u32 *rowSize_local_nrs,
		u32 *columnIndex,
		DATA_TYPE *values,
		DATA_TYPE *y,
		DATA_TYPE *x_local,
		u32 row_size,
		u32 nnz,
		u32 new_nnz
) {

#pragma HLS DATAFLOW
	int row_size_tmp=0;
	int j = 0;

	DATA_TYPE y_tmp = 0;
	u32 row_counter = 0;

	hls::stream<DATA_TYPE>       values_fifo;
#pragma HLS STREAM variable=values_fifo depth=64 dim=1
	hls::stream<u32>             col_indices_fifo;
#pragma HLS STREAM variable=col_indices_fifo depth=64 dim=1
	hls::stream<DATA_TYPE>       y_fifo;
#pragma HLS STREAM variable=y_fifo depth=64 dim=1


	for (u32 i = 0; i < nnz; i+=1) {
#pragma HLS pipeline
		values_fifo << values[i];
	}
	for (u32 i = 0; i < nnz; i+=1) {
#pragma HLS pipeline
		col_indices_fifo << columnIndex[i];
	}



	u32 row_size_remains = 0;
	for (u32 i = 0; i < new_nnz; i+=II) {
#pragma HLS pipeline
		if (row_size_tmp == 0) {
			row_size_tmp = rowSize_local_nrs[j];
			row_size_remains = 0;
			y_tmp = 0;
			row_counter	= rowSize_local_rs[j++];
		}

		DATA_TYPE y_local = 0;
		for (u32 p = 0; p < II; p++) {
			row_size_remains++;
			if (row_size_remains > row_counter) {
				y_local +=  0;
			} else {
				DATA_TYPE v = values_fifo.read();
				u32   ci = col_indices_fifo.read();
				y_local +=  v*x_local[ci];
			}
		}

		y_tmp += y_local;
		row_size_tmp-=II;

		if (row_size_tmp == 0) {
			y_fifo << y_tmp;
		}
	}

	for (u32 i = 0; i < row_size; i+=1) {
#pragma HLS pipeline
		y[i] = y_fifo.read();
	}
}



#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data sys_port(rowPtr:ps_e_S_AXI_HP0_FPD)

#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data sys_port(columnIndex:ps_e_S_AXI_HP0_FPD)

#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data sys_port(values:ps_e_S_AXI_HP1_FPD)

#pragma SDS data zero_copy(y[0:(row_size)])
#pragma SDS data sys_port(y:ps_e_S_AXI_HP0_FPD)

#pragma SDS data zero_copy(x[0:(col_size)])
#pragma SDS data sys_port(x:ps_e_S_AXI_HP1_FPD)

void spmv_mohammad(
		u32 *rowPtr,
		u32 *columnIndex,
		DATA_TYPE *values,
		DATA_TYPE *y,
		DATA_TYPE *x,
		u32 col_size,
		u32 row_size,
		u32 nnz
		) {


	u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
	u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];

	DATA_TYPE x_local[COL_SIZE_MAX];




#pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1



	u32 new_nnz = nnz;
	u32 idx_previous = rowPtr[0];
	for (u32 i=1; i<(row_size+1); i++){
#pragma HLS pipeline
		u32 rs;
		u32 nrs;

		int idx2 = rowPtr[i];

		rs = idx2 - idx_previous;
		idx_previous = idx2;
		if (rs  == 0) {
			nrs = II;
			new_nnz += II;
		}
		else if (rs%II == 0) {
			nrs = rs;
			new_nnz += 0;
		} else {
			nrs = rs + (II-rs%II);
			new_nnz += (II-rs%II);
		}


		rowSizeNew_local_rs[i-1] = rs;
		rowSizeNew_local_nrs[i-1] = nrs;
	}




	for (u32 i=0; i<(col_size); i++){
#pragma HLS pipeline
		 x_local[i] = x[i];
	}

	spmv_kernel(
			rowSizeNew_local_rs,
			rowSizeNew_local_nrs,
			columnIndex,
			values,
			y,
			x_local,
			row_size,
			nnz,
			new_nnz
	);


}
