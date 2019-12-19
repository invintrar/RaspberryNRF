/*
@author: darwinzh
*/
#include "main.h"

/* En esta parte se edito desde raspberry v2*/

uint8_t statusNrf, i, bandera, mutex;
uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};

uint8_t txEnv[8];
uint8_t rxRec[8];

struct tm *ptr;
time_t t;

void interrupcion(void);

int main(){

	bandera = 0;
	statusNrf = 0;
	mutex = 1;

	RF24L01_init();

	RF24L01_setup(tx_addr, rx_addr, 12);

	Led_SetOutput();

	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);

	Led_SetLow();


	while(1){
		//Obtener el tiempo
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


		/*Set Mode TX */
		RF24L01_set_mode_TX();

		/*Write Payload*/
		RF24L01_write_payload(txEnv, sizeof(txEnv));

		while(mutex);

		switch(statusNrf){
			case 1:
				printf("ACK is received\n\n");
				break;
			case 2:
				printf("Maximum number of TX retrasnmits\n\n");
				delay(10);
				break;
			case 3:
				printf("Dato recibido\n");
				RF24L01_read_payload(rxRec, sizeof(rxRec));
				break;
			defaul:
				printf("Ninguna Dato recibido\n");
				delay(10);
				break;
		}
	}
}


void interrupcion(){
	// For change Led
	if(bandera == 0)
		Led_SetHigh();
	else
		Led_SetLow();

	mutex = 0;

	bandera = !bandera;

	statusNrf = RF24L01_was_data_sent();

	RF24L01_clear_interrupts();
}
