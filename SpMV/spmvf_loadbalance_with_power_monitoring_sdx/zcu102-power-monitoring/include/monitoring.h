/* File: monitoring.cpp
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
* File name : monitoring.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 31 August 2018
* blog: https://highlevel-synthesis.com/
*/

#ifndef __MONITORING__H__
#define __MONITORING__H__
#include "read_sensors.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>


struct zcu102_sample {

	struct timeval start_time;
	struct timeval end_time;

	//ps rails
	double  ps_full_power;
	double  ps_low_power;
	double  ps_aux_power;
	double  ps_pll_power;
	double  ps_mgtravcc_power;
	double  ps_mgtravtt_power;
	double  ps_vcc0_power;
	double  ps_vcc03_power;
	double  ps_ddrpl_power;
	double  ps_0046_power;

	//pl rails
	double  pl_vccint_power;
	double  pl_bram_power;
	double  pl_aux_power;
	double  pl_1V2_power;
	double  pl_3V3_power;
	double  pl_vadj_power;
	double  pl_mgtavcc_power;
	double  pl_mgtavtt_power;


	struct zcu102_sample *next;
};


void *zcu102_read_samples(void *head);
void *zcu102_read_sample_pthread();
void zcu102_read_sample_start();
void zcu102_read_sample_stop();
void zcu102_save_average_pthread(struct zcu102_sample *head, char *file_name);
void zcu102_clear_sample_pthread(struct zcu102_sample *head);


void file_power_profile_create (char *file_name);
void power_monitoring_prologue();
void power_monitoring_epilogue();
void power_monitoring_stop();



#endif //__MONITORING__H__

