/* File: spmm_widebus-tb.h
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
* File name : spmm_widebus-tb.h
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 20 january 2021
* blog: https://highlevel-synthesis.com/
*/
#pragma once

#include "spmm_widebus.h"

void spmm_widebus(
		u32           rowPtr[N+1],

		DATA_TYPE_WB  values[NNZ],
		DATA_TYPE_WB  columnIndex[NNZ],

		DATA_TYPE     x[M],
		DATA_TYPE     y[N],

		u32 no_vector,
		u32 col_size,
		u32 row_size,
		u32 nnz);
