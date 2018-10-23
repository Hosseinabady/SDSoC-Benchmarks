/* File: matrix_mult_openmp_neon.c
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
* File name : matrix_mult_openmp_neon.c
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 24 November 2016 2016
* blog: https://highlevel-synthesis.com/
*/

#include "matrix_mult.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

double getTimestamp()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec + tv.tv_sec*1e6;
}


double start_time;
double end_time;
double execution_time;


#define BLOCK 8
unsigned Fast_MMM(float *a, float *b, float *c)
{
	int i_m,j_m,k_m,i_block,j_block,k_block;
	float *c_p, *b_p, *a_p;
#pragma omp parallel shared(a,b,c) private(i_m,j_m,k_m,i_block,j_block,k_block)
	{
#pragma omp for  schedule(static)
	for (i_m = 0; i_m < N; i_m += BLOCK) {
	    for (j_m = 0; j_m < P; j_m += BLOCK) {
	        for (k_m = 0; k_m < M; k_m += BLOCK) {
	        	c_p = c+i_m*P+j_m;
	        	a_p = a+i_m*M+k_m;
	            for (i_block = 0; i_block < BLOCK; i_block++ ) {
	            	b_p = b+k_m*P+j_m;
	                for (j_block = 0; j_block < BLOCK; j_block++) {
	                    for (k_block = 0; k_block < BLOCK; k_block++) {
	                        c_p[k_block] += a_p[j_block] * b_p[k_block];
	                    }
	                    b_p += P;
	                }
	                c_p += P;
	                a_p += M;
	            }
	        }
	    }
	}
	}

	return 1;
}



unsigned golden_MMM(float *a, float *b, float *c)
{
	int i,j,k;

	for(i=0;i<N;i++)
		for(j=0;j<P;j++)
			for(k=0;k<M;k++)
		        c[i*P+j] += a[i*M+k]*b[k*P+j];
	return 1;
}


int main()
{
	printf("Hello Large MM\n");
	int status = 0;
	unsigned int i,j;
	int iRetCode;

	float *a, *b, *c, *c_golden;

	a = (float *)malloc(N*M*sizeof(float));
	b = (float *)malloc(M*P*sizeof(float));
	c = (float *)malloc(N*P*sizeof(float));
	c_golden=(float *)malloc(N*P*sizeof(float));

	printf("Matrix size= %d * %d\r\n", N, N);


	for(i=0;i<N;i++){
		for(j=0;j<M;j++) {
			float t = rand()/(1.0*RAND_MAX);
		    a[i*M+j]=t;
		}
	}

	for(i=0;i<M;i++) {
		for(j=0;j<P;j++) {
			float t = rand()/(1.0*RAND_MAX);
		    b[i*P+j]=t;
		}
	}


	for(i=0;i<N;i++)
		for(j=0;j<P;j++) {
			float t = rand()/(1.0*RAND_MAX);
		    c[i*P+j]=0;
		}




	start_time = getTimestamp();
	Fast_MMM(a, b, c);
	end_time = getTimestamp();
	execution_time = (end_time-start_time)/(1000);
	printf("Fast MM execution time  %.6lf ms elapsed\n", execution_time);

	start_time = getTimestamp();
	golden_MMM(a, b, c_golden);
	end_time = getTimestamp();
	execution_time = (end_time-start_time)/(1000);
	printf("Golden MM execution time  %.6lf ms elapsed\n", execution_time);


	for(i = 0; i < N*P; i++) {
		float fc = c[i];
		float gc = c_golden[i];
		float diff = fabs(fc-gc);
		if (diff > 0.001 || diff != diff) {
			printf("Error at %d Fast_MMM=%f  Golden MMM=%f\n", i, fc, gc);
			status = -1;
			break;
		}
	}

	if (!status) {
		printf("Evaluation PASSED\n");
	} else {
		printf("Evaluation FAILED\n");
	}

    printf("By Large MM\n");

	return 0;
}
