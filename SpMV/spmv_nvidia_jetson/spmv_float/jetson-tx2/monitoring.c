#include "monitoring.h"
#include "jetson_tx2_power.h"



struct jetsontx2_sample *head;

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


	head = (struct jetsontx2_sample *)jetsontx2_read_sample_pthread();

	jetsontx2_read_sample_start();
}


void power_monitoring_stop() {
	jetsontx2_read_sample_stop();
}
void power_monitoring_epilogue() {
	power_monitoring_stop();
	jetsontx2_save_average_pthread(head, log_file_name);
	jetsontx2_clear_sample_pthread(head);
}


