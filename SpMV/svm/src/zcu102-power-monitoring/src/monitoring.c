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
