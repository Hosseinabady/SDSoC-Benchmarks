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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include "time.h"
#include <sys/time.h>


extern "C" {
#include "monitoring.h"
}

extern char log_file_name[1000];
FILE* fp_v;



double getTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec + tv.tv_sec*1e6;
}


double software_start;
double software_end;
double software_execution_time;

double software_execution_time_one_iteration;

double software_execution_time_min;
double software_execution_time_max;


void SpMV_Ref(char* inputFile, DATA_TYPE *y) {

	FILE *fp;


	DATA_TYPE v;

	u32 row_size = 0;
	u32 col_size = 0;
	u32 data_size = 0;

	DATA_TYPE *x;
	u32       *rows;
	DATA_TYPE *values;
	u32       *cols;

	fp = fopen(inputFile, "r");


	if (fp != NULL) {
	   char line [1000];
	   while(fgets(line,sizeof line,fp)!= NULL) {// read a line from a file
		   if (line[0] != '%') {
			   unsigned int r, c, d;
			   sscanf( line, "%u %u %u", &r, &c, &d);
			   //sscanf( line, "%lu %lu %lu", &row_size, &col_size, &data_size);
			   row_size = r;
			   col_size = c;
			   data_size = d;
			   x        = (DATA_TYPE*)malloc(col_size*sizeof(DATA_TYPE));
			   rows     = (u32*)malloc(row_size*sizeof(u32));
			   values   = (DATA_TYPE*)malloc(data_size*sizeof(DATA_TYPE));
			   cols     = (u32*)malloc(data_size*sizeof(u32));

			   for (u32 i = 0; i < (row_size); i++) {
				   if(fgets(line,sizeof line,fp)!= NULL) {
					   sscanf( line, "%d ", &r);
					   rows[i] = r;
				   } else {
					   printf("error reading file 1-1\n");
				   }
			   }
			   for (u32 i = 0; i < data_size; i++) {
				   if(fgets(line,sizeof line,fp)!= NULL) {
					   sscanf( line, "%d %f", &c, &v);
					   cols[i] = c;
					   values[i] = v;
				   } else {
					   printf("error reading file 1-2\n");
				   }
			   }
			   break;
		   }
	   }
	   fclose(fp);
	} else {
		perror(inputFile); //print the error message on stderr.
	}


	for (u32 i = 0; i < col_size; i++) {
		x[i] = i;//(1.0*rand()+1.0)/RAND_MAX;
	}

    int i=0, j=0, rowStart=0, rowEnd=row_size;

    DATA_TYPE y0=0.0;
    int last_j = 0;
    for (i = rowStart; i < rowEnd; ++i) {
    	y0 = 0.0;
    	int new_index_j = last_j+rows[i];
        for (j = last_j ; j < new_index_j; ++j) {
        	y0 += values[j] * x[cols[j]];
        }
        last_j = last_j + rows[i];
        y[i] = y0;
    }
    free(x);
    free(rows);
    free(values);
    free(cols);
}


void spmv_openmp_init( DATA_TYPE **values, DATA_TYPE **x, u32 **cols, u32 **rows, u32 *row_size, u32 *col_size, u32 *data_size, char* inputFile) {

	FILE *fp;


	DATA_TYPE v;


	fp = fopen(inputFile, "r");

	if (fp != NULL) {
	   char line [1000];
	   while(fgets(line,sizeof line,fp)!= NULL) {// read a line from a file
		   if (line[0] != '%') {
			   unsigned int r, c, d;
			   sscanf( line, "%u %u %u", &r, &c, &d);
			   *row_size = r;
			   *col_size = c;
			   *data_size = d;


			   *x        = (DATA_TYPE*)malloc(*col_size*sizeof(DATA_TYPE));
			   *rows     = (u32*)malloc(*row_size*sizeof(u32));
			   *values   = (DATA_TYPE*)malloc(*data_size*sizeof(DATA_TYPE));
			   *cols     = (u32*)malloc(*data_size*sizeof(u32));

			   (*rows)[0] = 0;
			   for (u32 i = 0; i < (*row_size); i++) {
				   if(fgets(line,sizeof line,fp)!= NULL) {
					   sscanf( line, "%d ", &r);
					   (*rows)[i+1] = (*rows)[i] + r;
				   } else {
					   printf("error reading file 1-1\n");
				   }
			   }
			   for (u32 i = 0; i < *data_size; i++) {
				   if(fgets(line,sizeof line,fp)!= NULL) {
					   sscanf( line, "%d %f", &c, &v);
					   (*cols)[i] = c;
					   (*values)[i] = v;
				   } else {
					   printf("error reading file 1-2\n");
				   }
			   }
			   break;
		   }
	   }
	   fclose(fp);
	} else {
		perror(inputFile); //print the error message on stderr.
	}


	for (u32 i = 0; i < *col_size; i++) {
		(*x)[i] = i;//(1.0*rand()+1.0)/RAND_MAX;
	}
}

int main(int argc, char **argv) {

	printf("check point main:01\n");
	int status = 0;
	DATA_TYPE *x = NULL,  *y_sw = NULL, *y_openmp = NULL, *values = NULL;
	u32    *rows = NULL, *cols = NULL;
	u32     row_size, col_size, data_size;

	if (argc != 3) {
		printf ("Eneter the number of iterations and  matrix file name \n");
		exit(1);
	}
	printf("check point main:02\n");


	char inputFile_name[1000]= "";
	char opnmpFileName[1000]= "";
	char goldenFileName[1000]= "";

	unsigned long int iterations;
	iterations = atoi(argv[1]);

	strcpy(inputFile_name, argv[2]);

	strcpy(opnmpFileName, "../spmv_data/");
	strcat(opnmpFileName, inputFile_name);
	strcat(opnmpFileName, "/");
	strcat(opnmpFileName, "fpga_csr_");
	strcat(opnmpFileName, inputFile_name);
	strcat(opnmpFileName, ".mtx");

	strcpy(goldenFileName, "../spmv_data/");
	strcat(goldenFileName, inputFile_name);
	strcat(goldenFileName, "/");
	strcat(goldenFileName, "fpga_csr_");
	strcat(goldenFileName, inputFile_name);
	strcat(goldenFileName, ".mtx");

	printf("check point main:03\n");
	spmv_openmp_init( &values, &x, &cols, &rows, &row_size, &col_size, &data_size, opnmpFileName);
	printf("check point main:04\n");



	printf("\rHello MM!\n\r");
	y_sw            = (DATA_TYPE*)malloc(row_size*sizeof(DATA_TYPE));
	y_openmp        = (DATA_TYPE*)malloc(row_size*sizeof(DATA_TYPE));

    SpMV_Ref(goldenFileName, y_sw);

	printf("\rSoftware version started!\n\r");

	strcpy(log_file_name, "log_power_monitor_");
	file_power_profile_create(log_file_name);
	fp_v=fopen(log_file_name, "a");

	software_execution_time_min = 1000;
	software_execution_time_max = 0;

	power_monitoring_prologue();


	for (unsigned long int i = 0; i < iterations; i++) {
		software_start = getTimestamp();
		spmv_openmp(y_openmp, values, cols, rows, x, row_size, col_size, data_size);
		software_end = getTimestamp();
		software_execution_time_one_iteration = (software_end - software_start) / (1000);

		if (software_execution_time_one_iteration < software_execution_time_min)
			software_execution_time_min = software_execution_time_one_iteration;
		if (software_execution_time_one_iteration > software_execution_time_max)
			software_execution_time_max = software_execution_time_one_iteration;

		software_execution_time+=software_execution_time_one_iteration;
	}
	

	power_monitoring_epilogue();
	fclose(fp_v);
	printf("-------------------------------------------\n");
	printf("max software execution time for %s  %.6lf ms elapsed\n", inputFile_name, software_execution_time_max);
	printf("min software execution time for %s  %.6lf ms elapsed\n", inputFile_name, software_execution_time_min);
	printf("ave software execution time for %s  %.6lf ms elapsed\n", inputFile_name, software_execution_time/iterations);
	printf("-------------------------------------------\n");

	for(u32 i=0;i<row_size;i++) {
		DATA_TYPE diff = fabs(y_sw[i]-y_openmp[i]);
		if(diff > 0.0001 || diff != diff){
			std::cout << "error occurs at " << i << " with value y_openmp = " <<  y_openmp[i] << ", should be y_sw = " << y_sw[i] << std::endl;
			status = -1;
			break;
	    }
		//std::cout << "data occurs at " << i << " with value y_ openmp = " <<  y_openmp[i] << ", should be y_ sw = " << y_sw[i] << std::endl;

	}
	if(!status) {
		printf("Validation PASSED!\n");
	} else {
		printf("Validation FAILED!\n");
	}


    free(x);
    free(rows);
    free(values);
    free(cols);
    free(y_openmp);

    free(y_sw);
	printf("\rBye mm!\n\r");

    return 0;
}








