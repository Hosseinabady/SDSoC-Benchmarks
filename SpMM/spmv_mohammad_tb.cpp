#ifdef _WIN32
#include <cstring>
#include <conio.h>
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include "sds_lib.h"

#endif
#include "math.h"

#include <iostream>

#include "spmm_block.h"
#include "spmm_openmp.h"
#include <pthread.h>




double getTimestamp() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec + tv.tv_sec * 1e6;
}
void msleep(u32 c) {
	usleep(c * 1000);
}



void* fpga_malloc(size_t size) {
	void* result = NULL;
	result = (float*)sds_alloc(size);
	return result;
}

void fpga_free(void* ptr) {
	sds_free(ptr);
}


typedef float DATA_TYPE;


DATA_TYPE *array_values;
u32*       array_colIndices;
u32*       array_rowPtr;
DATA_TYPE *array_x;
DATA_TYPE *array_y;

DATA_TYPE * array_y_golden;

u32 row_size;
u32 col_size;
u32 nnz;


u32 ireartion_no=10;
u32 no_vectors;
float cpu_percentage;
float low_percentage;
float high_percentage;





u32 read_mtx_spmm(char* inFilename, u32* row_size, u32* col_size, u32* nnz) {

	printf("read_mtx_spmm: check point 1\n");
	FILE* fp_input;

	u32 r;
	u32 c;
	DATA_TYPE v;

	fp_input = fopen(inFilename, "r");

	u32 nnzeo_false = 0;
	if (fp_input != NULL) {
		printf("read_mtx_spmm: check point 2\n");
		char line[1000];
		while (fgets(line, sizeof line, fp_input) != NULL) {// read a line from a file
			if (line[0] != '%') {
				sscanf(line, "%d %d %d", row_size, col_size, nnz);
				std::cout << "row_size = " <<  *row_size << " col_size = " << *col_size << " nnz = " << *nnz << std::endl;
				printf("read_mtx_spmm: check point 3\n");

				{
					array_values = (DATA_TYPE*)fpga_malloc(*nnz * sizeof(DATA_TYPE));
					if (!array_values) {
						std::cout << "unable to allocate memory array_values" << std::endl;
						exit(1);
					}

					array_colIndices = (u32*)fpga_malloc(*nnz * sizeof(u32));
					if (!array_colIndices) {
						std::cout << "unable to allocate memory array_colIndices" << std::endl;
						exit(1);
					}

					array_rowPtr = (u32*)fpga_malloc((*row_size + 1) * sizeof(u32));
					if (!array_rowPtr) {
						std::cout << "unable to allocate memory array_rowPtr" << std::endl;
						exit(1);
					}

					array_x = (DATA_TYPE*)fpga_malloc(*col_size * no_vectors * sizeof(DATA_TYPE));
					if (!array_x) {
						std::cout << "unable to allocate memory array_x" << std::endl;
						exit(1);
					}

					array_y = (DATA_TYPE*)fpga_malloc(*row_size * no_vectors * sizeof(DATA_TYPE));
					if (!array_y) {
						std::cout << "unable to allocate memory array_y" << std::endl;
						exit(1);
					}

					array_y_golden = (DATA_TYPE*)malloc(*row_size * no_vectors * sizeof(DATA_TYPE));
					if (!array_y_golden) {
						std::cout << "unable to allocate memory array_y_golden" << std::endl;
						exit(1);
					}
				}

				u32 line_number = 0;
				while (fgets(line, sizeof line, fp_input) != NULL) {// read a line from a file
					if (line_number < *nnz) {

						sscanf(line, "%d %f", &c, &v);


						*(array_colIndices + line_number) = c;


						*(array_values + line_number) = v;

					}
					else {
						sscanf(line, "%d", &r);
						*(array_rowPtr + (line_number - (*nnz))) = r;
					}
					line_number++;
				}
			}
		}
	}
	else {
		perror(inFilename); //print the error message on stderr.
		exit(1);
	}

	return 0;
}


void arg_process(u32 argc, char** argv) {


	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " a-hight a-width chunk_size " << std::endl;
		exit(0);
	}
	else {

	}
}





u32 golden_spmm(DATA_TYPE * values, u32 *row_ptr, u32* col_indices, DATA_TYPE * x, u32 no_vectors, DATA_TYPE *y, u32 row_size, u32 col_size) {

	printf("gold_spmm: check point 1\n");
	u32 nvc = 0, i = 0, j = 0, rowStart = 0, rowEnd = row_size;

	DATA_TYPE y0 = 0.0;
	u32 last_j = 0;
	for (nvc = 0; nvc < no_vectors; nvc++) {
		for (i = rowStart; i < rowEnd; ++i) {
			y0 = 0.0;
			for (j = row_ptr[i]; j < row_ptr[i + 1]; ++j) {
				y0 += values[j] * x[nvc*col_size+col_indices[j]];
			}

			y[nvc*row_size+i] = y0;
		}
	}

	return 0;
}


double fpga_shared_start_time, fpga_shared_end_time, fpga_shared_execution_time;

void* fpgaThread_function(void *message) {
	fpga_shared_start_time = getTimestamp();
	spmm_block(
		array_values,
		array_colIndices,
		array_rowPtr,
		array_x+(u32)(cpu_percentage*no_vectors)*col_size,
		no_vectors-cpu_percentage*no_vectors,
		array_y+(u32)(cpu_percentage*no_vectors)*row_size,
		row_size,
		col_size,
		nnz,
		(u32)(low_percentage*row_size),
		(u32)(high_percentage*row_size)

		);
	fpga_shared_end_time = getTimestamp();
	fpga_shared_execution_time += (fpga_shared_end_time - fpga_shared_start_time) / (1000* ireartion_no);

	return message;
	}


int main(int argc, char** argv) {


	std::cout << "Hello spmm on FPGA+ARM-CPU application" << std::endl;

	if (argc != 7) {
		std::cout << "please eneter spmm sprse-matrix-file-name iterations low_percentage (a number between 0 to 100) high_percentage (a number between 0 to 100)  #vectors" << std::endl;
		exit(0);
	} else {
		ireartion_no    = atoi(argv[2]);
		cpu_percentage  = atoi(argv[3])/100.0;
		low_percentage  = atoi(argv[4])/100.0;
		high_percentage = atoi(argv[5])/100.0;
		no_vectors      = atoi(argv[6]);
		std::cout << "ireartion_no = " << ireartion_no
		          << " cpu_percentage = " << cpu_percentage
		          << " low_percentage = " << low_percentage
				  << " high_percentage = " << high_percentage
				  << " no_vectors = " << no_vectors
				  << std::endl;
	}

	arg_process(argc, argv);



	read_mtx_spmm(argv[1], &row_size, &col_size, &nnz);

	for (u32 i = 0; i < no_vectors; i++) {
		for (u32 j = 0; j < col_size; j++) {
			 DATA_TYPE t = (1.0*rand()+1.0)/RAND_MAX;
			 array_x[i*col_size+j] = t;
		}
	}
	std::cout << "main check point 1 " << std::endl;

	double start_time, end_time, execution_time;


	//======================ONLY CPU ==========================================
	 std::cout << "ireartion_no = " << ireartion_no << " no_vectors = " << no_vectors <<  std::endl;
	 std::cout << "row_size = " << row_size << " col_size = " << col_size << " nnz = "<<  nnz << std::endl;

	 start_time = getTimestamp();
	 for (u32 i = 0; i < ireartion_no; i++) {
	 	spmm_openmp(
	 		array_values,
	 		array_colIndices,
	 		array_rowPtr,
	 		array_x,
	 		no_vectors,
	 		array_y,
	 		row_size,
	 		col_size,
	 		nnz,
			0,
			row_size
	 	);

	 }
	 end_time = getTimestamp();
	 execution_time = (end_time - start_time) / (1000* ireartion_no);
	 std::cout << "ONLY CPU Total execution_time = " << execution_time << " msec" << std::endl;
	//=========================================================================
	//======================ONLY FPGA ==========================================


	start_time = getTimestamp();
	for (u32 i = 0; i < ireartion_no; i++) {
		spmm_block(
			array_values,
			array_colIndices,
			array_rowPtr,
			array_x,
			no_vectors,
			array_y,
			row_size,
			col_size,
			nnz,
			0,
			row_size);
	}
	end_time = getTimestamp();
	execution_time = (end_time - start_time) / (1000* ireartion_no);
	std::cout << "ONLY FPGA " << " Total execution_time = " << execution_time << " msec" << std::endl;


	//=========================================================================
	double cpu_shared_start_time, cpu_shared_end_time, cpu_shared_execution_time;




 	for (u32 i = 0; i < no_vectors; i++) {
 		for (u32 j = 0; j < row_size; j++) {
 			 DATA_TYPE t = (1.0*rand()+1.0)/RAND_MAX;
 			 array_y[i*row_size+j] = t;
 		}
 	}
 //==================CPU+FPGA=======================================================
 	start_time = getTimestamp();
 	pthread_t fpgaThread;
 	char fpgaMessage[1000] = "FPGA Thread";
 	cpu_shared_execution_time = 0;
 	fpga_shared_execution_time = 0;
 	for (u32 i = 0; i < ireartion_no; i++) {
 		pthread_create( &fpgaThread, NULL, fpgaThread_function, (void*) fpgaMessage);
 		//-----------------------------------------------------------------------------
 		cpu_shared_start_time = getTimestamp();

 		spmm_openmp(
 			array_values,
 			array_colIndices,
 			array_rowPtr,
			array_x+(u32)(cpu_percentage*no_vectors)*col_size,
			no_vectors-cpu_percentage*no_vectors,
			array_y+(u32)(cpu_percentage*no_vectors)*row_size,
 			row_size,
 			col_size,
 			nnz,
			0,
			(u32)(low_percentage*row_size)
			);
 		spmm_openmp(
 			array_values,
 			array_colIndices,
 			array_rowPtr,
			array_x+(u32)(cpu_percentage*no_vectors)*col_size,
			no_vectors-cpu_percentage*no_vectors,
			array_y+(u32)(cpu_percentage*no_vectors)*row_size,
 			row_size,
 			col_size,
 			nnz,
			(u32)(high_percentage*row_size),
			row_size
			);
//-------------------------------------
 		spmm_openmp(
 			array_values,
 			array_colIndices,
 			array_rowPtr,
 			array_x,
			cpu_percentage*no_vectors,
 			array_y,
 			row_size,
 			col_size,
 			nnz,
			0,
			row_size
			);

 		cpu_shared_end_time = getTimestamp();
 		cpu_shared_execution_time += (cpu_shared_end_time - cpu_shared_start_time) / (1000* ireartion_no);


 		//=========================================================================
 		pthread_join( fpgaThread, NULL);
 	}
 	end_time = getTimestamp();
 	execution_time = (end_time - start_time) / (1000* ireartion_no);
 	std::cout << "CPU+FPGA Total execution_time = " << execution_time << " msec" << std::endl;
 	std::cout << "FPGA-Shared Total execution_time = " << fpga_shared_execution_time << " msec" << std::endl;
 	std::cout << "CPU-Shared Total execution_time = " << cpu_shared_execution_time << " msec" << std::endl;
 	std::cout << "main check point 3 " << std::endl;

	golden_spmm(
		array_values,
		array_rowPtr,
		array_colIndices,
		array_x,
		no_vectors,
		array_y_golden,
		row_size,
		col_size);



    int status = 0;

    for (u32 i = 0; i < row_size*no_vectors; i++) {
        float diff = fabs(array_y[i]- array_y_golden[i]);
		//std::cout << "Data at  " << i << "array_y[" << i << "] = " << array_y[i] << " != " << "array_y_golden[" << i << "] = " << array_y_golden[i] << std::endl;
        if (diff != diff || diff > 0.1) {
            std::cout << "Error at  " << i << " array_y["<<i << "] = " << array_y[i] << " != " << " array_y_golden["<<i << "] = " << array_y_golden[i] << std::endl;
            status = -1;
            break;
        }
    }


    if (status == 0) {
        std::cout << "spmm SUCCESS!" << std::endl;
    } else {
        std::cout << "spmm FAILED!" << std::endl;
    }

	free(array_y_golden);

	fpga_free(array_values);
	fpga_free(array_colIndices);
	fpga_free(array_rowPtr);
	fpga_free(array_x);



}
