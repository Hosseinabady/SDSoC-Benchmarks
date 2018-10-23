#ifndef __SPMV_H__
#define __SPMV_H__

#include <stdint.h>
#include <ap_int.h>


const int II = 4;  //f=100MHz
//const int II = 8;  //f=200MHz




#define  ROWS          30000       //4 //1024
#define  COLS          30000    //5 //1024
#define  DATA_LENGTH   10000000  // 3070 //10 //2048
#define  MODIFIED_DATA_LENGTH   DATA_LENGTH
#define  MAX_COL_SIZE  30000
#define  MAX_ROW_SIZE  30000

//typedef ap_uint<32> u32;
typedef unsigned  int u32;

typedef float DATA_TYPE;
#define DATA_TYPE_BIT_WIDTH   32
#define BUS_BIT_WIDTH         128

typedef ap_uint<BUS_BIT_WIDTH> uintbuswidth_t;
typedef ap_uint<BUS_BIT_WIDTH/2> uintbushalfwidth_t;


union float_ap_uint32_t {
	float f;
	unsigned long int apint;



} ;



void spmv_accel(
		uintbuswidth_t  values_indices_1[DATA_LENGTH],           //HP1
		uintbuswidth_t  values_indices_2[DATA_LENGTH],           //HP2
		uintbuswidth_t  values_indices_3[DATA_LENGTH],           //HP3
		uintbuswidth_t  values_indices_4[DATA_LENGTH],           //HP4

		u32              indices_compact_length,

		uintbuswidth_t       x[COLS],


		uintbuswidth_t       y_1[ROWS],
		uintbuswidth_t       y_2[ROWS],
		uintbuswidth_t       y_3[ROWS],
		uintbuswidth_t       y_4[ROWS],




		u32              row_1_size,
		u32              row_2_size,
		u32              row_3_size,
		u32              row_4_size,
		u32              row_5_size,
		u32              row_6_size,
		u32              row_7_size,
		u32              row_8_size,
		u32              row_size_max,


		u32              values_1_size,
		u32              values_2_size,
		u32              values_3_size,
		u32              values_4_size,
		u32              values_5_size,
		u32              values_6_size,
		u32              values_7_size,
		u32              values_8_size,
		u32              col_size,

		u32              compact_indices_size
	) ;

#endif //__SPMV_H__
