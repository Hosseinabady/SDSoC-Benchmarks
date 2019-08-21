#ifndef __MONITORING__H__
#define __MONITORING__H__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>


struct jetsontx1_sample {

	struct timeval start_time;
	struct timeval end_time;


	double  gpu_power;
	double  cpu_power;
	double  in_power;


	struct jetsontx1_sample *next;
};


void *jetsontx1_read_samples(void *head);
void *jetsontx1_read_sample_pthread();
void jetsontx1_read_sample_start();
void jetsontx1_read_sample_stop(); 
void jetsontx1_save_average_pthread(struct jetsontx1_sample *head, char *file_name);
void jetsontx1_clear_sample_pthread(struct jetsontx1_sample *head);


void file_power_profile_create (char *file_name);
void power_monitoring_prologue();
void power_monitoring_epilogue();
void power_monitoring_stop();



#endif //__MONITORING__H__
