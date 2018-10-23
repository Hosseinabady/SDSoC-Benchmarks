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

