/* File: spmv_mohammad.cpp
 *
 Copyright (c) [2016] [Mohammad Hosseinabady (mohammad@hosseinabady.com)]

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
*
* File name : spmv_mohammad.cpp
* Author    : Mohammad Hosseinabady mohammad@hosseinabady.com
* Date      : 14 September 2018
* Blog      : https://highlevel-synthesis.com/
* Project   : ENEAC project at Bristol University (Jose Nunez-Yanez)
*/


#include "spmv_mohammad.h"
#include <hls_stream.h>


void spmv_kernel(
		int *rowSizeNew_local,
		int *rowSize_local,
		int *columnIndex,
		DATA_TYPE *values,
		DATA_TYPE *y,
		DATA_TYPE *x_local,
		int row_size,
		int nnz,
		int new_nnz
) {
#pragma HLS DATAFLOW
	int row_size_tmp=0;
	int j = 0;
	int k = 0;
	DATA_TYPE y_tmp = 0;
	int row_counter = 0;

	hls::stream<DATA_TYPE>       values_fifo;
	hls::stream<int>             col_indices_fifo;

	for (int i = 0; i < nnz; i+=1) {
#pragma HLS pipeline
		values_fifo << values[i];
		col_indices_fifo << columnIndex[i];
	}

	int row_size_remains = 0;
	for (int i = 0; i < new_nnz; i+=II) {
#pragma HLS pipeline

		if (row_size_tmp == 0) {
			row_size_tmp = rowSizeNew_local[j];
			row_size_remains = 0;
			y_tmp = 0;
			row_counter	= rowSize_local[j++];
		}

		DATA_TYPE y_local = 0;
		for (int p = 0; p < II; p++) {
			row_size_remains++;
			if (row_size_remains > row_counter) {
				y_local +=  0;
			} else {
				DATA_TYPE v = values_fifo.read();
				int   ci = col_indices_fifo.read();
				y_local +=  v*x_local[ci];
			}
		}

		y_tmp += y_local;
		row_size_tmp-=II;

		if (row_size_tmp == 0) {
			y[k] = y_tmp;
			k++;
		}
	}
}



#pragma SDS data zero_copy(rowPtr[0:(ROW_SIZE_MAX+1)])
#pragma SDS data zero_copy(columnIndex[0:(NNZ_MAX)])
//#pragma SDS data access_pattern(columnIndex:SEQUENTIAL)
#pragma SDS data zero_copy(values[0:(NNZ_MAX)])
//#pragma SDS data access_pattern(values:SEQUENTIAL)
#pragma SDS data zero_copy(y[0:(ROW_SIZE_MAX)])
#pragma SDS data zero_copy(x[0:(COL_SIZE_MAX)])

void spmv_mohammad(
		int *rowPtr,
		int *columnIndex,
		DATA_TYPE *values,
		DATA_TYPE *y,
		DATA_TYPE *x,
		int row_size,
		int nnz) {

	int rowPtr_local[ROW_SIZE_MAX+1];
	int rowSize_local[ROW_SIZE_MAX];
	int rowSizeNew_local[ROW_SIZE_MAX];
	DATA_TYPE x_local[COL_SIZE_MAX];
	int new_nnz;

	new_nnz = nnz;
	for (int i=0; i<(row_size+1); i++){
#pragma HLS pipeline
		int rs;
		int nrs;

		rowPtr_local[i] = rowPtr[i];
		if (i > 0) {
			rowSize_local[i-1] = rowPtr_local[i] - rowPtr_local[i-1];
			rs = rowSize_local[i-1];
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
			rowSizeNew_local[i-1] = nrs;
		}
	}


	for (int i=0; i<(row_size); i++){
#pragma HLS pipeline
		 x_local[i] = x[i];
	}

	spmv_kernel(
			rowSizeNew_local,
			rowSize_local,
			columnIndex,
			values,
			y,
			x_local,
			row_size,
			nnz,
			new_nnz
	);


}
