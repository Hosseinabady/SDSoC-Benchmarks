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

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include "sds_lib.h"
#include "matrix_mult.h"




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

static void init_arrays(float *A,  float *B, float *C_sw, float *C)
{
     for (int i = 0; i < N; i++) {
          for (int j = 0; j < N; j++) {
               A[i * N + j] = 1+i*N+j;
               B[i * N + j] = rand() % (N * N);
               C_sw[i * N + j] = 0.0;
               C[i * N + j] = 0.0;
          }
     }
}

void mmult_golden(float *A,  float *B, float *C)
{
     for (int row = 0; row < N; row++) {
          for (int col = 0; col < N; col++) {
               float result = 0.0;
               for (int k = 0; k < N; k++) {
                    result += A[row*N+k] * B[k*N+col];
               }
               C[row*N+col] = result;
          }
     }
}

static int result_check(float *C, float *C_sw)
{
     for (int i = 0; i < N * N; i++) {
          if (C_sw[i] != C[i]) {
               std::cout << "Mismatch: data index=" << i << " d=" << C_sw[i]
                         << ", dout=" << C[i] << std::endl;
               return 1;
          }
     }
     return 0;
}




int mmult_test(float *A,  float *B, float *C_sw, float *C)
{
     std::cout  << " floating point mmult " << A_HEIGHT << "x" << A_WIDTH << std::endl;

     int return_value = 0;
     perf_counter hw_ctr, sw_ctr;
     
     init_arrays(A, B, C_sw, C);




     hw_ctr.start();
     mmult_accel(A, B, C);
     hw_ctr.stop();

     uint64_t hw_cycles = hw_ctr.avg_cpu_cycles();
     std::cout << "Average number of CPU cycles running mmult in hardware: "
               << hw_cycles << " or "<< hw_cycles/(1000*666.6) << " msec" << std::endl;



     sw_ctr.start();
     mmult_golden(A, B, C_sw);
     sw_ctr.stop();

     uint64_t sw_cycles = sw_ctr.avg_cpu_cycles();
     std::cout << "Average number of CPU cycles running mmult in software: "
               << sw_cycles  << " or "<< sw_cycles/(1000*666.6) << " msec" << std::endl;



    if (result_check(C, C_sw))
        	  return_value = 1;



     double speedup = (double) sw_cycles / (double) hw_cycles;

     std::cout << "Speed up: " << speedup << std::endl;

     return return_value;
}


int main(int argc, char* argv[]){

	std::cout << "Hello Matrix Mult" << std::endl;

     int test_passed = 0;
     float *A, *B, *C_sw, *C;

     A = (float *)sds_alloc(N * N * sizeof(float));
     B = (float *)sds_alloc(N * N * sizeof(float));
     C = (float *)sds_alloc(N * N * sizeof(float));
     C_sw = (float *)sds_alloc(N * N * sizeof(float));

     
     if (!A || !B || !C || !C_sw) {
          if (A) sds_free(A);
          if (B) sds_free(B);
          if (C) sds_free(C);
          if (C_sw) sds_free(C_sw);
          return 2;
     }
     
     test_passed = mmult_test(A, B, C_sw, C);

     std::cout << "TEST " << (test_passed ? "FAILED" : "PASSED") << std::endl;

     sds_free(A);
     sds_free(B);
     sds_free(C);
     sds_free(C_sw);

     std::cout << "Bye Matrix Mult" << std::endl;
     return (test_passed ? -1 : 0);
     
}




