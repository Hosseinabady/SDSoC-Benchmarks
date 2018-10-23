/* File: spmv.cpp
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
* This file has been written at University of Bristol
* for the ENPOWER project funded by EPSRC
*
* File name : spmv.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 24 May 2017
* blog: https://highlevel-synthesis.com/
*/


#include "spmv.h"
#include <hls_stream.h>


void spmv_accel(
	REAL_DATA_TYPE  values[MODIFIED_DATA_LENGTH],
	INT_DATA_TYPE	cols[COLS],
	INT_DATA_TYPE   rows[ROWS],
	REAL_DATA_TYPE  x[COLS],
	REAL_DATA_TYPE	y[ROWS],
	u32        		max_n,
	u32        		row_size,
	u32        		col_size,
	u32        		data_size
	) {

	REAL_DATA_TYPE              x_local[MAX_COL_SIZE];


	u32 col_left=0;
	REAL_DATA_TYPE sum;
	REAL_DATA_TYPE value;
	u32 col;
	int row_index = 0;

	for (u32 i = 0; i < col_size; i++) {
#pragma HLS PIPELINE
		x_local[i] = x[i];
	}



	for (u32 r = 0; r < data_size; r++) {
#pragma HLS PIPELINE
		if (col_left == 0) {
			col_left = rows[row_index];
			sum = 0;
		}
		value = values[r];
		col   = cols[r];
		sum  += value * x_local[col];

		col_left--;

		if (col_left == 0) {
			y[row_index++] = sum;
		}
	}

}

