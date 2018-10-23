/* File: monitoring.c
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
* File name : monitoring.c
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 31 August 2018
* blog: https://highlevel-synthesis.com/
*/

#include "monitoring.h"




struct zcu102_sample *head;

char log_file_name[1000];
void file_power_profile_create (char *file_name) {
	time_t t;
	time(&t);
	strcat(file_name, ctime(&t));
	int charIndex = 0;
	while(file_name[charIndex] != '\0') {
		if (file_name[charIndex] == ' ' || file_name[charIndex] == ':')
			file_name[charIndex]='_';
		charIndex++;
	}
	file_name[charIndex-1] = '\0';

	strcat(file_name, "_ZCU102");
	strcat(file_name, ".txt");


	FILE *fp=fopen(file_name, "a");
	fclose(fp);

}



void power_monitoring_prologue () {

	//struct zcu102_sample *sample = head;
	head = (struct zcu102_sample *)zcu102_read_sample_pthread();

	zcu102_read_sample_start();
}


void power_monitoring_stop() {
	zcu102_read_sample_stop();
}
void power_monitoring_epilogue() {
	power_monitoring_stop();
	zcu102_save_average_pthread(head, log_file_name);
	zcu102_clear_sample_pthread(head);
}
