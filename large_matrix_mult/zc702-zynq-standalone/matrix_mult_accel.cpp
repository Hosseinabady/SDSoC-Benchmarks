/* File: matrix_mult_accel.cpp
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
* File name : matrix_mult_accel.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 1 October 2016
* blog: https://highlevel-synthesis.com/
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix_mult.h"

inline void mxv(float* a, float* c, float b[B_HEIGHT][GROUP_SIZE]);
typedef unsigned long u32;

const int BLOCK=64;  //BLOCK should be less than GROUP_SIZE 
const int STEP=1;


/*
the amount of data saved in the FPGA is B_HEIGHT*GROUP_SIZE+GROUP_SIZE+A_WIDTH which should be less than FPGA BRAM size
*/

int mmult_accel( float A[A_HEIGHT*A_WIDTH], float B[B_HEIGHT*B_WIDTH], float C[A_HEIGHT*B_WIDTH])
{

	float b[B_HEIGHT][GROUP_SIZE];
#pragma HLS ARRAY_PARTITION variable=b complete dim=2

	for (int k = 0; k < B_HEIGHT/GROUP_SIZE; k++) {

		for (int i = 0; i < B_HEIGHT; i++) {
			for (int j = 0; j < GROUP_SIZE; j++) {
	#pragma HLS PIPELINE
				b[i][j] = *(B+k*GROUP_SIZE+i*B_WIDTH+j);
			}
		}


		for (int p = 0; p < A_HEIGHT; p+=STEP) {
			mxv(A+p*A_WIDTH, C+p*A_WIDTH+k*GROUP_SIZE, b);
		}

	}
	return 0;
}
inline void mxv(float *A, float* C, float b[B_HEIGHT][GROUP_SIZE])  {

	float a[A_WIDTH];
	float c[GROUP_SIZE];
#pragma HLS ARRAY_PARTITION variable=c complete dim=1


	memcpy(a,(float *)(A),A_WIDTH*sizeof(float));

	for (int j = 0; j < GROUP_SIZE; j++) {
#pragma HLS UNROLL
		c[j] = 0;
	}

	for(int k = 0; k < B_HEIGHT; k+=1) {
		for (int j = 0; j < GROUP_SIZE; j++) {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor=BLOCK
			c[j] += a[k]*b[k][j];
		}
	}

	memcpy((float *)(C), c, GROUP_SIZE*sizeof(float));
	return;
}
