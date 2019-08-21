#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#if _WIN32 || _WIN64 
#include <windows.h>
#else 
#include <sys/time.h>
#endif




int read_spmv_file(
	int       *row_size,
	int       *col_size,
	int       *data_size,
	DATA_TYPE **cooValHostPtr,
	u32       **cooRowIndexHostPtr,
	u32       **cooColIndexHostPtr,
	DATA_TYPE **xHostPtr,
	DATA_TYPE **yHostPtr,
	char      *inputFile) {
	FILE *fp;
	char buff[1000];
	ssize_t read;
	DATA_TYPE *all_data;
	int r;
	int c;
	DATA_TYPE v;



	fp = fopen(inputFile, "r");


	if (fp != NULL) {
		char line[1000];
		while (fgets(line, sizeof line, fp) != NULL) {// read a line from a file
			if (line[0] != '%') {
				sscanf(line, "%d %d %d", row_size, col_size, data_size);


				*cooRowIndexHostPtr = (u32 *)malloc(*data_size * sizeof(u32));
				*cooColIndexHostPtr = (u32 *)malloc(*data_size * sizeof(u32));
				*cooValHostPtr = (DATA_TYPE *)malloc(*data_size * sizeof(DATA_TYPE));
				if ((!cooRowIndexHostPtr) || (!cooColIndexHostPtr) || (!cooValHostPtr)) {
					CLEANUP("Host malloc failed (matrix)");
					return 1;
				}



				int last_index = 0;

				while (fgets(line, sizeof line, fp) != NULL) {// read a line from a file
					sscanf(line, "%d %d %f", &r, &c, &v);
					if (v == 0) {
//						printf("r = %d col = %d val=%lf\n", r, c, v);
					}
					(*cooRowIndexHostPtr)[last_index] = r;
					(*cooColIndexHostPtr)[last_index] = c;
					(*cooValHostPtr)[last_index] = v;
					last_index++;
				}
			}
		}
		fclose(fp);
	}
	else {
		perror(inputFile); //print the error message on stderr.
	}




	*xHostPtr = (DATA_TYPE *)malloc(1 * *col_size * sizeof(DATA_TYPE));

	for (int i = 0; i < *col_size; i++) {
		(*xHostPtr)[i] = i;//(1.0*rand()+1.0)/RAND_MAX;
	}


	/* create a sparse and dense vector */

	if ((!yHostPtr)) {
		CLEANUP("Host malloc failed (vectors)");
		return 1;
	}

	*yHostPtr = (DATA_TYPE *)malloc(1 * *row_size * sizeof(DATA_TYPE));
	if ((!xHostPtr)) {
		CLEANUP("Host malloc failed (vectors)");
		return 1;
	}

	for (int i = 0; i < *row_size; i++) {
		(*yHostPtr)[i] = 0;//(1.0*rand()+1.0)/RAND_MAX;
	}


}




void SpMV_Ref(
	int       row_size,
	int       col_size,
	int       data_size,
	DATA_TYPE *cooValHostPtr,
	u32       *cooRowIndexHostPtr,
	u32       *cooColIndexHostPtr,
	DATA_TYPE *xHostPtr,
	DATA_TYPE *y_goldHostPtr
) {

	for (int i = 0; i < data_size; i++) {
		int r = cooRowIndexHostPtr[i];
		int c = cooColIndexHostPtr[i];
		DATA_TYPE d = cooValHostPtr[i];


		y_goldHostPtr[r] += d*xHostPtr[c];
	}

}



#if _WIN32 || _WIN64 
double getTimestamp() {
	SYSTEMTIME st;

	GetSystemTime(&st);

	return st.wMilliseconds*1000;
}
#else 
double getTimestamp() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec + tv.tv_sec*1e6;
}

#endif
