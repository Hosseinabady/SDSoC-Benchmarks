/* File: spmv_tb.cpp
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
* File name : spmv_tb.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 24 May 2017
* blog: https://highlevel-synthesis.com/
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "spmv_utilities.h"
#include <iostream>
#include "spmv.h"
#include "time.h"

#include "sds_lib.h"

class perf_counter
{
public:
     uint64_t tot, cnt, calls;
     perf_counter() : tot(0), cnt(0), calls(0) {};
     inline void reset() { tot = cnt = calls = 0; }
     inline void start() { cnt = sds_clock_counter(); calls++; };
     inline void stop() { tot += (sds_clock_counter() - cnt); };
     inline uint64_t avg_cpu_cycles() { return (tot / calls); };
};



int max_index = 0;

void SpMV_Ref(int n, REAL_DATA_TYPE* values, u32* cols, u32* rows, REAL_DATA_TYPE* x, REAL_DATA_TYPE* y);

void spmv_init( REAL_DATA_TYPE *values, REAL_DATA_TYPE *x, u32 *cols, u32* rows) {
	spmv_generate(values, x, cols, rows);
}



int main() {
	printf("\rHello SpMV!\n\r");

	perf_counter hw_ctr, sw_ctr;
	int status = 0;
	REAL_DATA_TYPE *x = NULL,  *y_sw = NULL, *y_hw = NULL, *values = NULL;
	u32    *rows = NULL, *cols = NULL;
	u32     row_size, col_size, data_size;
	row_size = ROWS;
	col_size = COLS;
	data_size = MODIFIED_DATA_LENGTH;
	REAL_DATA_TYPE *memory_REAL_DATA_TYPE = (REAL_DATA_TYPE *) sds_alloc  ((2*ROWS+COLS+MODIFIED_DATA_LENGTH)*sizeof(REAL_DATA_TYPE) );
    values                      = memory_REAL_DATA_TYPE;
    x                           = memory_REAL_DATA_TYPE + MODIFIED_DATA_LENGTH;
    y_sw                        = memory_REAL_DATA_TYPE + MODIFIED_DATA_LENGTH+COLS;
    y_hw                        = memory_REAL_DATA_TYPE + MODIFIED_DATA_LENGTH+COLS+ROWS;


    u32 *memory_int = (u32 *) sds_alloc (sizeof(u32) * (ROWS + MODIFIED_DATA_LENGTH));
    rows      = memory_int;
    cols      = memory_int + ROWS;

    spmv_init( values, x, cols, rows);




    sw_ctr.start();
    SpMV_Ref(row_size, values, rows, cols, x, y_sw);
    sw_ctr.stop();

    uint64_t sw_cycles = sw_ctr.avg_cpu_cycles();
    std::cout << "Average number of CPU cycles running mmult insoftware: "
               << sw_cycles  << std::endl;


	printf("\rHardware version started!\n\r");

    hw_ctr.start();
	spmv_accel(values, cols, rows, x, y_hw, max_index, row_size, col_size, data_size);
    hw_ctr.stop();

    uint64_t hw_cycles = hw_ctr.avg_cpu_cycles();
    std::cout << "Average number of CPU cycles running mmult in hardware: "
               << hw_cycles  << std::endl;


	for(int i=0;i<row_size;i++) {
		REAL_DATA_TYPE diff = fabs(y_sw[i]-y_hw[i]);
		if(diff > 0.0001 || diff != diff){
			printf("error occurs at  %d with value y_ hw = %lf, should be y_ sw = %lf \n",i,y_hw[i],y_sw[i]);
			status = -1;
			break;
	    }

	}
	if(!status) {
		printf("Validation PASSED!\n");
	} else {
		printf("Validation FAILED!\n");
	}


	sds_free(y_sw);
	sds_free(y_hw);
	sds_free(values);
	sds_free(x);
	sds_free(cols);
	sds_free(rows);

	sds_free(memory_REAL_DATA_TYPE);
	sds_free(memory_int);



	printf("\rBye SpMV!\n\r");

    return 0;
}


void SpMV_Ref(int n, REAL_DATA_TYPE* AR, u32* IA, u32* JA, REAL_DATA_TYPE* x, REAL_DATA_TYPE* y) {

    int i=0, j=0, rowStart=0, rowEnd=n;
    long int k=0;
    REAL_DATA_TYPE y0=0.0;
    int last_j = 0;
    for (i = rowStart; i < rowEnd; ++i) {
    	y0 = 0.0;
    	int new_index_j = last_j+IA[i];
        for (j = last_j ; j < new_index_j; ++j) {
        	y0 += AR[j] * x[JA[j]];
        }
        last_j = last_j + IA[i];
        y[i] = y0;
    }
}
