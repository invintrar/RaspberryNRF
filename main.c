/*
@author: darwinzh
*/
#include "main.h"
#include <signal.h>


/* En esta parte se edito desde raspberry v2*/

uint8_t bNrf, bTog, bMrx, bInit;
uint16_t sensor;

uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};

//Data sent or receive Nrf24L01 +
uint8_t txEnv[14];
uint8_t rxRec[14];

uint32_t valueX;
uint32_t valueY;
uint32_t valueZ;

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

float fnabs(float a){
		if(a<0)
				a=-a;
		return a;
}

int main(){
	float corriente, promedio;
	float voltajeS;
	uint8_t ipr;
	bNrf = 1;
	bTog = 1;
	bInit = 0;
	bMrx = 1;
	run = 1;
	ipr=promedio=0;

	//Setting Port CE and SPI
	RF24L01_init();

	//Setting address nrf and channel 
	RF24L01_setup(tx_addr, rx_addr, 22);

	Led_SetOutput();

	//Setting Interrupt
	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);



	//Catch Ctrl-C
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
		txEnv[8] = 0x48;
		txEnv[9] = 0x48;
		txEnv[10] = 0x48;
		txEnv[11] = 0x48;
		txEnv[12] = 0x48;
		txEnv[13] = 0x48;


		switch(bNrf){
			case 1:
				if(bMrx==1){
					bNrf =0;
					bMrx=0;
					RF24L01_set_mode_RX();
					printf("-----Mode  RX-----\nEsperando  Dato...\n");
				}else{
					bNrf = 0;
				    printf("\nData Rady\n");
					printf("RC:%d:%d:%d\n",rxRec[2],rxRec[1],rxRec[0]);
					sensor=(rxRec[4]<<8) | rxRec[3];
					voltajeS = sensor*3.3/1023;
					corriente = (fnabs(voltajeS - 1.65)/0.0132)+2;
					printf("Consumo:%.2f mA\n",corriente);
					valueX = (rxRec[5]<<12) | (rxRec[6]<<4) | (rxRec[7]>>4);
					valueY = (rxRec[8]<<12) | (rxRec[9]<<4) | (rxRec[10]>>4);
					valueZ = (rxRec[11]<<12) | (rxRec[12]<<4) | (rxRec[13]>>4);
					printf("X:%ld Y:%ld Z:%ld\n",valueX, valueY, valueZ);
				}
				break;
			case 2:
				bNrf = 1;
				bMrx = 1;
				printf("Data Sent\n");
				break;
			case 3:
				bNrf= 0;
				printf("CPU %d:%d:%d\n", txEnv[2], txEnv[1], txEnv[0]);
				sendData(txEnv, sizeof(txEnv));
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
		RF24L01_clear_interrupts();
		return;
	}

	RF24L01_clear_interrupts();
}

