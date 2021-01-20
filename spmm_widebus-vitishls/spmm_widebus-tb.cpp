#include "spmm_widebus-tb.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

DATA_TYPE *array_values;
u32*       array_colIndices;
u32*       array_rowPtr;
DATA_TYPE *array_x;
DATA_TYPE_OUT *array_y;


DATA_TYPE_OUT * array_y_golden;
u32 row_size;
u32 col_size;
u32 nnz;

u32 no_vector;


u32 read_mtx_spmm(
		u32  *row_size,
		u32  *col_size,
		u32  *nnz,
		char *input_data_file_name) {



	printf("read_csr: check point 1\n");


	u32 r;
	u32 c;
	DATA_TYPE v;

	char* inFilename = input_data_file_name;

	std::ifstream file(inFilename, std::ifstream::in);
	std::string str;

	u32 nnzeo_false = 0;
	if (file.is_open()) {
		printf("read_mtx_spmm: check point 2\n");
		char line[1000];

		while (std::getline(file, str)) {// read a line from a file
			std::cout << str << std::endl;

			if (str[0] != '%') {
				std::stringstream ss;
				ss << str;
				ss >> *row_size;
				ss >> *col_size;
				ss >> *nnz;
				std::cout << "row_size = " <<  *row_size << " col_size = " << *col_size << " nnz = " << *nnz << std::endl;
				printf("read_mtx_spmm: check point 3\n");

//				{
//					array_values = (DATA_TYPE*)malloc((*nnz+ (NO_THREAD-(*nnz)%NO_THREAD)) * sizeof(DATA_TYPE));
//					if (!array_values) {
//						std::cout << "unable to allocate memory array_values" << std::endl;
//						exit(1);
//					}
//
//					array_colIndices = (u32*)malloc((*nnz+ (NO_THREAD-(*nnz)%NO_THREAD)) * sizeof(u32));
//					if (!array_colIndices) {
//						std::cout << "unable to allocate memory array_colIndices" << std::endl;
//						exit(1);
//					}
//
//					for (int i = 0; i < (*nnz+ (NO_THREAD-(*nnz)%NO_THREAD)); i++) {
//							array_values[i] = 0;
//							array_colIndices[i] = 0;
//					}
//
//					array_rowPtr = (u32*)malloc((*row_size + 1) * sizeof(u32));
//					if (!array_rowPtr) {
//						std::cout << "unable to allocate memory array_rowPtr" << std::endl;
//						exit(1);
//					}
//
//					array_x = (DATA_TYPE*)malloc(*col_size * no_vector * sizeof(DATA_TYPE));
//					if (!array_x) {
//						std::cout << "unable to allocate memory array_x" << std::endl;
//						exit(1);
//					}
//
//					array_y = (DATA_TYPE_OUT*)malloc(*row_size * no_vector * sizeof(DATA_TYPE_OUT));
//					if (!array_y) {
//						std::cout << "unable to allocate memory array_y" << std::endl;
//						exit(1);
//					}
//
//					array_y_golden = (DATA_TYPE_OUT*)malloc(*row_size * no_vector * sizeof(DATA_TYPE_OUT));
//					if (!array_y_golden) {
//						std::cout << "unable to allocate memory array_y_golden" << std::endl;
//						exit(1);
//					}
//				}

				u32 line_number = 0;
				line[0]='\0';

				while (std::getline(file, str)) {

					std::stringstream ss;
					ss << str;
					if (line_number < *nnz) {


						ss >> c;
						ss >> v;

						*(array_colIndices + line_number) = c;


						*(array_values + line_number) = v;

					}
					else {

						ss >> r;
						*(array_rowPtr + (line_number - (*nnz))) = r;
					}
					line_number++;
				}
			}
		}

		file.close();
	}
	else {
		perror(inFilename); //print the error message on stderr.
		exit(1);
	}

	return 0;
}


void golden_spmm(
		u32     row_size,
		u32     col_size,
		u32     no_vector,
		int  *value,
		u32    *col_index,
		u32    *row_index,
		int  *x,
		int  *y)
{
	for (int t = 0; t < no_vector; t++) {
		u32 rowStart = 0, rowEnd = row_size;
		for (u32 i = rowStart; i < rowEnd; ++i) {
			int y0 = 0.0;
			for (u32 j=row_index[i]; j<row_index[i+1]; j++) {
				u32 k = col_index[j];
				y0 += value[j] * x[t*col_size + k];
			}
			y[t*row_size+i] = y0;
		}
	}
}



int main(int argc, char** argv) {

	int status = 0;

	std::cout << "spmv running" << std::endl;

	if (argc != 2) {
		std::cout << "please enter the input data file name" << std::endl;
	}

	no_vector = P;



	array_values = (DATA_TYPE*)malloc((NNZ+ (NO_THREAD-(NNZ)%NO_THREAD)) * sizeof(DATA_TYPE));
	if (!array_values) {
		std::cout << "unable to allocate memory array_values" << std::endl;
		exit(1);
	}

	array_colIndices = (u32*)malloc((NNZ+ (NO_THREAD-(NNZ)%NO_THREAD)) * sizeof(u32));
	if (!array_colIndices) {
		std::cout << "unable to allocate memory array_colIndices" << std::endl;
		exit(1);
	}

	array_rowPtr = (u32*)malloc((N + 1) * sizeof(u32));
	if (!array_rowPtr) {
		std::cout << "unable to allocate memory array_rowPtr" << std::endl;
		exit(1);
	}

	array_x = (DATA_TYPE*)malloc(M * P * sizeof(DATA_TYPE));
	if (!array_x) {
		std::cout << "unable to allocate memory array_x" << std::endl;
		exit(1);
	}

	array_y = (DATA_TYPE_OUT*)malloc(N * P * sizeof(DATA_TYPE_OUT));
	if (!array_y) {
		std::cout << "unable to allocate memory array_y" << std::endl;
		exit(1);
	}

	array_y_golden = (DATA_TYPE_OUT*)malloc(N * P * sizeof(DATA_TYPE_OUT));
	if (!array_y_golden) {
		std::cout << "unable to allocate memory array_y_golden" << std::endl;
		exit(1);
	}


	read_mtx_spmm(&row_size, &col_size, &nnz, argv[1]);

	std::cout << "check point 1" << std::endl;
	for (u32 i = 0; i < no_vector; i++) {
		for (u32 j = 0; j < col_size; j++) {
			DATA_TYPE t = (1.0*rand()+1.0)/RAND_MAX;
			array_x[i*col_size+j] = t;//0xFFFFFFFF;
		}
	}

	std::cout << "check point 2" << std::endl;

	spmm_widebus(
			array_rowPtr,

			(DATA_TYPE_WB*)array_values,
			(DATA_TYPE_WB*)array_colIndices,

			array_x,
			array_y,

			no_vector,
			col_size,
			row_size,
			nnz);

	std::cout << "check point 3" << std::endl;
	golden_spmm(
		row_size,
		col_size,
		no_vector,
		array_values,
		array_colIndices,
		array_rowPtr,
		array_x,
		array_y_golden
	);


    for (u32 i = 0; i < no_vector*row_size; i++) {
        int diff = fabs(array_y[i]- array_y_golden[i]);

        if (diff > 0.1 || diff != diff) {
            std::cout << "Error at  " << i << " array_y["<<i << "] = " << array_y[i] << " != " << " array_y_golden["<<i << "] = " << array_y_golden[i] << std::endl;
            status = -1;
            if ( i > 10 )
            	break;
        }
    }


    if (status == 0) {
        std::cout << "spmv SUCCESS!" << std::endl;
    } else {
        std::cout << "spmv FAILED!" << std::endl;
    }

	free(array_y_golden);

	free(array_values);
	free(array_colIndices);
	free(array_rowPtr);
	free(array_x);


	return status;

}
