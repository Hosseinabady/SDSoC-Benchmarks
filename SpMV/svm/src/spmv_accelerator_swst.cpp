
#include "spmv_accelerator_swst.h"



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


	) {


	int k=0;

	spmv_accel(
			values_indices_1,           //HP1
			values_indices_2,           //HP2
			values_indices_3,           //HP3
			values_indices_4,           //HP4



			x,


			y_1,
			y_2,
			y_3,
			y_4,


			row_1_size,
			row_2_size,
			row_3_size,
			row_4_size,
			row_5_size,
			row_6_size,
			row_7_size,
			row_8_size,

			row_size_max,


			values_1_size,
			values_2_size,
			values_3_size,
			values_4_size,
			values_5_size,
			values_6_size,
			values_7_size,
			values_8_size,

			col_size,

			compact_indices_size
		);


	return k;
}
