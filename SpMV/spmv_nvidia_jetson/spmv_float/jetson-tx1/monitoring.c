#include "monitoring.h"




struct jetsontx1_sample *head;

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

	strcat(file_name, "_Jetson_TX1");



	strcat(file_name, ".txt");


	FILE *fp=fopen(file_name, "a");
	fclose(fp);

}



void power_monitoring_prologue () {

	//struct jetsontx1_sample *sample = head;
	head = (struct jetsontx1_sample *)jetsontx1_read_sample_pthread();

	jetsontx1_read_sample_start();
}


void power_monitoring_stop() {
	jetsontx1_read_sample_stop();
}
void power_monitoring_epilogue() {
	power_monitoring_stop();
	jetsontx1_save_average_pthread(head, log_file_name);
	jetsontx1_clear_sample_pthread(head);
}


