#ifndef __SPMV_H__
#define __SPMV_H__

#include "ap_int.h"

#define  ROWS          400000
#define  COLS          400000
#define  DATA_LENGTH   20000000

#define  MAX_COL_SIZE  980000
#define  MAX_ROW_SIZE  980000


typedef ap_uint<32> u32;
//typedef unsigned long int u32;
typedef float DATA_TYPE;

const int II = 4;  //f=100MHz
//const int II = 8;  //f=200MHz
int spmv_accel(
		DATA_TYPE        values[DATA_LENGTH],
		u32        		 cols[DATA_LENGTH],
		u32        		 rows[ROWS],
		DATA_TYPE  		 x[COLS],
		DATA_TYPE  		 y[ROWS],

		u32        		  row_size,
		u32               col_size,
		u32               data_size
	) ;

#endif //__SPMV_H__
