/* File: spmv_mohammad.h
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
* File name : spmv_mohammad.h
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 18 February 2019
* blog: https://highlevel-synthesis.com/
*/

#ifndef __SPMV_MOHAMMAD__
#define __SPMV_MOHAMMAD__

#include "ap_int.h"


/*
 * for clock frequency 100 MHZ   II=4
 * for clock frequency 200 MHZ   II=8
 * for clock frequency 300 MHZ   II=10
 */
const static int II = 10;


typedef float DATA_TYPE;
typedef unsigned int u32;
const static int ROW_SIZE_MAX = (1024*128);
const static int COL_SIZE_MAX = ROW_SIZE_MAX;
const static int NNZ_MAX =  (1024*1024*16);


void spmv_mohammad(
		u32 *rowPtr,
		u32 *columnIndex,
		DATA_TYPE *values,
		DATA_TYPE *y,
		DATA_TYPE *x,
		u32 col_size,
		u32 row_size,
		u32 nnz
		);



#endif
