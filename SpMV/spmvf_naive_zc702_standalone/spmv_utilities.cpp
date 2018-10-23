/* File: spmv_utilities.cpp
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
* File name : spmv_utilities.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 24 May 2017
* blog: https://highlevel-synthesis.com/
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <spmv.h>

void spmv_generate( REAL_DATA_TYPE *values, REAL_DATA_TYPE *x, u32 *cols, u32* rows) {

//	srand((unsigned)time(NULL));
	REAL_DATA_TYPE *all_data = (REAL_DATA_TYPE*)malloc(ROWS*COLS*sizeof(REAL_DATA_TYPE));

	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			*(all_data + r*COLS + c) = 0;

	for (int r = 0; r < ROWS; r++) {
		int c = (rand()%(COLS)) ;
		*(all_data + r*COLS + c) = 2.034;
	}
	for(int i = 0; i < MODIFIED_DATA_LENGTH; i++) {
		values[i] = 0;
		cols[i]   = 0;
	}

	//generate  DATA_LENGTH randomly non-sero data
	for(int i = 0; i < DATA_LENGTH-ROWS; i++) {

		int r = (rand()%(ROWS)) ;
		int c = (rand()%(COLS)) ;

		REAL_DATA_TYPE v =*(all_data+r*COLS+c);

		if (v != 0) {
			i--;
			continue;
		}

		do {
			v = (REAL_DATA_TYPE)(1.0*rand()+1.0)/RAND_MAX;
		}while(fabs(v) < 0.0000000001);

		*(all_data + r*COLS + c) = v;

	}

	int k = 0;

	for (int r = 0; r < ROWS; r++){
		rows[r] = 0;
		for (int c = 0; c < COLS; c++) {
			REAL_DATA_TYPE v =*(all_data+r*COLS+c);
			if (v != 0) {
				rows[r]++;
			}
		}
	}


	for (int r = 0; r < ROWS; r++){
		for (int c = 0; c < COLS; c++) {
			REAL_DATA_TYPE v =*(all_data+r*COLS+c);
			if (v != 0) {
				cols[k]   = c;
				values[k] = v;
				k++;
			}
		}

	}

	printf("length = %d \n", k);
    for (int i = 1; i < COLS; i++) {
    	x[i] = (1.0*rand()+1.0)/RAND_MAX;
    }

	free(all_data);


}





