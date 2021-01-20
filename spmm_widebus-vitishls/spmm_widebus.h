/* File: spmm_widebus.h
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
* File name : spmm_widebus.h
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 20 january 2021
* blog: https://highlevel-synthesis.com/
*/

#pragma once

#include <ap_int.h>

#define DATA_TYPE     int
#define DATA_TYPE_OUT int
typedef unsigned int  u32;

typedef ap_uint<128>  u128;
#define DATA_TYPE_WB  u128

#define NO_THREAD     4

struct wb_int_data{
	int a;
	int b;
	int c;
	int d;
};

struct wb_u32_data{
	u32 a;
	u32 b;
	u32 c;
	u32 d;
} ;



#define NNZ (1024*1024)
#define N    4096  // ROW size
#define M    4096  // COL size
#define P    4096  // # of vectors

