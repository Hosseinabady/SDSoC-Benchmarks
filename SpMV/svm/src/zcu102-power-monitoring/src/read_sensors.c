#include "read_sensors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


#define DEVICE_NO 18
//int fp[DEVICE_NO];
int fp;


char *sensors[]={
		"/sys/bus/i2c/drivers/ina2xx/1-0040/hwmon/hwmon0/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-0041/hwmon/hwmon1/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-0042/hwmon/hwmon2/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-0043/hwmon/hwmon3/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-0044/hwmon/hwmon4/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-0045/hwmon/hwmon5/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-0046/hwmon/hwmon6/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-0047/hwmon/hwmon7/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-004a/hwmon/hwmon8/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/1-004b/hwmon/hwmon9/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0040/hwmon/hwmon10/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0041/hwmon/hwmon11/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0042/hwmon/hwmon12/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0043/hwmon/hwmon13/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0044/hwmon/hwmon14/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0045/hwmon/hwmon15/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0046/hwmon/hwmon16/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/2-0047/hwmon/hwmon17/power1_input"
};
/*
int zcu102_open_device() {
	printf("check point 1 zcu102_open_device \n");
	for (int r = 0; r < DEVICE_NO; r++) {
		fp[r]=open(sensors[r],O_RDONLY);
		if (fp[r] == 0) {
			printf("Error opening device %s\n", sensors[r]);
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
*/

int zcu102_get_power(rails r, unsigned long int *power_value) {

	char buf[256];
	unsigned long int value;

	fp=open(sensors[r],O_RDONLY);
	if (fp == 0) {
		printf("Error opening device %s\n", sensors[r]);
		exit(EXIT_FAILURE);
	}
	read(fp,&buf,256);
	sscanf(buf," %lu ", &value);
  	*(power_value) = value;
	usleep(500);
	close(fp);
	return 0; 
}


/*
int zcu102_close_device() {
	printf("check point 1 zcu102_close_device \n");
	for (int r = 0; r < DEVICE_NO; r++) {
		close(fp[r]);
	}
return 0;
}
*/
