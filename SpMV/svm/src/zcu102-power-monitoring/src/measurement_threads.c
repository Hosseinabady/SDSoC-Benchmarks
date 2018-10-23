/*
 * measurement_threads.c
 *
 *  Created on: May 15, 2015
 *      Author: csxmh
 */

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "monitoring.h"

int start;
pthread_mutex_t mutex;

int stop;
pthread_t  thread_ID;
void      *exit_status;
struct zcu102_sample * head;

long long int sample_count = 0;

//ps rails
double ps_full_power_total=0;
double ps_low_power_total=0;
double ps_aux_power_total=0;
double ps_pll_power_total=0;
double ps_mgtravcc_power_total=0;
double ps_mgtravtt_power_total=0;
double ps_vcc0_power_total=0;
double ps_vcc03_power_total=0;
double ps_ddrpl_power_total=0;
double ps_0046_power_total=0;
	    
//pl rails
double pl_vccint_power_total=0;
double pl_bram_power_total=0;
double pl_aux_power_total=0;
double pl_1V2_power_total=0;
double pl_3V3_power_total=0;
double pl_vadj_power_total=0;
double pl_mgtavcc_power_total=0;
double pl_mgtavtt_power_total=0;






void *zcu102_read_samples(void *head) {

	FILE *fp_log;


	//ps rails
	unsigned long int  ps_full_power_value;
	unsigned long int  ps_low_power_value;
	unsigned long int  ps_aux_power_value;
	unsigned long int  ps_pll_power_value;
	unsigned long int  ps_mgtravcc_power_value;
	unsigned long int  ps_mgtravtt_power_value;
	unsigned long int  ps_vcc0_power_value;
	unsigned long int  ps_vcc03_power_value;
	unsigned long int  ps_ddrpl_power_value;
	unsigned long int  ps_0046_power_value;

	//pl rails
	unsigned long int  pl_vccint_power_value;
	unsigned long int  pl_bram_power_value;
	unsigned long int  pl_aux_power_value;
	unsigned long int  pl_1V2_power_value;
	unsigned long int  pl_3V3_power_value;
	unsigned long int  pl_vadj_power_value;
	unsigned long int  pl_mgtavcc_power_value;
	unsigned long int  pl_mgtavtt_power_value;


	//unsigned int in_power_value;


//	int i;
//	struct  zcu102_sample *t = (struct zcu102_sample*) head;
	int start_flag = 0;
	int stop_flag = 0;
	fp_log = fopen("power_measurement_log.txt", "a");
	if (!fp_log) {
		printf("cannot open test_debug.txt file \n ");
		return 0;
	}


	while(1) {

		start_flag = 1;
		pthread_mutex_lock(&mutex);
		if(start == 0)
			start_flag = 0;
		pthread_mutex_unlock(&mutex);
		if(start_flag == 0)
			continue;






		//ps rails
		zcu102_get_power(VCCPSINTFP,   &ps_full_power_value);
		zcu102_get_power(VCCPSINTLP,   &ps_low_power_value);
		zcu102_get_power(VCCPSAUX,     &ps_aux_power_value);
		zcu102_get_power(VCCPSPLL,     &ps_pll_power_value);
		zcu102_get_power(MGTRAVCC,     &ps_mgtravcc_power_value);
		zcu102_get_power(MGTRAVTT,     &ps_mgtravtt_power_value);
		zcu102_get_power(VCCOPS,       &ps_vcc0_power_value);
		zcu102_get_power(VCCOPS3,      &ps_vcc03_power_value);
		zcu102_get_power(VCCPSDDRPLL,  &ps_ddrpl_power_value);
		zcu102_get_power(V2_0046,      &ps_0046_power_value);

		//pl rails
		zcu102_get_power(VCCINT,       &pl_vccint_power_value);
		zcu102_get_power(VCCBRAM,      &pl_bram_power_value);
		zcu102_get_power(VCCAUX,       &pl_aux_power_value);
		zcu102_get_power(VCC1V2,       &pl_1V2_power_value);
		zcu102_get_power(VCC3V3,       &pl_3V3_power_value);
		zcu102_get_power(VADJ_FMC,     &pl_vadj_power_value);
		zcu102_get_power(MGTAVCC,      &pl_mgtavcc_power_value);
		zcu102_get_power(MGTAVTT,      &pl_mgtavtt_power_value);


		

		//ps rails
		ps_full_power_total += ps_full_power_value;
		ps_low_power_total += ps_low_power_value;
		ps_aux_power_total += ps_aux_power_value;
		ps_pll_power_total += ps_pll_power_value;
		ps_mgtravcc_power_total += ps_mgtravcc_power_value;
		ps_mgtravtt_power_total += ps_mgtravtt_power_value;
		ps_vcc0_power_total += ps_vcc0_power_value;
		ps_vcc03_power_total += ps_vcc03_power_value;
		ps_ddrpl_power_total += ps_ddrpl_power_value;
		ps_0046_power_total += ps_0046_power_value;

		//pl rails
		pl_vccint_power_total += pl_vccint_power_value;
		pl_bram_power_total += pl_bram_power_value;
		pl_aux_power_total += pl_aux_power_value;
		pl_1V2_power_total += pl_1V2_power_value;
		pl_3V3_power_total += pl_3V3_power_value;
		pl_vadj_power_total += pl_vadj_power_value;
		pl_mgtavcc_power_total += pl_mgtavcc_power_value;
		pl_mgtavtt_power_total += pl_mgtavtt_power_value;


		sample_count++;



		stop_flag = 0;
		pthread_mutex_lock(&mutex);
		if (stop == 1) {
			stop_flag = 1;
		}
		pthread_mutex_unlock(&mutex);

		if(stop_flag==1) {
			break;
		}
	}
	fclose(fp_log);
	return 0;
}

void *zcu102_read_sample_pthread() {

	pthread_mutex_init(&mutex, NULL);

	pthread_mutex_lock(&mutex);
	stop = 0;
	start = 0;
	pthread_mutex_unlock(&mutex);

	head = (struct zcu102_sample *)malloc(sizeof (struct zcu102_sample));
	head->next = NULL;


	cpu_set_t cpu_set2;
	CPU_SET(1, &cpu_set2);
	pthread_create(&thread_ID, NULL, zcu102_read_samples, head);
	sched_setaffinity(thread_ID, sizeof(cpu_set_t), &cpu_set2);

//	pthread_join(thread_ID, &exit_status);


	return head;
}


void zcu102_read_sample_start( ) {

	pthread_mutex_lock(&mutex);

	sample_count = 0;


//	zcu102_open_device();
	//ps rails

	ps_full_power_total=0;
	ps_low_power_total=0;
	ps_aux_power_total=0;
	ps_pll_power_total=0;
	ps_mgtravcc_power_total=0;
	ps_mgtravtt_power_total=0;
	ps_vcc0_power_total=0;
	ps_vcc03_power_total=0;
	ps_ddrpl_power_total=0;
	ps_0046_power_total=0;

	//pl rails
	pl_vccint_power_total=0;
	pl_bram_power_total=0;
	pl_aux_power_total=0;
	pl_1V2_power_total=0;
	pl_3V3_power_total=0;
	pl_vadj_power_total=0;
	pl_mgtavcc_power_total=0;
	pl_mgtavtt_power_total=0;


	/*
	ps_full_power = 0;
	ps_low_power = 0;
	ps_aux_power = 0;
	ps_pll_power = 0;
	ps_mgtravcc_power = 0;
	ps_mgtravtt_power = 0;
	ps_vcc0_power = 0;
	ps_vcc03_power = 0;
	ps_ddrpl_power = 0;
	ps_0046_power = 0;

	//pl rails
	pl_vccint_power = 0;
	pl_bram_power = 0;
	pl_aux_power = 0;
	pl_1V2_power = 0;
	pl_3V3_power = 0;
	pl_vadj_power = 0;
	pl_mgtavcc_power = 0;
	pl_mgtavtt_power = 0;


	*/



	start = 1;
	stop  = 0;
	pthread_mutex_unlock(&mutex);
}


void zcu102_read_sample_stop( ) {

	pthread_mutex_lock(&mutex);
	stop = 1;
	start = 0;
	pthread_mutex_unlock(&mutex);
	pthread_join(thread_ID, &exit_status);
	pthread_detach(thread_ID);
//	zcu102_close_device();
}



void zcu102_save_average_pthread(struct zcu102_sample *head, char *file_name) {

	FILE *fp;


//	struct zcu102_sample *sample = head;

	fp=fopen(file_name, "a");

	//ps rails
	fprintf(fp, "ps_full_power_average \t\t\t= %f \t\tcount = %llu\n",   ps_full_power_total/sample_count, sample_count);
	fprintf(fp, "ps_low_power_average \t\t\t= %f \t\tcount = %llu\n",   ps_low_power_total/sample_count, sample_count);
	fprintf(fp, "ps_aux_power_average \t\t\t= %f \t\t\tcount = %llu\n",   ps_aux_power_total/sample_count, sample_count);
	fprintf(fp, "ps_pll_power_average \t\t\t= %f \t\t\tcount = %llu\n",   ps_pll_power_total/sample_count, sample_count);
	fprintf(fp, "ps_mgtravcc_power_average \t\t= %f \t\t\tcount = %llu\n",   ps_mgtravcc_power_total/sample_count, sample_count);
	fprintf(fp, "ps_mgtravtt_power_average \t\t= %f \t\t\tcount = %llu\n",   ps_mgtravtt_power_total/sample_count, sample_count);
	fprintf(fp, "ps_vcc0_power_average \t\t\t= %f \t\t\tcount = %llu\n",   ps_vcc0_power_total/sample_count, sample_count);
	fprintf(fp, "ps_vcc03_power_average \t\t\t= %f \t\t\tcount = %llu\n",   ps_vcc03_power_total/sample_count, sample_count);
	fprintf(fp, "ps_ddrpl_power_average \t\t\t= %f \t\t\tcount = %llu\n",   ps_ddrpl_power_total/sample_count, sample_count);
	fprintf(fp, "ps_0046_power_average \t\t\t= %f \t\t\tcount = %llu\n",   ps_0046_power_total/sample_count, sample_count);
	    
	//pl rails
	fprintf(fp, "pl_vccint_power_average \t\t= %f \t\tcount = %llu\n",   pl_vccint_power_total/sample_count, sample_count);
	fprintf(fp, "pl_power_average \t\t\t= %f \t\t\tcount = %llu\n",   pl_bram_power_total/sample_count, sample_count);
	fprintf(fp, "pl_aux_power_average \t\t\t= %f \t\tcount = %llu\n",   pl_aux_power_total/sample_count, sample_count);
	fprintf(fp, "pl_1V2_power_average \t\t\t= %f \t\t\tcount = %llu\n",   pl_1V2_power_total/sample_count, sample_count);
	fprintf(fp, "pl_3V3_power_average \t\t\t= %f \t\t\tcount = %llu\n",   pl_3V3_power_total/sample_count, sample_count);
	fprintf(fp, "pl_vadj_power_average \t\t\t= %f \t\t\tcount = %llu\n",   pl_vadj_power_total/sample_count, sample_count);
	fprintf(fp, "pl_mgtavcc_power_average \t\t= %f \t\t\tcount = %llu\n",   pl_mgtavcc_power_total/sample_count, sample_count);
	fprintf(fp, "pl_mgtavtt_power_average \t\t= %f \t\t\tcount = %llu\n",   pl_mgtavtt_power_total/sample_count, sample_count);
	


	fclose(fp);
}

void zcu102_clear_sample_pthread(struct zcu102_sample *head) {

	struct zcu102_sample *sample = head;
	while (sample != (struct zcu102_sample *)NULL) {
		struct zcu102_sample *next = sample->next;
		free (sample);
		sample = next;
	}
	pthread_mutex_destroy(&mutex);
}
