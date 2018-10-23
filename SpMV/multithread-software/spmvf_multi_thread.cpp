/* File: spmvf_multi_thread.cpp
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
* File name : spmvf_multi_thread.cpp
* Author    : Mohammad Hosseinabady mohammad@hosseinabady.com
* Date      : 14 September 2018
* Blog      : https://highlevel-synthesis.com/
* Project   : ENEAC project at Bristol University (Jose Nunez-Yanez)
*/

#include "spmvf.h"


void spmv_openmp(DATA_TYPE * z, DATA_TYPE * data, u32 * colind, u32 * row_ptr, DATA_TYPE * x, u32 row_size, u32 col_size, u32 data_size){

	long i, j, ckey;

	DATA_TYPE tmp;
	{
  	  #pragma omp parallel for private(ckey,j) 
		for (i=0; i<row_size; i++ ){
			z[i]=0;
			for (ckey=row_ptr[i]; ckey<row_ptr[i+1]; ckey++) {
				j = colind[ckey];
				tmp = data[ckey]*x[j];
				z[i] +=  tmp;

			}
		}
	}
}
