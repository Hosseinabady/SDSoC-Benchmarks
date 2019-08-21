#pragma once

#define CLEANUP(s)                                   \
do {                                                 \
    printf ("%s\n", s);                              \
} while (0)

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

typedef int u32;
typedef float DATA_TYPE;




int read_spmv_file(
	int       *row_size,
	int       *col_size,
	int       *data_size,
	DATA_TYPE **cooValHostPtr,
	u32       **cooRowIndexHostPtr,
	u32       **cooColIndexHostPtr,
	DATA_TYPE **xHostPtr,
	DATA_TYPE **yHostPtr,
	char      *inputFile);


void SpMV_Ref(
	int       row_size,
	int       col_size,
	int       data_size,
	DATA_TYPE *cooValHostPtr,
	u32       *cooRowIndexHostPtr,
	u32       *cooColIndexHostPtr,
	DATA_TYPE *xHostPtr,
	DATA_TYPE *y_goldHostPtr
	);

double getTimestamp();
