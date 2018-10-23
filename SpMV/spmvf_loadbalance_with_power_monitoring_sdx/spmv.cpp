/* File: spmv.cpp
 *
 Copyright (c) [2016] [Mohammad Hosseinabady (mohammad@hosseinabady.com)]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
* This file has been written at University of Bristol
* for the ENPOWER project funded by EPSRC
*
* File name : spmv.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 31 August 2018
* blog: https://highlevel-synthesis.com/
*/


#include "spmv.h"
#include <hls_stream.h>

const int BUFFER_SIZE=128;


void spvm_kernel(
		uintbuswidth_t  *values_indices_1,
		uintbuswidth_t  *values_indices_2,
		uintbuswidth_t  *values_indices_3,
		uintbuswidth_t  *values_indices_4,



		DATA_TYPE  *x_1,
		DATA_TYPE  *x_2,
		DATA_TYPE  *x_3,
		DATA_TYPE  *x_4,
		DATA_TYPE  *x_5,
		DATA_TYPE  *x_6,
		DATA_TYPE  *x_7,
		DATA_TYPE  *x_8,


		DATA_TYPE  *y_1,
		DATA_TYPE  *y_2,
		DATA_TYPE  *y_3,
		DATA_TYPE  *y_4,
		DATA_TYPE  *y_5,
		DATA_TYPE  *y_6,
		DATA_TYPE  *y_7,
		DATA_TYPE  *y_8,



		u32        row_1_size,
		u32        row_2_size,
		u32        row_3_size,
		u32        row_4_size,
		u32        row_5_size,
		u32        row_6_size,
		u32        row_7_size,
		u32        row_8_size,
		u32        row_size_max,

		u32        values_1_size,
		u32        values_2_size,
		u32        values_3_size,
		u32        values_4_size,
		u32        values_5_size,
		u32        values_6_size,
		u32        values_7_size,
		u32        values_8_size,

		u32        compact_indices_size
) {

#pragma HLS DATAFLOW


	hls::stream<u32>       indices_fifo_1;
#pragma HLS STREAM variable=indices_fifo_1 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       indices_fifo_2;
#pragma HLS STREAM variable=indices_fifo_2 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       indices_fifo_3;
#pragma HLS STREAM variable=indices_fifo_3 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       indices_fifo_4;
#pragma HLS STREAM variable=indices_fifo_4 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       indices_fifo_5;
#pragma HLS STREAM variable=indices_fifo_5 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       indices_fifo_6;
#pragma HLS STREAM variable=indices_fifo_6 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       indices_fifo_7;
#pragma HLS STREAM variable=indices_fifo_7 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       indices_fifo_8;
#pragma HLS STREAM variable=indices_fifo_8 depth=BUFFER_SIZE dim=1


	hls::stream<u32>       row_fifo_1;
#pragma HLS STREAM variable=row_fifo_1 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       row_fifo_2;
#pragma HLS STREAM variable=row_fifo_2 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       row_fifo_3;
#pragma HLS STREAM variable=row_fifo_3 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       row_fifo_4;
#pragma HLS STREAM variable=row_fifo_4 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       row_fifo_5;
#pragma HLS STREAM variable=row_fifo_5 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       row_fifo_6;
#pragma HLS STREAM variable=row_fifo_6 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       row_fifo_7;
#pragma HLS STREAM variable=row_fifo_7 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       row_fifo_8;
#pragma HLS STREAM variable=row_fifo_8 depth=BUFFER_SIZE dim=1


	hls::stream<u32>       col_fifo_1;
#pragma HLS STREAM variable=col_fifo_1 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       col_fifo_2;
#pragma HLS STREAM variable=col_fifo_2 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       col_fifo_3;
#pragma HLS STREAM variable=col_fifo_3 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       col_fifo_4;
#pragma HLS STREAM variable=col_fifo_4 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       col_fifo_5;
#pragma HLS STREAM variable=col_fifo_5 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       col_fifo_6;
#pragma HLS STREAM variable=col_fifo_6 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       col_fifo_7;
#pragma HLS STREAM variable=col_fifo_7 depth=BUFFER_SIZE dim=1
	hls::stream<u32>       col_fifo_8;
#pragma HLS STREAM variable=col_fifo_8 depth=BUFFER_SIZE dim=1



	hls::stream<uintbushalfwidth_t> values_indices_fifo_1;
#pragma HLS STREAM variable=values_indices_fifo_1 depth=BUFFER_SIZE dim=1
	hls::stream<uintbushalfwidth_t> values_indices_fifo_2;
#pragma HLS STREAM variable=values_indices_fifo_2 depth=BUFFER_SIZE dim=1
	hls::stream<uintbushalfwidth_t> values_indices_fifo_3;
#pragma HLS STREAM variable=values_indices_fifo_3 depth=BUFFER_SIZE dim=1
	hls::stream<uintbushalfwidth_t> values_indices_fifo_4;
#pragma HLS STREAM variable=values_indices_fifo_4 depth=BUFFER_SIZE dim=1
	hls::stream<uintbushalfwidth_t> values_indices_fifo_5;
#pragma HLS STREAM variable=values_indices_fifo_5 depth=BUFFER_SIZE dim=1
	hls::stream<uintbushalfwidth_t> values_indices_fifo_6;
#pragma HLS STREAM variable=values_indices_fifo_6 depth=BUFFER_SIZE dim=1
	hls::stream<uintbushalfwidth_t> values_indices_fifo_7;
#pragma HLS STREAM variable=values_indices_fifo_7 depth=BUFFER_SIZE dim=1
	hls::stream<uintbushalfwidth_t> values_indices_fifo_8;
#pragma HLS STREAM variable=values_indices_fifo_8 depth=BUFFER_SIZE dim=1



	hls::stream<DATA_TYPE> values_fifo_1_;
#pragma HLS STREAM variable=values_fifo_1_ depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_2_;
#pragma HLS STREAM variable=values_fifo_2_ depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_3_;
#pragma HLS STREAM variable=values_fifo_3_ depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_4_;
#pragma HLS STREAM variable=values_fifo_4_ depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_5_;
#pragma HLS STREAM variable=values_fifo_5_ depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_6_;
#pragma HLS STREAM variable=values_fifo_6_ depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_7_;
#pragma HLS STREAM variable=values_fifo_7_ depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_8_;
#pragma HLS STREAM variable=values_fifo_8_ depth=BUFFER_SIZE dim=1


	hls::stream<DATA_TYPE> values_fifo_1;
#pragma HLS STREAM variable=values_fifo_1 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_2;
#pragma HLS STREAM variable=values_fifo_2 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_3;
#pragma HLS STREAM variable=values_fifo_3 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_4;
#pragma HLS STREAM variable=values_fifo_4 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_5;
#pragma HLS STREAM variable=values_fifo_5 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_6;
#pragma HLS STREAM variable=values_fifo_6 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_7;
#pragma HLS STREAM variable=values_fifo_7 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> values_fifo_8;
#pragma HLS STREAM variable=values_fifo_8 depth=BUFFER_SIZE dim=1



	hls::stream<DATA_TYPE> results_fifo_1;
#pragma HLS STREAM variable=results_fifo_1 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> results_fifo_2;
#pragma HLS STREAM variable=results_fifo_2 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> results_fifo_3;
#pragma HLS STREAM variable=results_fifo_3 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> results_fifo_4;
#pragma HLS STREAM variable=results_fifo_4 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> results_fifo_5;
#pragma HLS STREAM variable=results_fifo_5 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> results_fifo_6;
#pragma HLS STREAM variable=results_fifo_6 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> results_fifo_7;
#pragma HLS STREAM variable=results_fifo_7 depth=BUFFER_SIZE dim=1
	hls::stream<DATA_TYPE> results_fifo_8;
#pragma HLS STREAM variable=results_fifo_8 depth=BUFFER_SIZE dim=1


	u32 tmp_int;
	u32 i, r;


	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE
		uintbuswidth_t  tmp_value_indices = values_indices_1[i];
		values_indices_fifo_1 << tmp_value_indices.range(63, 0);
		values_indices_fifo_2 << tmp_value_indices.range(127, 64);
	}


	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE
		uintbuswidth_t  tmp_value_indices = values_indices_2[i];

		values_indices_fifo_3 << tmp_value_indices.range(63, 0);
		values_indices_fifo_4 << tmp_value_indices.range(127, 64);

	}

	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE
		uintbuswidth_t  tmp_value_indices = values_indices_3[i];

		values_indices_fifo_5 << tmp_value_indices.range(63, 0);
		values_indices_fifo_6 << tmp_value_indices.range(127, 64);
	}




	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE
		uintbuswidth_t  tmp_value_indices = values_indices_4[i];
		values_indices_fifo_7 << tmp_value_indices.range(63, 0);
		values_indices_fifo_8 << tmp_value_indices.range(127, 64);
	}


	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_1.read();

		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);

		if (i < values_1_size+row_1_size) {
			indices_fifo_1 << tmp_u32;
			values_fifo_1_ << tmp_float_uint32.f;
		}
	}


	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_2.read();

		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);


		if (i < values_2_size+row_2_size) {
			indices_fifo_2 << tmp_u32;
			values_fifo_2_ << tmp_float_uint32.f;
		}
	}


	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_3.read();
		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);

		if (i < values_3_size+row_3_size) {
			indices_fifo_3 << tmp_u32;
			values_fifo_3_ << tmp_float_uint32.f;
		}
	}


	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_4.read();
		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);

		if (i < values_4_size+row_4_size) {
			indices_fifo_4 << tmp_u32;
			values_fifo_4_ << tmp_float_uint32.f;
		}
	}

	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_5.read();
		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);


		if (i < values_5_size+row_5_size) {
			indices_fifo_5 << tmp_u32;
			values_fifo_5_ << tmp_float_uint32.f;
		}
	}

	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_6.read();
		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);

		if (i < values_6_size+row_6_size) {
			indices_fifo_6 << tmp_u32;
			values_fifo_6_ << tmp_float_uint32.f;
		}
	}


	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_7.read();
		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);

		if (i < values_7_size+row_7_size) {
			indices_fifo_7 << tmp_u32;
			values_fifo_7_ << tmp_float_uint32.f;
		}
	}

	for (i = 0; i < compact_indices_size; i++) {
#pragma HLS PIPELINE

		uintbushalfwidth_t	tmp_value_indices = values_indices_fifo_8.read();
		u32 tmp_u32 = tmp_value_indices.range(31, 0);
		union float_ap_uint32_t tmp_float_uint32;
		tmp_float_uint32.apint = tmp_value_indices.range(63, 32);

		if (i < values_8_size+row_8_size) {
			indices_fifo_8 << tmp_u32;
			values_fifo_8_ << tmp_float_uint32.f;
		}
	}



	u32 col_left_1 = 0;
	for (r = 0; r < values_1_size+row_1_size; r++) {
#pragma HLS PIPELINE
		u32 index_1 = indices_fifo_1.read();
		if (col_left_1 == 0 ) {
			col_left_1 = index_1;
			if (col_left_1 != 0)
				row_fifo_1 << col_left_1;
		} else {
			col_fifo_1 << index_1;
			col_left_1--;
		}

		values_fifo_1 << values_fifo_1_.read();
	}


	u32 col_left_2 = 0;
	for (r = 0; r < values_2_size+row_2_size; r++) {
#pragma HLS PIPELINE
		u32 index_2 = indices_fifo_2.read();
		if (col_left_2 == 0 ) {
			col_left_2 = index_2;
			if (col_left_2 != 0)
				row_fifo_2 << col_left_2;
		} else {
			col_fifo_2 << index_2;
			col_left_2--;
		}

		values_fifo_2 << values_fifo_2_.read();
	}


	u32 col_left_3 = 0;
	for (r = 0; r < values_3_size+row_3_size; r++) {
#pragma HLS PIPELINE
		u32 index_3 = indices_fifo_3.read();

		if (col_left_3 == 0 ) {
			col_left_3 = index_3;
			if (col_left_3 != 0)
				row_fifo_3 << col_left_3;
		} else {
			col_fifo_3 << index_3;
			col_left_3--;
		}

		values_fifo_3 << values_fifo_3_.read();
	}

	u32 col_left_4 = 0;
	for (r = 0; r < values_4_size+row_4_size; r++) {
#pragma HLS PIPELINE
		u32 index_4 = indices_fifo_4.read();

		if (col_left_4 == 0 ) {
			col_left_4 = index_4;
			if (col_left_4 != 0)
				row_fifo_4 << col_left_4;
		} else {
			col_fifo_4 << index_4;
			col_left_4--;
		}

		values_fifo_4 << values_fifo_4_.read();
	}


	u32 col_left_5 = 0;
	for (r = 0; r < values_5_size+row_5_size; r++) {
#pragma HLS PIPELINE
		u32 index_5 = indices_fifo_5.read();

		if (col_left_5 == 0 ) {
			col_left_5 = index_5;
			if (col_left_5 != 0)
				row_fifo_5 << col_left_5;
		} else {
			col_fifo_5 << index_5;
			col_left_5--;
		}

		values_fifo_5 << values_fifo_5_.read();
	}


	u32 col_left_6 = 0;
	for (r = 0; r < values_6_size+row_6_size; r++) {
#pragma HLS PIPELINE
		u32 index_6 = indices_fifo_6.read();

		if (col_left_6 == 0 ) {
			col_left_6 = index_6;
			if (col_left_6 != 0)
				row_fifo_6 << col_left_6;
		} else {
			col_fifo_6 << index_6;
			col_left_6--;
		}

		values_fifo_6 << values_fifo_6_.read();
	}


	u32 col_left_7 = 0;
	for (r = 0; r < values_7_size+row_7_size; r++) {
#pragma HLS PIPELINE
		u32 index_7 = indices_fifo_7.read();

		if (col_left_7 == 0 ) {
			col_left_7 = index_7;
			if (col_left_7 != 0)
				row_fifo_7 << col_left_7;
		} else {
			col_fifo_7 << index_7;
			col_left_7--;
		}

		values_fifo_7 << values_fifo_7_.read();
	}


	u32 col_left_8 = 0;
	for (r = 0; r < values_8_size+row_8_size; r++) {
#pragma HLS PIPELINE
		u32 index_8 = indices_fifo_8.read();

		if (col_left_8 == 0 ) {
			col_left_8 = index_8;
			if (col_left_8 != 0)
				row_fifo_8 << col_left_8;
		} else {
			col_fifo_8 << index_8;
			col_left_8--;
		}

		values_fifo_8 << values_fifo_8_.read();
	}


	DATA_TYPE sum_1;

	DATA_TYPE value_1[II];
#pragma HLS ARRAY_PARTITION variable=value_1 complete dim=1
	u32 col_1[II];
#pragma HLS ARRAY_PARTITION variable=col_1 complete dim=1
	DATA_TYPE term_1[II];
#pragma HLS ARRAY_PARTITION variable=term_1 complete dim=1


	col_left_1 = 0;
	for (r = 0; r < values_1_size+row_1_size*II; r+=II) {
#pragma HLS PIPELINE

		if (col_left_1 == 0) {
			col_left_1 = row_fifo_1.read();
			volatile  DATA_TYPE dummy = values_fifo_1.read();
			sum_1 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_1[i] = values_fifo_1.read();
				col_1[i]   = col_fifo_1.read();
				term_1[i]  = value_1[i] * x_1[col_1[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_1[i];
			}
			sum_1 += sum_tmp;
			col_left_1-=II;

			if (col_left_1 == 0) {
				results_fifo_1 << sum_1;
			}
		}

	}


	DATA_TYPE sum_2;

	DATA_TYPE value_2[II];
#pragma HLS ARRAY_PARTITION variable=value_2 complete dim=1
	u32 col_2[II];
#pragma HLS ARRAY_PARTITION variable=col_2 complete dim=1
	DATA_TYPE term_2[II];
#pragma HLS ARRAY_PARTITION variable=term_2 complete dim=1

	col_left_2 = 0;

	for (r = 0; r < values_2_size+row_2_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left_2 == 0) {
			col_left_2 = row_fifo_2.read();
			volatile  DATA_TYPE dummy = values_fifo_2.read();
			sum_2 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_2[i] = values_fifo_2.read();
				col_2[i]   = col_fifo_2.read();
				term_2[i]  = value_2[i] * x_2[col_2[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_2[i];
			}
			sum_2 += sum_tmp;
			col_left_2-=II;

			if (col_left_2 == 0) {
				results_fifo_2 << sum_2;
			}
		}

	}




	DATA_TYPE sum_3;

	DATA_TYPE value_3[II];
#pragma HLS ARRAY_PARTITION variable=value_3 complete dim=1
	u32 col_3[II];
#pragma HLS ARRAY_PARTITION variable=col_3 complete dim=1
	DATA_TYPE term_3[II];
#pragma HLS ARRAY_PARTITION variable=term_3 complete dim=1

	col_left_3 = 0;
	for (r = 0; r < values_3_size+row_3_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left_3 == 0) {
			col_left_3 = row_fifo_3.read();
			volatile  DATA_TYPE dummy = values_fifo_3.read();
			sum_3 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_3[i] = values_fifo_3.read();
				col_3[i]   = col_fifo_3.read();
				term_3[i]  = value_3[i] * x_3[col_3[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_3[i];
			}
			sum_3 += sum_tmp;
			col_left_3-=II;

			if (col_left_3 == 0) {
				results_fifo_3 << sum_3;
			}
		}

	}


	DATA_TYPE sum_4;

	DATA_TYPE value_4[II];
#pragma HLS ARRAY_PARTITION variable=value_4 complete dim=1
	u32 col_4[II];
#pragma HLS ARRAY_PARTITION variable=col_4 complete dim=1
	DATA_TYPE term_4[II];
#pragma HLS ARRAY_PARTITION variable=term_4 complete dim=1

	col_left_4 = 0;
	for (r = 0; r < values_4_size+row_4_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left_4 == 0) {
			col_left_4 = row_fifo_4.read();
			volatile  DATA_TYPE dummy = values_fifo_4.read();
			sum_4 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_4[i] = values_fifo_4.read();
				col_4[i]   = col_fifo_4.read();
				term_4[i]  = value_4[i] * x_4[col_4[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_4[i];
			}
			sum_4 += sum_tmp;
			col_left_4-=II;

			if (col_left_4 == 0) {
				results_fifo_4 << sum_4;
			}
		}

	}


	DATA_TYPE sum_5;

	DATA_TYPE value_5[II];
#pragma HLS ARRAY_PARTITION variable=value_5 complete dim=1
	u32 col_5[II];
#pragma HLS ARRAY_PARTITION variable=col_5 complete dim=1
	DATA_TYPE term_5[II];
#pragma HLS ARRAY_PARTITION variable=term_5 complete dim=1

	col_left_5 = 0;
	for (r = 0; r < values_5_size+row_5_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left_5 == 0) {
			col_left_5 = row_fifo_5.read();
			volatile  DATA_TYPE dummy = values_fifo_5.read();
			sum_5 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_5[i] = values_fifo_5.read();
				col_5[i]   = col_fifo_5.read();
				term_5[i]  = value_5[i] * x_5[col_5[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_5[i];
			}
			sum_5 += sum_tmp;
			col_left_5-=II;

			if (col_left_5 == 0) {
				results_fifo_5 << sum_5;
			}
		}

	}



	DATA_TYPE sum_6;

	DATA_TYPE value_6[II];
#pragma HLS ARRAY_PARTITION variable=value_6 complete dim=1
	u32 col_6[II];
#pragma HLS ARRAY_PARTITION variable=col_6 complete dim=1
	DATA_TYPE term_6[II];
#pragma HLS ARRAY_PARTITION variable=term_6 complete dim=1

	col_left_6 = 0;
	for (r = 0; r < values_6_size+row_6_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left_6 == 0) {
			col_left_6 = row_fifo_6.read();
			volatile  DATA_TYPE dummy = values_fifo_6.read();
			sum_6 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_6[i] = values_fifo_6.read();
				col_6[i]   = col_fifo_6.read();
				term_6[i]  = value_6[i] * x_6[col_6[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_6[i];
			}
			sum_6 += sum_tmp;
			col_left_6-=II;

			if (col_left_6 == 0) {
				results_fifo_6 << sum_6;
			}
		}

	}




	DATA_TYPE sum_7;

	DATA_TYPE value_7[II];
#pragma HLS ARRAY_PARTITION variable=value_7 complete dim=1
	u32 col_7[II];
#pragma HLS ARRAY_PARTITION variable=col_7 complete dim=1
	DATA_TYPE term_7[II];
#pragma HLS ARRAY_PARTITION variable=term_7 complete dim=1

	col_left_7 = 0;
	for (r = 0; r < values_7_size+row_7_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left_7 == 0) {
			col_left_7 = row_fifo_7.read();
			volatile  DATA_TYPE dummy = values_fifo_7.read();
			sum_7 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_7[i] = values_fifo_7.read();
				col_7[i]   = col_fifo_7.read();
				term_7[i]  = value_7[i] * x_7[col_7[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_7[i];
			}
			sum_7 += sum_tmp;
			col_left_7-=II;

			if (col_left_7 == 0) {
				results_fifo_7 << sum_7;
			}
		}

	}




	DATA_TYPE sum_8;

	DATA_TYPE value_8[II];
#pragma HLS ARRAY_PARTITION variable=value_8 complete dim=1
	u32 col_8[II];
#pragma HLS ARRAY_PARTITION variable=col_8 complete dim=1
	DATA_TYPE term_8[II];
#pragma HLS ARRAY_PARTITION variable=term_8 complete dim=1

	col_left_8 = 0;
	for (r = 0; r < values_8_size+row_8_size*II; r+=II) {
#pragma HLS PIPELINE
		if (col_left_8 == 0) {
			col_left_8 = row_fifo_8.read();
			volatile  DATA_TYPE dummy = values_fifo_8.read();
			sum_8 = 0;
		} else {
			for (int i = 0; i < II; i++) {
				value_8[i] = values_fifo_8.read();
				col_8[i]   = col_fifo_8.read();
				term_8[i]  = value_8[i] * x_8[col_8[i]];
			}
			DATA_TYPE sum_tmp=0;
			for (int i = 0; i < II; i++) {
				sum_tmp += term_8[i];
			}
			sum_8 += sum_tmp;
			col_left_8-=II;

			if (col_left_8 == 0) {
				results_fifo_8 << sum_8;
			}
		}

	}



	for (i = 0; i < row_1_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_1.read();
		y_1[i] =  d;
	}

	for (i = 0; i < row_2_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_2.read();
		y_2[i] =  d;
	}

	for (i = 0; i < row_3_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_3.read();
		y_3[i] =  d;
	}
	for (i = 0; i < row_4_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_4.read();
		y_4[i] =  d;
	}

	for (i = 0; i < row_5_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_5.read();
		y_5[i] =  d;
	}

	for (i = 0; i < row_6_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_6.read();
		y_6[i] =  d;
	}

	for (i = 0; i < row_7_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_7.read();
		y_7[i] =  d;
	}
	for (i = 0; i < row_8_size; i++) {
#pragma HLS PIPELINE

		DATA_TYPE d = results_fifo_8.read();
		y_8[i] =  d;
	}


}


void write_back_data (
		u32              row_1_size,
		u32              row_2_size,
		u32              row_3_size,
		u32              row_4_size,
		u32              row_5_size,
		u32              row_6_size,
		u32              row_7_size,
		u32              row_8_size,
		DATA_TYPE  *y_1,
		DATA_TYPE  *y_2,
		DATA_TYPE  *y_3,
		DATA_TYPE  *y_4,
		DATA_TYPE  *y_5,
		DATA_TYPE  *y_6,
		DATA_TYPE  *y_7,
		DATA_TYPE  *y_8,
		uintbuswidth_t       *y_out_1,
		uintbuswidth_t       *y_out_2,
		uintbuswidth_t       *y_out_3,
		uintbuswidth_t       *y_out_4
		)
{
#pragma HLS DATAFLOW
	hls::stream<DATA_TYPE>       y_fifo_1;
	hls::stream<DATA_TYPE>       y_fifo_2;
	hls::stream<DATA_TYPE>       y_fifo_3;
	hls::stream<DATA_TYPE>       y_fifo_4;


	u32 i;

	for (i = 0; i < row_1_size + row_2_size; i++) {
	#pragma HLS PIPELINE
		if (i < row_1_size)
			y_fifo_1 << y_1[i];
		else
			y_fifo_1 << y_2[i - (row_1_size)];
	}


	for (i = 0; i < row_3_size + row_4_size; i++) {
	#pragma HLS PIPELINE
		if (i < row_3_size)
			y_fifo_2 << y_3[i];
		else
			y_fifo_2 << y_4[i - (row_3_size)];
	}


	for (i = 0; i < row_5_size + row_6_size; i++) {
	#pragma HLS PIPELINE
		if (i < row_5_size)
			y_fifo_3 << y_5[i];
		else
			y_fifo_3 << y_6[i - (row_5_size)];
	}

	for (i = 0; i < row_7_size + row_8_size; i++) {
	#pragma HLS PIPELINE
		if (i < row_7_size)
			y_fifo_4 << y_7[i];
		else
			y_fifo_4 << y_8[i - (row_7_size)];
	}



	for (i = 0; i < (row_1_size + row_2_size)/4; i++) {
#pragma HLS PIPELINE
		float_ap_uint32_t f2int_1;
		uintbuswidth_t  y_128;

		f2int_1.f = y_fifo_1.read();
		y_128.range(31,  0)   = f2int_1.apint;

		f2int_1.f = y_fifo_1.read();
		y_128.range(63,  32)  = f2int_1.apint;

		f2int_1.f = y_fifo_1.read();
		y_128.range(95,  64)  = f2int_1.apint;

		f2int_1.f = y_fifo_1.read();
		y_128.range(127,  96) = f2int_1.apint;

		y_out_1[i] = y_128;
	}


	for (i = 0; i < (row_3_size + row_4_size)/4; i++) {
#pragma HLS PIPELINE
		float_ap_uint32_t f2int_1;
		uintbuswidth_t  y_128;

		f2int_1.f = y_fifo_2.read();
		y_128.range(31,  0)   = f2int_1.apint;

		f2int_1.f = y_fifo_2.read();
		y_128.range(63,  32)  = f2int_1.apint;

		f2int_1.f = y_fifo_2.read();
		y_128.range(95,  64)  = f2int_1.apint;

		f2int_1.f = y_fifo_2.read();
		y_128.range(127,  96) = f2int_1.apint;

		y_out_2[i] = y_128;


	}

	for (i = 0; i < (row_5_size + row_6_size)/4; i++) {
#pragma HLS PIPELINE
		float_ap_uint32_t f2int_1;
		uintbuswidth_t  y_128;

		f2int_1.f = y_fifo_3.read();
		y_128.range(31,  0)   = f2int_1.apint;

		f2int_1.f = y_fifo_3.read();
		y_128.range(63,  32)  = f2int_1.apint;

		f2int_1.f = y_fifo_3.read();
		y_128.range(95,  64)  = f2int_1.apint;

		f2int_1.f = y_fifo_3.read();
		y_128.range(127,  96) = f2int_1.apint;

		y_out_3[i] = y_128;


	}

	for (i = 0; i < (row_7_size + row_8_size)/4; i++) {
#pragma HLS PIPELINE
		float_ap_uint32_t f2int_1;
		uintbuswidth_t  y_128;

		f2int_1.f = y_fifo_4.read();
		y_128.range(31,  0)   = f2int_1.apint;

		f2int_1.f = y_fifo_4.read();
		y_128.range(63,  32)  = f2int_1.apint;

		f2int_1.f = y_fifo_4.read();
		y_128.range(95,  64)  = f2int_1.apint;

		f2int_1.f = y_fifo_4.read();
		y_128.range(127,  96) = f2int_1.apint;

		y_out_4[i] = y_128;


	}


}







#pragma SDS data zero_copy(values_indices_1[0:DATA_LENGTH])
#pragma SDS data mem_attribute(values_indices_1:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(values_indices_2[0:DATA_LENGTH])
#pragma SDS data mem_attribute(values_indices_2:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(values_indices_3[0:DATA_LENGTH])
#pragma SDS data mem_attribute(values_indices_3:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(values_indices_4[0:DATA_LENGTH])
#pragma SDS data mem_attribute(values_indices_4:PHYSICAL_CONTIGUOUS)

#pragma SDS data zero_copy(x[0:COLS])
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS)



#pragma SDS data zero_copy(y_1[0:ROWS])
#pragma SDS data mem_attribute(y_1:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(y_2[0:ROWS])
#pragma SDS data mem_attribute(y_2:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(y_3[0:ROWS])
#pragma SDS data mem_attribute(y_3:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(y_4[0:ROWS])
#pragma SDS data mem_attribute(y_4:PHYSICAL_CONTIGUOUS)



void spmv_accel(
		uintbuswidth_t  *values_indices_1,           //HP1
		uintbuswidth_t  *values_indices_2,           //HP2
		uintbuswidth_t  *values_indices_3,           //HP3
		uintbuswidth_t  *values_indices_4,           //HP4

		u32              indices_compact_length,

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







	DATA_TYPE              x_1_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_1_local cyclic factor=2 dim=1
	DATA_TYPE              x_2_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_2_local cyclic factor=2 dim=1
	DATA_TYPE              x_3_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_3_local cyclic factor=2 dim=1
	DATA_TYPE              x_4_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_4_local cyclic factor=2 dim=1
	DATA_TYPE              x_5_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_5_local cyclic factor=2 dim=1
	DATA_TYPE              x_6_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_6_local cyclic factor=2 dim=1
	DATA_TYPE              x_7_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_7_local cyclic factor=2 dim=1
	DATA_TYPE              x_8_local[MAX_COL_SIZE];
#pragma HLS ARRAY_PARTITION variable=x_8_local cyclic factor=2 dim=1



	DATA_TYPE              y_1_local[MAX_COL_SIZE];
	DATA_TYPE              y_2_local[MAX_COL_SIZE];
	DATA_TYPE              y_3_local[MAX_COL_SIZE];
	DATA_TYPE              y_4_local[MAX_COL_SIZE];
	DATA_TYPE              y_5_local[MAX_COL_SIZE];
	DATA_TYPE              y_6_local[MAX_COL_SIZE];
	DATA_TYPE              y_7_local[MAX_COL_SIZE];
	DATA_TYPE              y_8_local[MAX_COL_SIZE];



	for (u32 i = 0; i < col_size/4; i+=1) {
#pragma HLS PIPELINE
		uintbuswidth_t x128_tmp = x[i];

		float_ap_uint32_t f2uint_tmp_1;
		float_ap_uint32_t f2uint_tmp_2;
		float_ap_uint32_t f2uint_tmp_3;
		float_ap_uint32_t f2uint_tmp_4;

		f2uint_tmp_1.apint = x128_tmp.range(31, 0);
		f2uint_tmp_2.apint = x128_tmp.range(63, 32);
		f2uint_tmp_3.apint = x128_tmp.range(95, 64);
		f2uint_tmp_4.apint = x128_tmp.range(127, 96);


		DATA_TYPE x_tmp_1 = f2uint_tmp_1.f;
		DATA_TYPE x_tmp_2 = f2uint_tmp_2.f;
		DATA_TYPE x_tmp_3 = f2uint_tmp_3.f;
		DATA_TYPE x_tmp_4 = f2uint_tmp_4.f;
		x_1_local[i*4+0] = x_tmp_1;
		x_1_local[i*4+1] = x_tmp_2;
		x_1_local[i*4+2] = x_tmp_3;
		x_1_local[i*4+3] = x_tmp_4;

		x_2_local[i*4+0] = x_tmp_1;
		x_2_local[i*4+1] = x_tmp_2;
		x_2_local[i*4+2] = x_tmp_3;
		x_2_local[i*4+3] = x_tmp_4;

		x_3_local[i*4+0] = x_tmp_1;
		x_3_local[i*4+1] = x_tmp_2;
		x_3_local[i*4+2] = x_tmp_3;
		x_3_local[i*4+3] = x_tmp_4;

		x_4_local[i*4+0] = x_tmp_1;
		x_4_local[i*4+1] = x_tmp_2;
		x_4_local[i*4+2] = x_tmp_3;
		x_4_local[i*4+3] = x_tmp_4;

		x_5_local[i*4+0] = x_tmp_1;
		x_5_local[i*4+1] = x_tmp_2;
		x_5_local[i*4+2] = x_tmp_3;
		x_5_local[i*4+3] = x_tmp_4;

		x_6_local[i*4+0] = x_tmp_1;
		x_6_local[i*4+1] = x_tmp_2;
		x_6_local[i*4+2] = x_tmp_3;
		x_6_local[i*4+3] = x_tmp_4;

		x_7_local[i*4+0] = x_tmp_1;
		x_7_local[i*4+1] = x_tmp_2;
		x_7_local[i*4+2] = x_tmp_3;
		x_7_local[i*4+3] = x_tmp_4;

		x_8_local[i*4+0] = x_tmp_1;
		x_8_local[i*4+1] = x_tmp_2;
		x_8_local[i*4+2] = x_tmp_3;
		x_8_local[i*4+3] = x_tmp_4;
	}
	spvm_kernel(
			values_indices_1,
			values_indices_2,
			values_indices_3,
			values_indices_4,



			x_1_local,
			x_2_local,
			x_3_local,
			x_4_local,
			x_5_local,
			x_6_local,
			x_7_local,
			x_8_local,


			y_1_local,
			y_2_local,
			y_3_local,
			y_4_local,
			y_5_local,
			y_6_local,
			y_7_local,
			y_8_local,

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


			compact_indices_size);



	write_back_data(
			row_1_size,
			row_2_size,
			row_3_size,
			row_4_size,
			row_5_size,
			row_6_size,
			row_7_size,
			row_8_size,
			y_1_local,
			y_2_local,
			y_3_local,
			y_4_local,
			y_5_local,
			y_6_local,
			y_7_local,
			y_8_local,
			y_1,
			y_2,
			y_3,
			y_4);
}


