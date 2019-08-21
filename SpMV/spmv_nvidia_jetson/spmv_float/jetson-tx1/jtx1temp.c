/**
 * @file jtx1temp.c
 * @author cs
 * @brief This file contains the implementations of functions 
 * for reading Jetson TX1's values of thermal zones.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "jtx1temp.h"

/**
 * @brief Read on-chip and on-module temperatures.
 *
 * @param zone Indexed by ::jtx1_tzone
 * @param *temp Reference to variable storing temperature
 */
void jtx1_get_temp(jtx1_tzone zone, unsigned int *temp)
{
    FILE *fp;
    char buff[MAX_BUFF];
    unsigned long ans;
    
    snprintf(buff, sizeof(buff), SYSFS_TEMP_PATH "/thermal_zone%d/temp", zone);

    fp = fopen(buff, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (!fscanf(fp, "%lu", &ans)) {
        fprintf(stderr, "Error scanning the file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        fclose(fp);
    }
    
    *temp = ans;
}
