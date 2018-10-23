#ifndef __SVM_ACCELERATOR_SWST_H__
#define __SVM_ACCELERATOR_SWST_H__
#include "spmv_accelerator.h"

int get_Q_form_hardware_accel (
		uintbuswidth_t  *values_indices_1,           //HP1
		uintbuswidth_t  *values_indices_2,           //HP2
		uintbuswidth_t  *values_indices_3,           //HP3
		uintbuswidth_t  *values_indices_4,           //HP4



		uintbuswidth_t       *x,


		uintbuswidth_t       *y_1,
		uintbuswidth_t       *y_2,
		uintbuswidth_t       *y_3,
		uintbuswidth_t       *y_4,



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


	);

#endif //__SVM_ACCELERATOR_SWST_H__
