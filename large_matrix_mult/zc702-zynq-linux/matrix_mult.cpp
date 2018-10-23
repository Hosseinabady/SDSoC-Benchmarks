/* File: matrix_mult.cpp
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
* File name : matrix_mult.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 1 October 2016
* blog: https://highlevel-synthesis.com/
*/

#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>
#include "sds_lib.h"
#include "matrix_mult.h"


double getTimestamp();

double hardware_start;
double hardware_end;
double hardware_time;

double software_start;
double software_end;
double software_time;



static void init_arrays(float *A,  float *B, float *C_sw, float *C) {
	for (int i = 0; i < A_HEIGHT; i++) {
		for (int j = 0; j < A_WIDTH; j++) {
			A[i * A_WIDTH + j] = rand() / (1.0*RAND_MAX);
        }
    }

    for (int i = 0; i < B_HEIGHT; i++) {
    	for (int j = 0; j < B_WIDTH; j++) {
            B[i * B_WIDTH + j] = rand() / (1.0*RAND_MAX);
        }
    }
    for (int i = 0; i < C_HEIGHT; i++) {
    	for (int j = 0; j < C_WIDTH; j++) {
            C_sw[i * C_WIDTH + j] = 0.0;
            C[i * C_WIDTH + j] = 0.0;
        }
    }
}

void mmult_golden(float *A,  float *B, float *C) {
	for (int row = 0; row < C_HEIGHT; row++) {
		for (int col = 0; col < C_WIDTH; col++) {
			float result = 0.0;
            for (int k = 0; k < A_WIDTH; k++) {
            	result += A[row*A_WIDTH+k] * B[k*B_WIDTH+col];
            }
            C[row*C_WIDTH+col] = result;
          }
     }
}


static int result_check(float *C, float *C_sw) {
    for (int i = 0; i < C_HEIGHT * C_WIDTH; i++) {
    	if (C_sw[i] != C[i]) {
    		std::cout << "Error at index=" << i << " sw = " << C_sw[i]
                      << ", hw=" << C[i] << std::endl;
            return 1;
    	 }
	}
     return 0;
}



int mmult_test(float *A,  float *B, float *C_sw, float *C)
{
     std::cout << " floating point matrix multiplication " << std::endl;
 	float *A_accel, *B_accel, *C_accel;
     A_accel    = (float *)sds_alloc(A_HEIGHT_BLOCK * A_WIDTH * sizeof(float));
     B_accel    = (float *)sds_alloc(B_HEIGHT * B_WIDTH_BLOCK * sizeof(float));
     C_accel    = (float *)sds_alloc(C_HEIGHT_BLOCK * C_WIDTH_BLOCK * sizeof(float));

     int return_value = 0;
     

     init_arrays(A, B, C_sw, C);


     hardware_start = getTimestamp();
     for (int A_index = 0; A_index < A_HEIGHT/A_HEIGHT_BLOCK; A_index++) {
    	 for (int B_index = 0; B_index < B_WIDTH/B_WIDTH_BLOCK; B_index++) {

    		 for (int i = 0; i < A_HEIGHT_BLOCK; i++) {
				for (int j = 0; j < A_WIDTH; j++) {
					A_accel[i*A_WIDTH+j] = A[(i+A_index*A_HEIGHT_BLOCK)*A_WIDTH+j];
				}
			 }

			 for (int i = 0; i < B_HEIGHT; i++) {
				for (int j = 0; j < B_WIDTH_BLOCK; j++) {
					B_accel[i*B_WIDTH_BLOCK+j] = B[i*B_WIDTH+j+B_index*B_WIDTH_BLOCK];
				}
			 }


			 mmult_accel(A_accel, B_accel, C_accel);

			 for (int i = 0; i < C_HEIGHT_BLOCK; i++) {
				for (int j = 0; j < C_WIDTH_BLOCK; j++) {
					C[(i+A_index*A_HEIGHT_BLOCK)*C_WIDTH+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
				}
			 }

    	 }
     }

     hardware_end = getTimestamp();
     hardware_time = (hardware_end-hardware_start)/1000;
     std::cout << "Exeution time running matrix multiplication in hardware: "
               << hardware_time << " msec " << std::endl;



     software_start = getTimestamp();
     mmult_golden(A, B, C_sw);
     software_end = getTimestamp();
     software_time = (software_end-software_start)/1000;


     std::cout << "Exeution time running matrix multiplication in software: "
               << software_time << " msec " << std::endl;



    if (result_check(C, C_sw))
    	return_value = 1;



     double speedup = (double) software_time/ (double) hardware_time;

     std::cout << "Speed up: " << speedup << std::endl;

     sds_free(A);
     sds_free(B);
     sds_free(C);


     return return_value;
}


int main(int argc, char* argv[]){

	std::cout << "Hello Matrix Mult" << std::endl;

     int validation = 0;
     float *A, *B, *C_sw, *C;


     A    = (float *)malloc(A_HEIGHT * A_WIDTH * sizeof(float));
     B    = (float *)malloc(B_HEIGHT * B_WIDTH * sizeof(float));
     C    = (float *)malloc(C_HEIGHT * C_WIDTH * sizeof(float));
     C_sw = (float *)malloc(C_HEIGHT * C_WIDTH * sizeof(float));
     
     if (!A || !B || !C || !C_sw) {
          if (A) free(A);
          if (B) free(B);
          if (C) free(C);
          if (C_sw) free(C_sw);
          return 2;
     }
     
     validation = mmult_test(A, B, C_sw, C);

     std::cout << "TEST " << (validation ? "FAILED" : "PASSED") << std::endl;

     free(A);
     free(B);
     free(C);
     free(C_sw);

     std::cout << "Bye Matrix Mult" << std::endl;
     return (validation ? -1 : 0);
     
}




double getTimestamp()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec + tv.tv_sec*1e6;
}
