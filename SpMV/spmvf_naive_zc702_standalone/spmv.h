/* File: spmv.h
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
* File name : spmv.h
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 24 May 2017
* blog: https://highlevel-synthesis.com/
*/

#ifndef __SPMV_H__
#define __SPMV_H__

#include "sds_lib.h"

#define  ROWS                   500//1216  //500 //4 //1024
#define  COLS                   500//1216  //500 //5 //1024
#define  DATA_LENGTH            5888 //12000// 3070 //10 //2048
#define  MODIFIED_DATA_LENGTH   DATA_LENGTH


#define  MAX_COL_SIZE 10000
typedef unsigned long int u32;
typedef double REAL_DATA_TYPE;
typedef u32 INT_DATA_TYPE;

#pragma SDS data zero_copy(values[0:MODIFIED_DATA_LENGTH], cols[0:COLS], rows[0:ROWS], x[0:COLS], y[0:ROWS])
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
	);

#endif //__SPMV_H__
