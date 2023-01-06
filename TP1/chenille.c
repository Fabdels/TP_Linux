#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ADDR_PATTERN "/sys/class/leds/fpga_led%d/brightness"

void setLED(int ledID, int state){
	char addrBuffer[strlen(ADDR_PATTERN)];
	sprintf(addrBuffer, ADDR_PATTERN, ledID);
	FILE * fileID = fopen(addrBuffer, "w");
	if (fileID == NULL){
		printf("error while opening file\n");
		exit(-1);
	}
	else{
		fprintf(fileID, "%d", state);
	}

	fclose(fileID);
}

#define NB_LED 10

void chenillard(int delay){
	int ledID = 0, ledIDpast, sensChenille;
	for(;;){
		if (ledID <=0){
			sensChenille = 1;
		}
		else if (ledID >= NB_LED - 1)
		{
			sensChenille = -1;
		}
		ledIDpast = ledID;
		ledID += sensChenille;

		setLED(ledIDpast, 0);
		setLED(ledID, 1);
		usleep(delay);
	}
}

int main(int argc, char const *argv[])
{
	chenillard(50000);
	return 0;
}