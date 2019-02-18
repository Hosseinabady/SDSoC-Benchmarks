/* File: spmv_mohammad_tb.cpp
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
* File name : spmv_mohammad_tb.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 18 February 2019
* blog: https://highlevel-synthesis.com/
*/


#include <stdio.h>
#include <stdlib.h>
#include "spmv_mohammad.h"
#include <math.h>
#include <sds_lib.h>

#include <stdint.h>
#include "time.h"
#include <sys/time.h>


double getTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec + tv.tv_sec*1e6;
}


double software_start;
double software_end;
double software_execution_time;

double hardware_start;
double hardware_end;
double hardware_execution_time;



DATA_TYPE *values;
u32       *col_indices;
u32       *row_ptr;


DATA_TYPE *x;
DATA_TYPE *y_fpga;
DATA_TYPE *y_gold;





int row_size = 2000;
int col_size = 2000;
int nnz      = 0;






#define M 1



int read_mtx_SpMV(char* inFilename, int *row_size, int *col_size, int *nnz) {

	printf("read_mtx_SpMV: check point 1\n");
	FILE *fp_input;

	int r;
	int c;
	DATA_TYPE v;

	fp_input = fopen(inFilename, "r");

	int nnzeo_false = 0;
	if (fp_input != NULL) {
		printf("read_mtx_SpMV: check point 2\n");
		char line[1000];
		while (fgets(line, sizeof line, fp_input) != NULL) {// read a line from a file
			if (line[0] != '%') {
				sscanf(line, "%d %d %d", row_size, col_size, nnz);
				printf("read_mtx_SpMV: check point 3\n");

				int line_number = 0;
				while (fgets(line, sizeof line, fp_input) != NULL) {// read a line from a file
					if ( line_number < *nnz) {

						sscanf(line, "%d %f", &c, &v);

						*(col_indices+line_number) = c;

						*(values+line_number) = v;

					} else {
						sscanf(line, "%d", &r);
						*(row_ptr+(line_number-(*nnz))) = r;
					}
					line_number++;
				}
			}
		}
	} else {
		perror(inFilename); //print the error message on stderr.
		exit(1);
	}
}




int gold_spmv() {
	printf("gold_spmv: check point 1\n");
	int i=0, j=0, rowStart=0, rowEnd=row_size;
	long int k=0;
	DATA_TYPE y0=0.0;
	int last_j = 0;
	for (i = rowStart; i < rowEnd; ++i) {
		y0 = 0.0;
	    for (j = row_ptr[i] ; j < row_ptr[i+1]; ++j) {
	    	y0 += values[j] * x[col_indices[j]];
	    }

	    y_gold[i] = y0;
	 }


	return 0;
}







int main(int argc, char** argv) {

	printf("main: check point 1\n");
	if (argc != 2) {
		printf("please enter the input file name  \n");
		exit(1);
	}
	printf("main: check point 2\n");
	values      = (DATA_TYPE *)sds_alloc_non_cacheable(NNZ_MAX*sizeof(DATA_TYPE));
	col_indices = (u32 *)sds_alloc_non_cacheable(NNZ_MAX*sizeof(u32));
	row_ptr     = (u32 *)sds_alloc_non_cacheable((ROW_SIZE_MAX+1)*sizeof(u32));
	x           = (DATA_TYPE *)sds_alloc_non_cacheable(COL_SIZE_MAX*sizeof(DATA_TYPE));
	y_gold      = (DATA_TYPE *)malloc(ROW_SIZE_MAX*sizeof(DATA_TYPE));
	y_fpga      = (DATA_TYPE *)sds_alloc_non_cacheable(ROW_SIZE_MAX*sizeof(DATA_TYPE));
	printf("main: check point 3\n");



	read_mtx_SpMV(argv[1], &row_size, &col_size, &nnz);
	printf("main: check point 4\n");
	printf("main: checkpoint 3\n");

	for (int i = 0; i < col_size; i++) {
		x[i] = (1.0*rand()+1.0)/RAND_MAX;
	}

	gold_spmv();

	printf("row_size=%u, col_size=%u, nnz=%u\n", row_size, col_size, nnz);
	printf("\rHardware version started!\n\r");



	hardware_start = getTimestamp();


	spmv_mohammad(
			row_ptr,
			col_indices,
			values,
			y_fpga,
			x,
			col_size,
			row_size,
			nnz
			);


    hardware_end = getTimestamp();
    printf("\rHardware version finished!\n\r");
    hardware_execution_time = (hardware_end-hardware_start)/(1000);
	printf("Hardware execution time  %.6f ms elapsed\n", hardware_execution_time);



	int status = 0;
	for(int i=0;i<row_size;i++) {
		DATA_TYPE diff = fabs(y_gold[i]-y_fpga[i]);
		if(diff > 0.1 || diff != diff){
			printf("error occurs at  %d with value y_hw = %lf, should be y_gold = %lf \n",i,y_fpga[i],y_gold[i]);
			status = -1;
			break;
		}
	}

	if(!status) {
		printf("Hardware Validation PASSED!\n");
	} else {
		printf("Hardware Validation FAILED!\n");
		return -1;
	}




	sds_free(values);
	sds_free(col_indices);
	sds_free(row_ptr);
	sds_free(x);
	free(y_gold);
	sds_free(y_fpga);



	return status;
}

