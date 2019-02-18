/* File: spmv_mohammad_tb.h
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
* File name : spmv_mohammad_tb.h
* Author    : Mohammad Hosseinabady mohammad@hosseinabady.com
* Date      : 14 September 2018
* Blog      : https://highlevel-synthesis.com/
* Project   : ENEAC project at Bristol University (Jose Nunez-Yanez)
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
int       *col_indices;
int       *row_ptr;
DATA_TYPE *x;
DATA_TYPE *y_fpga;
DATA_TYPE *y_gold;


int row_size = 2000;
int col_size = 2000;
int nnz      = 0;

#define M 1



int read_mtx_SpMV(char* inFilename, int *row_size, int *col_size, int *nnz) {


	DATA_TYPE *cooValHostPtr = 0;
	int       *cooRowIndexHostPtr = 0;
	int       *cooColIndexHostPtr = 0;


	FILE *fp_input;

	int r;
	int c;
	DATA_TYPE v;


	fp_input = fopen(inFilename, "r");

	int nnzeo_false = 0;
	if (fp_input != NULL) {
		char line[1000];
		while (fgets(line, sizeof line, fp_input) != NULL) {// read a line from a file
			if (line[0] != '%') {
				sscanf(line, "%d %d %d", row_size, col_size, nnz);

				cooRowIndexHostPtr = (int *)malloc(*nnz * sizeof(int));
				cooColIndexHostPtr = (int *)malloc(*nnz * sizeof(int));
				cooValHostPtr = (DATA_TYPE *)malloc(*nnz * sizeof(DATA_TYPE));
				if ((!cooRowIndexHostPtr) || (!cooColIndexHostPtr) || (!cooValHostPtr)) {
					printf("Host malloc failed (matrix)\n");
					return 1;
				}


				int line_number = 0;
				while (fgets(line, sizeof line, fp_input) != NULL) {// read a line from a file

					sscanf(line, "%d %d %f", &r, &c, &v);
					if (v == 0) {
						//						printf("r = %d col = %d val=%lf\n", r, c, v);
						nnzeo_false++;
						continue;
					}

					r--;
					c--;
					//find row place
					(cooRowIndexHostPtr)[line_number] = r;
					(cooColIndexHostPtr)[line_number] = c;
					(cooValHostPtr)[line_number] = v;
					line_number++;
				}
			}
		}
	} else {
		perror(inFilename); //print the error message on stderr.
		exit(1);
	}

	*nnz = *nnz - nnzeo_false;

	int *cooRowIndexHostNewPtr = (int *)malloc(sizeof(int) * *nnz);
	int *cooColIndexHostNewPtr    = (int *)malloc(sizeof(int) * *nnz);
	DATA_TYPE *cooValHostNewPtr = (DATA_TYPE *)malloc(sizeof(DATA_TYPE) * *nnz);



	int index = 0;
	for (int i = 0; i < *row_size; i++) {
		for (int j = 0; j < *nnz; j++) {
			if (cooRowIndexHostPtr[j] == i) {
				cooRowIndexHostNewPtr[index] = cooRowIndexHostPtr[j];
				cooColIndexHostNewPtr[index] = cooColIndexHostPtr[j];
				cooValHostNewPtr[index] = cooValHostPtr[j];
				index++;
			}
		}
	}

//	for (int i = 0; i < *nnz; i++) {
//		printf("%d %d %f\n ", cooRowIndexHostNewPtr[i], cooColIndexHostNewPtr[i], cooValHostNewPtr[i]);
//	}


	int d = 0;
	int r_index = 0;
	for (r_index = 0; r_index < *row_size; r_index++) {
		int nonzero_line = 0;
		for (; d < *nnz; d++) {
			int current_row_index = cooRowIndexHostNewPtr[d];
			if (current_row_index == r_index) {
				row_ptr[r_index] = d;
				nonzero_line = 1;
				break;
			}
		}
		if (nonzero_line==0) {
			row_ptr[r_index] = row_ptr[r_index-1];
		}
	}
	row_ptr[r_index]=*nnz;
	for (int i = 0; i < *nnz; i++) {
		col_indices[i] = cooColIndexHostNewPtr[i];
		values[i]      = cooValHostNewPtr[i];
	}



//	for (int i = 0; i < *row_size; i++) {
//		printf("row_ptr[%d]=%d\n", i, row_ptr[i]);
//	}

	free(cooValHostPtr);
	free(cooRowIndexHostPtr);
	free(cooColIndexHostPtr);
	free(cooRowIndexHostNewPtr);

}

int generateSpMV(int row_size, int col_size, int *nnz) {

	int rand_index[II*M];

	int v_i = 0;
	int r_i = 0;
	int index_not_found = 1;



	for (int ix = 0; ix < row_size; ix++) {

		row_ptr[r_i] = v_i;
		int r_tmp = 0;
		for (int i = 0; i < II*M; i++) {
			index_not_found = 1;
			while(index_not_found) {
				int rand_col_index = rand()%col_size;
				index_not_found = 0;
				for (int s = 0; s < r_tmp; s++) {
					if (rand_index[s] == rand_col_index) {
						index_not_found = 1;
						break;
					}
				}
				if (index_not_found == 0)
					rand_index[r_tmp++] = rand_col_index;
			}
		}

		for (int i = 0; i < II*M; i++) {
			DATA_TYPE r = (10.0*(rand()+1)/RAND_MAX);
			values[v_i] = r;
			col_indices[v_i++] = rand_index[i];
		}
		r_i++;
	}

	*nnz = v_i--;
	row_ptr[r_i]=*nnz;
	printf("nnz = %d\n", *nnz);
	return 0;
}



int gold_spmv() {

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

	if (argc != 2) {
		printf("please enter the input file name  \n");
		exit(1);
	}

	values      = (DATA_TYPE *)sds_alloc_non_cacheable(NNZ_MAX*sizeof(DATA_TYPE));
	col_indices = (int *)sds_alloc_non_cacheable(NNZ_MAX*sizeof(int));
	row_ptr     = (int *)sds_alloc_non_cacheable((ROW_SIZE_MAX+1)*sizeof(int));
	x           = (DATA_TYPE *)sds_alloc_non_cacheable(COL_SIZE_MAX*sizeof(DATA_TYPE));
	y_gold      = (DATA_TYPE *)malloc(ROW_SIZE_MAX*sizeof(DATA_TYPE));
	y_fpga      = (DATA_TYPE *)sds_alloc_non_cacheable(ROW_SIZE_MAX*sizeof(DATA_TYPE));


//	generateSpMV(row_size, col_size, &nnz);

	read_mtx_SpMV(argv[1], &row_size, &col_size, &nnz);

	for (int i = 0; i < col_size; i++) {
		x[i] = (1.0*rand()+1.0)/RAND_MAX;
	}

/*
	for(int i = 0; i < row_size+1; i++) {
		printf("row_ptr=%d\n", row_ptr[i]);
	}

	for(int i = 0; i < nnz; i++) {
		printf("col_indices=%d, values=%f \n", col_indices[i], values[i]);
	}

	for(int i = 0; i < row_size; i++) {
		printf("y_gold=%f \t y_fpga=%f\n", y_gold[i], y_fpga[i]);
	}
*/
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
			row_size,
			row_size,
			nnz);


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

