/*
@author: darwinzh
*/
#include "main.h"
#include <signal.h>


/* En esta parte se edito desde raspberry v2*/

uint8_t bNrf, bTog, bMrx,bInit;

uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};

uint8_t txEnv[8];
uint8_t rxRec[8];

struct tm *ptr;
time_t t;

static uint8_t run;

void interrupcion(void);

//Catch Ctrl C
void intHandler(int dummy){
	Led_SetLow();
	RF24L01_powerDown();
	run = 0;
}


int main(){
	bNrf = 3;
	bTog = 1;
	bInit = 0;
	bMrx = 1;
	run = 1;

	RF24L01_init();

	RF24L01_setup(tx_addr, rx_addr, 22);

	Led_SetOutput();

	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);

	signal(SIGINT,intHandler);

	while(run){
		if(bTog){
			bTog = 0;
			Led_SetHigh();
			delay(250);
		}else{
			bTog = 1;
			Led_SetLow();
			delay(250);
		}

		// Get time
		t = time(NULL);
		ptr = localtime(&t);

		//Prepare the buffer to send from the data_to_send struct
		txEnv[0] = ptr->tm_sec;
		txEnv[1] = ptr->tm_min;
		txEnv[2] = ptr->tm_hour;
		txEnv[3] = (ptr->tm_wday)+1;
		txEnv[4] = (ptr->tm_mon)+1;
		txEnv[5] = ptr->tm_mday;
		txEnv[6] = ptr->tm_year-100;
		txEnv[7] = 0x48;

		switch(bNrf){
			case 1:
				bNrf = 0;
				if(bMrx==1){
					bMrx=0;
					RF24L01_set_mode_RX();
					printf("-----Mode  RX-----\nEsperando  Dato...\n");
				}else{
					printf("\nData Rady\n");
					printf("RC:%d:%d:%d\n",rxRec[3],rxRec[2],rxRec[1]);
				}
				break;
			case 2:
				bNrf = 3;
				printf("Data Sent\n");
				delayMicroseconds(130);
				break;
			case 3:
				printf("CPU %d:%d:%d\n", txEnv[2], txEnv[1], txEnv[0]);
				sendData(txEnv, sizeof(txEnv));
				delay(1000);
				break;
			default:
				break;
		}

	}//End while
	return 0;
}//End Main


void interrupcion(){
	// Return 1:Data Sent, 2:RX_DR, 3:MAX_RT
	bNrf = RF24L01_status();

	if(bNrf){
		RF24L01_read_payload(rxRec, sizeof(rxRec));
		delayMicroseconds(130);
		RF24L01_clear_interrupts();
		return;
	}

	RF24L01_clear_interrupts();
}

