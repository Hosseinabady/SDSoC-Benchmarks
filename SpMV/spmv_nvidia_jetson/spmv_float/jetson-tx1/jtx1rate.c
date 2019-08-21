/**
 * @file jtx1rate.c
 * @author cs
 * @brief This file contains the implementations of functions 
 * for setting and reading Jetson TX1's GPU and EMC frequncies.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUFF 128

#include "jtx1rate.h"

/**
 * @brief Set operation frequency of graphics processing unit (GPU)
 * @param gpu_rate An operation frequency
 */
static void jtx1_set_gpu_rate(const unsigned long gpu_rate)
{
    int val = 1;
    FILE *fp[2];
  
    fp[0] = fopen("/sys/kernel/debug/clock/override.gbus/rate", "w");
    
    if (fp[0] == NULL) {
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else if (!fprintf(fp[0], "%lu", gpu_rate)) {
	fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else {
	fclose(fp[0]);
    }

    fp[1] = fopen("/sys/kernel/debug/clock/override.gbus/state", "w");

    if (fp[1] == NULL) {
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else if (!fprintf(fp[1], "%u", val)) {
	fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else {
	fclose(fp[1]);
    }
    
}

/**
 * @brief Set operation frequency of external memory controller (EMC)
 * @param emc_rate See ::jtx1_emc_freqs
 */
static void jtx1_set_emc_rate(const unsigned long emc_rate)
{
    int val = 1;
    FILE *fp[2];
  
    fp[0] = fopen("/sys/kernel/debug/clock/override.emc/rate", "w");

    if (fp[0] == NULL) {
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else {
	fprintf(fp[0], "%lu", emc_rate);
	fclose(fp[0]);
    }

    fp[1] = fopen("/sys/kernel/debug/clock/override.emc/state", "w");

    if (fp[1] == NULL) {
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else {
	fprintf(fp[1], "%u", val);
	fclose(fp[1]);
    }

}

/**
 * @brief Set operation frequency of central processing unit (CPU)
 * @param cpu_rate An operation frequency
 * @param cpu Core selection
 */
static void jtx1_set_cpu_rate(const unsigned long cpu_rate, const unsigned int cpu)
{
    FILE *fp[2];
    char buff[2][MAX_BUFF];

    snprintf(buff[0], sizeof(buff[0]), "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor", cpu);
    fp[0] = fopen(buff[0], "w");
    
    if (fp[0] == NULL) {
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else {
	fprintf(fp[0], "%s", "userspace");
	fclose(fp[0]);
    }

    snprintf(buff[1], sizeof(buff[1]), "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_setspeed", cpu);
    fp[1] = fopen(buff[1], "w");

    if (fp[1] == NULL) {
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else {
	fprintf(fp[1], "%lu", cpu_rate);
	fclose(fp[1]);
    }
}

/**
 * @brief Read operating frequency
 * @param unit See ::jtx1_unit
 * @param *rate Output's reference
 */
void jtx1_get_rate(const jtx1_unit unit, unsigned long *rate)
{
    FILE *fp;
    unsigned long ra;
    fp = fopen("", "r");

    switch (unit) {
    case EMC_RATE: {
	fp = fopen("/sys/kernel/debug/clock/emc/rate", "r");
	break;
    }
    case GPU_RATE: {
	fp = fopen("/sys/kernel/debug/clock/gbus/rate", "r");
	break;
    }
    case CPU0_RATE: {
	fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
	break;
    }
    case CPU1_RATE: {
	fp = fopen("/sys/devices/system/cpu/cpu1/cpufreq/scaling_cur_freq", "r");
	break;
    }
    case CPU2_RATE: {
	fp = fopen("/sys/devices/system/cpu/cpu2/cpufreq/scaling_cur_freq", "r");
	break;
    }
    case CPU3_RATE: {
	fp = fopen("/sys/devices/system/cpu/cpu3/cpufreq/scaling_cur_freq", "r");
	break;
    }
    default:
	break;
    }

    if (fp == NULL) {
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else if (!fscanf(fp, "%lu", &ra)) {
	fprintf(stderr, "Error scaning file: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else {
	fclose(fp);
    }

    *rate = ra;
}

/**
 * @brief Set operating frequency
 * @param unit See ::jtx1_unit
 * @param rate Operating frequency 
 */
void jtx1_set_rate(const jtx1_unit unit, const unsigned long rate)
{
    switch (unit) {
    case EMC_RATE: {
	jtx1_set_emc_rate(rate);
	break;
    }
    case GPU_RATE: {
	jtx1_set_gpu_rate(rate);
	break;
    }
    case CPU0_RATE: {
	jtx1_set_cpu_rate(rate, 0);
	break;
    }
    case CPU1_RATE: {
	jtx1_set_cpu_rate(rate, 1);
	break;
    }
    case CPU2_RATE: {
	jtx1_set_cpu_rate(rate, 2);
	break;
    }
    case CPU3_RATE: {
	jtx1_set_cpu_rate(rate, 3);
	break;
    }
    default:
	break;
    }
}
