
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include "cusparse.h"

#include "utilities.h"
#include "monitoring.h"

extern char log_file_name[1000];

double gpu_start;
double gpu_end;
double gpu_execution_time_one_iteration;
double gpu_execution_time;
double gpu_execution_time_min;
double gpu_execution_time_max;

double gpu_data_2_GPU_time;
double gpu_data_2_CPU_time;



int main(int argc, char** argv) {
	cudaError_t cudaStat1, cudaStat2, cudaStat3, cudaStat4, cudaStat5;
	cusparseStatus_t status;
	cusparseHandle_t handle = 0;
	cusparseMatDescr_t descr = 0;

	int      n, m, nnz, nnz_vector;
	DATA_TYPE * cooValHostPtr = 0;
	u32 *    cooRowIndexHostPtr = 0;
	u32 *    cooColIndexHostPtr = 0;

	DATA_TYPE * cooVal = 0;
	int *    cooRowIndex = 0;
	int *    cooColIndex = 0;
	int *    csrRowPtr = 0;
	DATA_TYPE * y = 0;
	DATA_TYPE * x = 0;
	DATA_TYPE * yHostPtr = 0;
	DATA_TYPE * xHostPtr = 0;
	DATA_TYPE * y_goldHostPtr=0;
	DATA_TYPE   dtwo     = 1.0;
	DATA_TYPE   dthree   = 0.0;



	if (argc != 3) {
		printf("enter iteration and input file name with cusparse_ prefix : \n");
		exit(1);
	}

	int iterations = atoi(argv[1]);
	printf("iterations = %d \n", iterations);
	char *filename = argv[2];

	read_spmv_file(&n, &m, &nnz, &cooValHostPtr, &cooRowIndexHostPtr, &cooColIndexHostPtr, &xHostPtr, &yHostPtr, filename);



	/* allocate GPU memory and copy the matrix and vectors into it */
	cudaStat1 = cudaMalloc((void**)&cooRowIndex, nnz * sizeof(cooRowIndex[0]));
	cudaStat2 = cudaMalloc((void**)&cooColIndex, nnz * sizeof(cooColIndex[0]));
	cudaStat3 = cudaMalloc((void**)&cooVal, nnz * sizeof(cooVal[0]));
	cudaStat4 = cudaMalloc((void**)&y, 1 * n * sizeof(y[0]));
	cudaStat5 = cudaMalloc((void**)&x, 1 * m * sizeof(x[0]));
	if ((cudaStat1 != cudaSuccess) ||
		(cudaStat2 != cudaSuccess) ||
		(cudaStat3 != cudaSuccess) ||
		(cudaStat4 != cudaSuccess) ||
		(cudaStat5 != cudaSuccess)) {
		CLEANUP("Device malloc failed");
		return 1;
	}
	gpu_start = getTimestamp();
	cudaStat1 = cudaMemcpy(cooRowIndex, cooRowIndexHostPtr,	(size_t)(nnz * sizeof(cooRowIndex[0])),	cudaMemcpyHostToDevice);
	cudaStat2 = cudaMemcpy(cooColIndex, cooColIndexHostPtr,	(size_t)(nnz * sizeof(cooColIndex[0])),	cudaMemcpyHostToDevice);
	cudaStat3 = cudaMemcpy(cooVal, cooValHostPtr, (size_t)(nnz * sizeof(cooVal[0])), cudaMemcpyHostToDevice);
	cudaStat4 = cudaMemcpy(y, yHostPtr,	(size_t)(1 * n * sizeof(y[0])),	cudaMemcpyHostToDevice);
	cudaStat5 = cudaMemcpy(x, xHostPtr,	(size_t)(1 * m * sizeof(x[0])),	cudaMemcpyHostToDevice);
	if ((cudaStat1 != cudaSuccess) ||
		(cudaStat2 != cudaSuccess) ||
		(cudaStat3 != cudaSuccess) ||
		(cudaStat4 != cudaSuccess) ||
		(cudaStat4 != cudaSuccess)) {
		CLEANUP("Memcpy from Host to Device failed");
		return 1;
	}

	gpu_end = getTimestamp();
	gpu_data_2_GPU_time = (gpu_end - gpu_start) / (1000);
	printf("gpu_data_2_GPU_time execution time  %.6lf ms elapsed\n", gpu_data_2_GPU_time);

	/* initialize cusparse library */
	status = cusparseCreate(&handle);
	if (status != CUSPARSE_STATUS_SUCCESS) {
		CLEANUP("CUSPARSE Library initialization failed");
		return 1;
	}

	/* create and setup matrix descriptor */
	status = cusparseCreateMatDescr(&descr);
	if (status != CUSPARSE_STATUS_SUCCESS) {
		CLEANUP("Matrix descriptor initialization failed");
		return 1;
	}
	cusparseSetMatType(descr, CUSPARSE_MATRIX_TYPE_GENERAL);
	cusparseSetMatIndexBase(descr, CUSPARSE_INDEX_BASE_ZERO);


	/* exercise conversion routines (convert matrix from COO 2 CSR format) */
	cudaStat1 = cudaMalloc((void**)&csrRowPtr, (n + 1) * sizeof(csrRowPtr[0]));
	if (cudaStat1 != cudaSuccess) {
		CLEANUP("Device malloc failed (csrRowPtr)");
		return 1;
	}
	status = cusparseXcoo2csr(handle, cooRowIndex, nnz, n,	csrRowPtr, CUSPARSE_INDEX_BASE_ZERO);
	if (status != CUSPARSE_STATUS_SUCCESS) {
		CLEANUP("Conversion from COO to CSR format failed");
		return 1;
	}


	gpu_execution_time=0;

	gpu_execution_time_min = 1000;
	gpu_execution_time_max = 0;

	FILE* fp_v;

	strcpy(log_file_name, "log_power_monitor_");
	file_power_profile_create(log_file_name);
	fp_v=fopen(log_file_name, "a");
	create_devices();
	power_monitoring_prologue();
	/* exercise Level 2 routines (csrmv) */
	for (int i = 0; i < iterations; i++) {
		gpu_start = getTimestamp();
//  		cudaStat1 = cudaMemcpy(cooRowIndex, cooRowIndexHostPtr,	(size_t)(nnz * sizeof(cooRowIndex[0])),	cudaMemcpyHostToDevice);
//		cudaStat2 = cudaMemcpy(cooColIndex, cooColIndexHostPtr,	(size_t)(nnz * sizeof(cooColIndex[0])),	cudaMemcpyHostToDevice);
//		cudaStat3 = cudaMemcpy(cooVal, cooValHostPtr, (size_t)(nnz * sizeof(cooVal[0])), cudaMemcpyHostToDevice);
//		cudaStat4 = cudaMemcpy(y, yHostPtr,	(size_t)(1 * n * sizeof(y[0])),	cudaMemcpyHostToDevice);
		cudaStat5 = cudaMemcpy(x, xHostPtr,	(size_t)(1 * m * sizeof(x[0])),	cudaMemcpyHostToDevice);
		if (
//			(cudaStat1 != cudaSuccess) ||
//			(cudaStat2 != cudaSuccess) ||
//			(cudaStat3 != cudaSuccess) ||
//			(cudaStat4 != cudaSuccess) ||
			(cudaStat5 != cudaSuccess)
			) {
			CLEANUP("Memcpy from Host to Device failed");
			return 1;
		}



		status = cusparseScsrmv(
			handle,
			CUSPARSE_OPERATION_NON_TRANSPOSE,
			n,
			m,
			nnz,
			&dtwo,
			descr,
			cooVal,
			csrRowPtr,
			cooColIndex,
			&x[0],
			&dthree,
			&y[0]);
			
		if (status != CUSPARSE_STATUS_SUCCESS) {
			CLEANUP("Matrix-vector multiplication failed");
			return 1;
		}
		cudaMemcpy(yHostPtr, y, (size_t)(1 * n * sizeof(y[0])), cudaMemcpyDeviceToHost);

		gpu_end = getTimestamp();
		gpu_execution_time_one_iteration = (gpu_end - gpu_start) / (1000);
//		printf("gpu one iteration execution time  %.6lf ms elapsed\n", gpu_execution_time_one_iteration);
		if (gpu_execution_time_one_iteration < gpu_execution_time_min)
			gpu_execution_time_min = gpu_execution_time_one_iteration;
		if (gpu_execution_time_one_iteration > gpu_execution_time_max)
			gpu_execution_time_max = gpu_execution_time_one_iteration;

		gpu_execution_time+=gpu_execution_time_one_iteration;

	}
	power_monitoring_epilogue();
	destroy_devices();
	fclose(fp_v);

	printf("-------------------------------------------\n");
	printf("max gpu execution time for %s  %.6lf ms elapsed\n", filename, gpu_execution_time_max);
	printf("min gpu execution time for %s  %.6lf ms elapsed\n", filename, gpu_execution_time_min);
	printf("ave gpu execution time for %s  %.6lf ms elapsed\n", filename, gpu_execution_time/iterations);
	printf("-------------------------------------------\n");



//	printf(" results:\n");
//	for (int i = 0; i<n; i++) {
//		printf("yHostPtr[%d]=%f\n", i, yHostPtr[i]);
//	}


	y_goldHostPtr = (DATA_TYPE *)malloc(n * sizeof(DATA_TYPE));
	for (int i = 0; i<n; i++) {
		y_goldHostPtr[i] = 0;
	}
	SpMV_Ref(
		n,
		m,
		nnz,
		cooValHostPtr,
		cooRowIndexHostPtr,
		cooColIndexHostPtr,
		xHostPtr,
		y_goldHostPtr
	);

//	printf(" results:\n");
//	for (int i = 0; i<n; i++) {
//		printf("yHostPtr[%d]=%f   y_goldHostPtr[%d]=%f\n", i, yHostPtr[i], i, y_goldHostPtr[i]);
//	}

	int result_error = 0;
	for (int i = 0; i<n; i++) {
		DATA_TYPE diff = fabs(y_goldHostPtr[i] - yHostPtr[i]);
		if (diff > 1 || diff != diff) {
			printf("error occurs at  %d with value y_gold = %lf, should be y_gpu = %lf \n", i, y_goldHostPtr[i], yHostPtr[i]);
			result_error = -1;
			break;
		}
	//	printf("data at  %d with value y_gold = %lf, should be y_gpu = %lf \n", i, y_goldHostPtr[i], yHostPtr[i]);

	}
	if (!result_error) {
		printf("Validation PASSED!\n");
	}
	else {
		printf("Validation FAILED!\n");
	}


	printf("\rBye mm!\n\r");
}
