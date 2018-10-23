#ifndef __MONITORING__H__
#define __MONITORING__H__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "jetson_tx2_power.h"
struct jetsontx2_sample {

	struct timeval start_time;
	struct timeval end_time;


	double  gpu_power;
	double  cpu_power;
	double  in_power;


	struct jetsontx2_sample *next;
};
void file_power_profile_create (char *file_name);

void *jetsontx2_read_samples(void *head);
void *jetsontx2_read_sample_pthread();
void jetsontx2_read_sample_start();
void jetsontx2_read_sample_stop();
void jetsontx2_save_average_pthread(struct jetsontx2_sample *head, char *file_name);
void jetsontx2_clear_sample_pthread(struct jetsontx2_sample *head);


void file_power_profile_create (char *file_name);
void power_monitoring_prologue();
void power_monitoring_epilogue();
void power_monitoring_stop();



#endif //__MONITORING__H__
