/* File: spmm_widebus.cpp
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
* File name : spmm_widebus.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 20 january 2021
* blog: https://highlevel-synthesis.com/
*/

#include "spmm_widebus.h"

#include <hls_stream.h>

void read_x(DATA_TYPE  x[M], DATA_TYPE x_local[NO_THREAD/2][M], u32 col_size) {

	for (u32 i = 0; i < col_size; i++) {
#pragma HLS PIPELINE
		DATA_TYPE xt = x[i];
		for (u32 j = 0; j < NO_THREAD/2; j++) {
			x_local[j][i] = xt;
		}
	}
}

void read_row(u32  row[M], u32 row_local[NO_THREAD][N+1], u32 row_size) {

	for (u32 i = 0; i < row_size+1; i++) {
#pragma HLS PIPELINE
		u32 r  = row[i];
		for (u32 j = 0; j < NO_THREAD; j++) {
			row_local[j][i] = r;

		}
	}
}

void read_value(
		DATA_TYPE_WB               values[NNZ],
		hls::stream<DATA_TYPE_WB> &values_fifo,
		u32                        nnz)
{
	for (u32 i = 0; i < nnz/NO_THREAD+1; i++) {
#pragma HLS pipeline
		values_fifo << values[i];
	}
}

void read_column_index(
		DATA_TYPE_WB               columnIndex[NNZ],
		hls::stream<DATA_TYPE_WB> &columnIndex_fifo,
		u32                        nnz)
{
	for (u32 i = 0; i < nnz/NO_THREAD+1; i++) {
#pragma HLS pipeline
		columnIndex_fifo << columnIndex[i];
	}
}


void spmv_acc(
		hls::stream<DATA_TYPE_WB>       &values_fifo,
		hls::stream<DATA_TYPE_WB>       &columnIndex_fifo,
		DATA_TYPE                        x_local[NO_THREAD/2][M],
		u32                              nnz,
		hls::stream<DATA_TYPE>          &acc_0_fifo,
		hls::stream<DATA_TYPE>          &acc_1_fifo,
		hls::stream<DATA_TYPE>          &acc_2_fifo,
		hls::stream<DATA_TYPE>          &acc_3_fifo)

{

	DATA_TYPE acc = 0;
	for (int i = 0; i < nnz/NO_THREAD+1; i+=1) {
#pragma HLS pipeline
		DATA_TYPE_WB v   = values_fifo.read();
		DATA_TYPE_WB ci  = columnIndex_fifo.read();


		wb_int_data *vf;
		vf =(wb_int_data*)(&v);

		wb_u32_data *ciu;
		ciu =(wb_u32_data*)(&ci);


		DATA_TYPE a, b, c, d;
		DATA_TYPE vx_0, vx_1, vx_2, vx_3;


		(i*NO_THREAD+0 < nnz)? a = vf->a: 0;
		vx_0 = a*x_local[0][ciu->a];

		(i*NO_THREAD+1 < nnz)? b = vf->b: 0;
		vx_1 = b*x_local[1][ciu->b];

		(i*NO_THREAD+2 < nnz)? c = vf->c: 0;
		vx_2 = c*x_local[0][ciu->c];

		(i*NO_THREAD+3 < nnz)? d = vf->d: 0;
		vx_3 = d*x_local[1][ciu->d];

		DATA_TYPE acc0 = acc+vx_0;
		DATA_TYPE acc1 = acc+vx_0+vx_1;
		DATA_TYPE acc2 = acc+vx_0+vx_1+vx_2;
		DATA_TYPE acc3 = acc+vx_0+vx_1+vx_2+vx_3;

		acc_0_fifo << acc0;
		acc_1_fifo << acc1;
		acc_2_fifo << acc2;
		acc_3_fifo << acc3;

		acc = acc3;
	}
}


void spmv_vx_acc(

		u32                              row_local[NO_THREAD][N+1],
		hls::stream<DATA_TYPE>          &acc_0_fifo,
		hls::stream<DATA_TYPE>          &acc_1_fifo,
		hls::stream<DATA_TYPE>          &acc_2_fifo,
		hls::stream<DATA_TYPE>          &acc_3_fifo,
		DATA_TYPE                        y_local[NO_THREAD][N],

		u32                              nnz)

{

	int j = 1;

	int row_size_tmp=0;
	wb_int_data y_acc = {0, 0, 0 ,0};
	int row_counter = 0;

	DATA_TYPE previous_acc = 0;
	for (int i = 0; i < nnz/NO_THREAD+1; i+=1) {
#pragma HLS pipeline
		int next_j = j;



		DATA_TYPE acc_0 = acc_0_fifo.read();
		DATA_TYPE acc_1 = acc_1_fifo.read();
		DATA_TYPE acc_2 = acc_2_fifo.read();
		DATA_TYPE acc_3 = acc_3_fifo.read();


		if (row_local[0][j]-1 == i*NO_THREAD) {
			y_local[0][j-1] = acc_0-previous_acc;
			previous_acc = acc_0;
			next_j = j+1;
		} else if (row_local[0][j]-1 == i*NO_THREAD+1) {
			y_local[0][j-1] = acc_1-previous_acc;
			previous_acc = acc_1;
			next_j = j+1;
		} else if (row_local[0][j]-1 == i*NO_THREAD+2) {
			y_local[0][j-1] = acc_2-previous_acc;
			previous_acc = acc_2;
			next_j = j+1;
		} else if (row_local[0][j]-1 == i*NO_THREAD+3) {
			y_local[0][j-1] = acc_3-previous_acc;
			previous_acc = acc_3;
			next_j = j+1;
		}


		if (row_local[1][j+1]-1 == i*NO_THREAD+1) {
			y_local[1][j] = acc_1-acc_0;
			previous_acc = acc_1;
			next_j = j+2;
		} else if (row_local[1][j+1]-1 == i*NO_THREAD+2) {
			y_local[1][j] = acc_2-acc_1;
			previous_acc = acc_2;
			next_j = j+2;
		} else if (row_local[1][j+1]-1 == i*NO_THREAD+3) {
			y_local[1][j] = acc_3-acc_2;
			previous_acc = acc_3;
			next_j = j+2;
		}


		if (row_local[2][j+2]-1 == i*NO_THREAD+2) {
			y_local[2][j+1] = acc_2-acc_1;
			previous_acc = acc_2;
			next_j = j+3;
		} else if (row_local[2][j+2]-1 == i*NO_THREAD+3) {
			y_local[2][j+1] = acc_3-acc_2;
			previous_acc = acc_3;
			next_j = j+3;
		}

		if (row_local[3][j+3]-1 == i*NO_THREAD+3) {
			y_local[3][j+2] = acc_3-acc_2;
			previous_acc = acc_3;
			next_j = j+4;
		}

		j = next_j;



	}
}

void write_y(
		DATA_TYPE y_local[NO_THREAD][N],

		DATA_TYPE     y[N],
		u32 row_size)
{
	for (u32 i = 0; i < row_size; i++) {
#pragma HLS PIPELINE
		DATA_TYPE yt=0;
		for (u32 j = 0; j < NO_THREAD; j++) {
			 yt += y_local[j][i];
		}
		y[i] = yt;
	}

}

void spmv_dataflow(
		u32                 row_local[NO_THREAD][N+1],

		DATA_TYPE_WB        values[NNZ],
		DATA_TYPE_WB        columnIndex[NNZ],

		DATA_TYPE           x_local[NO_THREAD/2][M],
		DATA_TYPE           y_local[NO_THREAD][N],


		u32 nnz)
{
#pragma HLS DATAFLOW

	hls::stream<DATA_TYPE_WB> values_fifo;
	hls::stream<DATA_TYPE_WB> columnIndex_fifo;

	hls::stream<DATA_TYPE_WB> y_fifo;
	hls::stream<DATA_TYPE_WB> vx_fifo;

	hls::stream<DATA_TYPE>          acc_0_fifo;
	hls::stream<DATA_TYPE>          acc_1_fifo;
	hls::stream<DATA_TYPE>          acc_2_fifo;
	hls::stream<DATA_TYPE>          acc_3_fifo;
	hls::stream<wb_int_data>      y_r;

	read_value(values, values_fifo, nnz);
	read_column_index(columnIndex, columnIndex_fifo, nnz);

	spmv_acc(
			values_fifo,
			columnIndex_fifo,
			x_local,
			nnz,
			acc_0_fifo,
			acc_1_fifo,
			acc_2_fifo,
			acc_3_fifo);
	spmv_vx_acc(
			row_local,
			acc_0_fifo,
			acc_1_fifo,
			acc_2_fifo,
			acc_3_fifo,
			y_local,

			nnz);




}


void spmm_widebus(
		u32           rowPtr[N+1],

		DATA_TYPE_WB  values[NNZ],
		DATA_TYPE_WB  columnIndex[NNZ],

		DATA_TYPE     x[M],
		DATA_TYPE     y[N],

		u32 no_vector,
		u32 col_size,
		u32 row_size,
		u32 nnz)
{
#pragma HLS INTERFACE m_axi port=rowPtr        bundle=gmem0
#pragma HLS INTERFACE m_axi port=values        bundle=gmem0
#pragma HLS INTERFACE m_axi port=columnIndex   bundle=gmem1
#pragma HLS INTERFACE m_axi port=y             bundle=gmem0
#pragma HLS INTERFACE m_axi port=x             bundle=gmem0



	DATA_TYPE x_local[NO_THREAD/2][M];
#pragma HLS ARRAY_PARTITION variable=x_local dim=1 complete
	DATA_TYPE y_local[NO_THREAD][N];
#pragma HLS ARRAY_PARTITION variable=y_local dim=1 complete

	u32       row_local[NO_THREAD][N+1];
#pragma HLS ARRAY_PARTITION variable=row_local dim=1 complete

	for (u32 i = 0; i < row_size; i++) {
#pragma HLS PIPELINE
		for (u32 j = 0; j < NO_THREAD; j++) {
			y_local[j][i] = 0;
		}
	}

	for (int i = 0; i < no_vector; i++) {
		read_x(&x[i*col_size], x_local, col_size);
		read_row(rowPtr, row_local, row_size);
		spmv_dataflow(
				row_local,

				values,
				columnIndex,

				x_local,
				y_local,

				nnz);
		write_y(y_local, &y[i*row_size], row_size);
	}
}
