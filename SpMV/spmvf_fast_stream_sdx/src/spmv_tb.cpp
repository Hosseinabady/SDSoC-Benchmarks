#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include "spmv.h"
#include "time.h"
#include "sds_lib.h"


#include <sys/time.h>

double getTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec + tv.tv_sec*1e6;
}


double hardware_start;
double hardware_end;
double hardware_execution_time;

double software_start;
double software_end;
double software_execution_time;

void SpMV_Ref(char* inputFile, DATA_TYPE *y) {

	FILE *fp;




	DATA_TYPE v;

	u32 row_size;
	u32 col_size;
	u32 data_size;

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
					   printf("SpMV_Ref: error reading file 1\n");
				   }
			   }
			   for (u32 i = 0; i < data_size; i++) {
				   if(fgets(line,sizeof line,fp)!= NULL) {
					   sscanf( line, "%d %f", &c, &v);
					   cols[i] = c;
					   values[i] = v;
				   } else {
					   printf("SpMV_Ref: error reading file 2\n");
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
}

void spmv_init( DATA_TYPE **values, DATA_TYPE **x, u32 **cols, u32 **rows, u32 *row_size, u32 *col_size, u32 *data_size, char* inputFile) {

	FILE *fp;

	DATA_TYPE v;

	fp = fopen(inputFile, "r");

	if (fp != NULL) {
	   char line [1000];
	   while(fgets(line,sizeof line,fp)!= NULL) {// read a line from a file
		   if (line[0] != '%') {
			   printf("check point spmv_init:05\n");
			   unsigned int r, c, d;
			   sscanf( line, "%u %u %u", &r, &c, &d);
			   // sscanf( line, "%lu %lu %lu", row_size, col_size, data_size);
			   *row_size = r;
			   *col_size = c;
			   *data_size = d;


			   printf("line = %s\n", line);
			   std::cout << "\r row_size = " << *row_size << std::endl;
			   std::cout << "\r col_size = " << *col_size << std::endl;
			   std::cout << "\r data_size = " << *data_size << std::endl;
			   std::cout << "check point spmv_init:06\n" << std::endl;

			   *x        = (DATA_TYPE*)sds_alloc_non_cacheable(*col_size*sizeof(DATA_TYPE));
			   *rows     = (u32*)sds_alloc_non_cacheable(*row_size*sizeof(u32));
			   *values   = (DATA_TYPE*)sds_alloc_non_cacheable(*data_size*sizeof(DATA_TYPE));
			   *cols     = (u32*)sds_alloc_non_cacheable(*data_size*sizeof(u32));

			   for (u32 i = 0; i < (*row_size); i++) {
				   if(fgets(line,sizeof line,fp)!= NULL) {
					   sscanf( line, "%d ", &r);
					   (*rows)[i] = r;
				   } else {
					   printf("spmv_init: error reading file 1\n");
				   }
			   }
			   for (u32 i = 0; i < *data_size; i++) {
				   if(fgets(line,sizeof line,fp)!= NULL) {
					   sscanf( line, "%d %f", &c, &v);
					   (*cols)[i] = c;
					   (*values)[i] = v;
				   } else {
					   printf("spmv_init: error reading file 2\n");
				   }
			   }
			   break;
		   }
	   }

	   fclose(fp);
	 }
	 else {
	    perror(inputFile); //print the error message on stderr.
	 }



    for (u32 i = 0; i < *col_size; i++) {
    	(*x)[i] = i;//(1.0*rand()+1.0)/RAND_MAX;
    }





}



int main(int argc, char **argv) {

	int status = 0;
	DATA_TYPE *x = NULL,  *y_sw = NULL, *y_hw = NULL, *values = NULL;
	u32    *rows = NULL, *cols = NULL;
	u32     row_size, col_size, data_size;


	if (argc != 3) {
		printf ("Please enter the number of iterations and matrix file name \n");
		exit(1);
	}


	char II_string[10];//="4";

	sprintf(II_string, "%d", II);

	char* inputFile_name;
	char inputFile[1000]= "";
	char fpgaFileName[1000]= "";
	char goldenFileName[1000]= "";


	strcpy(inputFile, argv[2]);

	int no_iterations = atoi(argv[1]);


	char* ts1 = strdup(inputFile);
	char* ts2 = strdup(inputFile);

	char* dir = dirname(ts1);
	inputFile_name = basename(ts2);

	if(strstr(inputFile_name, ".mtx") != NULL) {
		printf("please remove the file extension");
		exit(1);
	}

	printf("dir=%s, filename=%s, \n", dir, inputFile_name);


	strcpy(fpgaFileName, dir);
	strcat(fpgaFileName, "/");
	strcat(fpgaFileName, "padded_II=");
	strcat(fpgaFileName, II_string);
	strcat(fpgaFileName, inputFile_name);
	strcat(fpgaFileName, ".mtx");


	strcpy(goldenFileName, dir);
	strcat(goldenFileName, "/");
	strcat(goldenFileName, "fpga_csr_");
	strcat(goldenFileName, inputFile_name);
	strcat(goldenFileName, ".mtx");

//	char fpgaFileName[1000] = "padded_fpga_csr_cusparse_";
//	strcat(fpgaFileName, argv[1]);
//	char goldenFileName[1000] = "fpga_csr_cusparse_";
//	strcat(goldenFileName, argv[1]);

	spmv_init( &values, &x, &cols, &rows, &row_size, &col_size, &data_size, fpgaFileName);
	y_sw        = (DATA_TYPE*)malloc(row_size*sizeof(DATA_TYPE));
	y_hw        = (DATA_TYPE*)sds_alloc_non_cacheable(row_size*sizeof(DATA_TYPE));







	printf("\rHello MM!\n\r");




	SpMV_Ref(goldenFileName, y_sw);

	printf("\rHardware version started!\n\r");


	hardware_start = getTimestamp();
	spmv_accel(values, cols, rows, x, y_hw, row_size, col_size, data_size);
    hardware_end = getTimestamp();


	hardware_execution_time = (hardware_end-hardware_start)/(1000);
	printf("first time hardware execution time  %.6f ms elapsed\n", hardware_execution_time);

	printf("\rHardware version started!\n\r");
	hardware_start = getTimestamp();
	spmv_accel(values, cols, rows, x, y_hw, row_size, col_size, data_size);
    hardware_end = getTimestamp();


	hardware_execution_time = (hardware_end-hardware_start)/(1000);
	printf("second hardware execution time  %.6f ms elapsed\n", hardware_execution_time);


	printf("\rHardware version started!\n\r");
	hardware_start = getTimestamp();

	for (int i = 0; i < no_iterations; i++) {
		spmv_accel(values, cols, rows, x, y_hw, row_size, col_size, data_size);
	}
    hardware_end = getTimestamp();


	hardware_execution_time = (hardware_end-hardware_start)/(1000*no_iterations);
	printf("average hardware execution time  %.6f ms elapsed\n", hardware_execution_time);


	for(u32 i=0;i<row_size;i++) {
		DATA_TYPE diff = fabs(y_sw[i]-y_hw[i]);
		if(diff > 0.1 || diff != diff){
			std::cout << "error occurs at " << i << " with value y_ hw = " <<  y_hw[i] << ", should be y_ sw = " << y_sw[i] << std::endl;
			status = -1;
			break;
	    }


	}
	if(!status) {
		printf("Validation PASSED!\n");
	} else {
		printf("Validation FAILED!\n");
	}


	printf("\rBye mm!\n\r");

    return 0;
}


