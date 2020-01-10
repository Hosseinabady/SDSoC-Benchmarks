/* File: spmm_mohammad.cpp
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
* File name : spmm_mohammad.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 18 February 2019
* blog: https://highlevel-synthesis.com/
*/


#include <hls_stream.h>
#include <spmm_mohammad.h>



void spmm_kernel(
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
	u32 row_size_tmp=0;
	u32 j = 0;

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
#pragma SDS data sys_port(rowPtr:ps_e_S_AXI_HPC0_FPD)
//----------------------------------------------------------------
#pragma SDS data zero_copy(columnIndex_0[0:(nnz)])
#pragma SDS data sys_port(columnIndex_0:ps_e_S_AXI_HPC0_FPD)

#pragma SDS data zero_copy(columnIndex_1[0:(nnz)])
#pragma SDS data sys_port(columnIndex_1:ps_e_S_AXI_HPC0_FPD)

#pragma SDS data zero_copy(columnIndex_2[0:(nnz)])
#pragma SDS data sys_port(columnIndex_2:ps_e_S_AXI_HPC1_FPD)

#pragma SDS data zero_copy(columnIndex_3[0:(nnz)])
#pragma SDS data sys_port(columnIndex_3:ps_e_S_AXI_HPC1_FPD)



//----------------------------------------------------------------
#pragma SDS data zero_copy(values_0[0:(nnz)])
#pragma SDS data sys_port(values_0:ps_e_S_AXI_HPC0_FPD)

#pragma SDS data zero_copy(values_1[0:(nnz)])
#pragma SDS data sys_port(values_1:ps_e_S_AXI_HPC0_FPD)

#pragma SDS data zero_copy(values_2[0:(nnz)])
#pragma SDS data sys_port(values_2:ps_e_S_AXI_HPC1_FPD)

#pragma SDS data zero_copy(values_3[0:(nnz)])
#pragma SDS data sys_port(values_3:ps_e_S_AXI_HPC1_FPD)



//----------------------------------------------------------------
#pragma SDS data zero_copy(y_0[0:(row_size*no_vectors)])
#pragma SDS data sys_port(y_0:ps_e_S_AXI_HPC0_FPD)

#pragma SDS data zero_copy(y_1[0:(row_size*no_vectors)])
#pragma SDS data sys_port(y_1:ps_e_S_AXI_HPC0_FPD)

#pragma SDS data zero_copy(y_2[0:(row_size*no_vectors)])
#pragma SDS data sys_port(y_2:ps_e_S_AXI_HPC1_FPD)

#pragma SDS data zero_copy(y_3[0:(row_size*no_vectors)])
#pragma SDS data sys_port(y_3:ps_e_S_AXI_HPC1_FPD)


//----------------------------------------------------------------
#pragma SDS data zero_copy(x[0:(col_size*no_vectors)])
#pragma SDS data sys_port(x:ps_e_S_AXI_HPC1_FPD)

void spmm_mohammad(
		u32 *rowPtr,

		u32 *columnIndex_0,
		u32 *columnIndex_1,
		u32 *columnIndex_2,
		u32 *columnIndex_3,




		DATA_TYPE *values_0,
		DATA_TYPE *values_1,
		DATA_TYPE *values_2,
		DATA_TYPE *values_3,



		DATA_TYPE *y_0,
		DATA_TYPE *y_1,
		DATA_TYPE *y_2,
		DATA_TYPE *y_3,


		DATA_TYPE *x,
		u32        no_vectors,

		u32 col_size,
		u32 row_size,
		u32 nnz,

		u32 begin,
		u32 end,

		u32 first_rowPrt_value


		) {
#pragma HLS DATAFLOW

	u32 rowSizeNew_local_rs[NO_HW_THREAD][ROW_SIZE_THREAD_MAX];
	u32 rowSizeNew_local_nrs[NO_HW_THREAD][ROW_SIZE_THREAD_MAX];
#pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs complete dim=1
#pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs complete dim=1
#pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=2

	//================================================

	DATA_TYPE x_local[NO_HW_THREAD][COL_SIZE_MAX];
#pragma HLS ARRAY_PARTITION variable=x_local complete dim=1
	//================================================

	u32 row_size_threads[NO_HW_THREAD];
	u32 values_offset_threads[NO_HW_THREAD];
	u32 row_offset_threads[NO_HW_THREAD];
	u32 nnz_threads[NO_HW_THREAD];
	u32 new_nnz_threads[NO_HW_THREAD];
#pragma HLS ARRAY_PARTITION variable=row_size_threads complete dim=1
#pragma HLS ARRAY_PARTITION variable=values_offset_threads complete dim=1
#pragma HLS ARRAY_PARTITION variable=row_offset_threads complete dim=1
#pragma HLS ARRAY_PARTITION variable=nnz_threads complete dim=1
#pragma HLS ARRAY_PARTITION variable=new_nnz_threads complete dim=1


	//=======================================================

	{

		u32 ideal_nnz = nnz/NO_HW_THREAD;

		for ( u32 i = 0; i < NO_HW_THREAD; i++) {
	#pragma HLS UNROLL
			row_size_threads[i] = 0;
			nnz_threads[i] = 0;
			new_nnz_threads[i] = 0;
		}

		u32 nrs = 0;
		u32 new_nnz = 0;
		u32 j = 0;
		u32 prev_index = first_rowPrt_value;
		u32 k = 0;
		for (u32 i = 0; i < end-begin; i++) {
	#pragma HLS PIPELINE
			u32 current_index= rowPtr[i+begin+1];
			u32 rs = (current_index-prev_index);

			if (rs  == 0) {
				nrs = II;
				new_nnz = II;
			} else if (rs%II == 0) {
				nrs = rs;
				new_nnz = 0;
			} else {
				nrs = rs + (II-rs%II);
				new_nnz = (II-rs%II);
			}

			u32 t = nnz_threads[j] + rs;
			prev_index = current_index;

			if (t < ideal_nnz) {
				nnz_threads[j] = t;
			} else {
				if (j+1 < NO_HW_THREAD) {
					j++;
					k=0;
					nnz_threads[j] = rs;

				} else {

					nnz_threads[j] = t;

				}
			}
			row_size_threads[j]++;
			new_nnz_threads[j] += new_nnz;
			rowSizeNew_local_rs[j][k]  = rs;
			rowSizeNew_local_nrs[j][k] = nrs;
			k++;
		}


		for ( u32 i = 0; i < NO_HW_THREAD; i++) {
	#pragma HLS UNROLL
			new_nnz_threads[i] += nnz_threads[i];
		}
		values_offset_threads[0] = 0;
		row_offset_threads[0] = 0;
		for ( u32 i = 1; i < NO_HW_THREAD; i++) {
	#pragma HLS UNROLL
			values_offset_threads[i] = values_offset_threads[i-1]+nnz_threads[i-1];
			row_offset_threads[i] = row_offset_threads[i-1] + row_size_threads[i-1];
		}
	}

//=======================================================================================

	for (u32 nv=0; nv < no_vectors; nv++){

			for (u32 i=0; i<(col_size); i++){
		#pragma HLS pipeline
				for (u32 j=0; j<(NO_HW_THREAD); j++){
					x_local[j][i] = x[nv*col_size + i];
				}
			}

//=======================================================================================

			u32 i;

			i = 0;
			spmm_kernel(
					rowSizeNew_local_rs[i],
					rowSizeNew_local_nrs[i],
					columnIndex_0 + first_rowPrt_value + values_offset_threads[i],
					values_0 + first_rowPrt_value + values_offset_threads[i],
					y_0 + begin + nv*row_size + row_offset_threads[i],
					x_local[i],
					row_size_threads[i],
					nnz_threads[i],
					new_nnz_threads[i]
			);

			i = 1;
			spmm_kernel(
					rowSizeNew_local_rs[i],
					rowSizeNew_local_nrs[i],
					columnIndex_1 + first_rowPrt_value + values_offset_threads[i],
					values_1 + first_rowPrt_value + values_offset_threads[i],
					y_1 + begin + nv*row_size + row_offset_threads[i],
					x_local[i],
					row_size_threads[i],
					nnz_threads[i],
					new_nnz_threads[i]
			);

			i = 2;
			spmm_kernel(
					rowSizeNew_local_rs[i],
					rowSizeNew_local_nrs[i],
					columnIndex_2 + first_rowPrt_value + values_offset_threads[i],
					values_2 + first_rowPrt_value + values_offset_threads[i],
					y_2 + begin + nv*row_size + row_offset_threads[i],
					x_local[i],
					row_size_threads[i],
					nnz_threads[i],
					new_nnz_threads[i]
			);

			i = 3;
			spmm_kernel(
					rowSizeNew_local_rs[i],
					rowSizeNew_local_nrs[i],
					columnIndex_3 + first_rowPrt_value + values_offset_threads[i],
					values_3 + first_rowPrt_value + values_offset_threads[i],
					y_3 + begin + nv*row_size + row_offset_threads[i],
					x_local[i],
					row_size_threads[i],
					nnz_threads[i],
					new_nnz_threads[i]
			);
		}


}

