#include "main.h"

uint8_t mutex;
uint8_t bandera;
uint8_t tx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
uint8_t rx_addr[5] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};

typedef struct _data_to_send{
	uint32_t op1;
	uint32_t op2;
}data_to_send;
data_to_send to_send;

typedef struct _data_received{
	uint32_t add;
	uint32_t sub;
	uint32_t mult;
	uint32_t div;
}data_received;
data_received received;

void interrupcion(void);

int main(){
	unsigned char i;
	bandera = 0;

	RF24L01_init();
	//RF24L01_clear_setup();
	RF24L01_setup(tx_addr, rx_addr, 12);

	Led_SetOutput();
	wiringPiISR(RF_IRQ , INT_EDGE_FALLING, interrupcion);

	Led_SetLow();
	to_send.op1 = 1;
	to_send.op2 = 1;

	//Prepare the buffer to send from the data_to_send struct
	uint8_t buffer_to_send[32];

	for (i = 0; i < 32; i++){
		buffer_to_send[i] =0xAA;
	}
	*((data_to_send *) &buffer_to_send) = to_send;

	while(1){
		//send the buffer
		mutex = 0;
		RF24L01_set_mode_TX();
		RF24L01_write_payload(buffer_to_send, 32);
		//Wait for the buffer to be sent
		printf("Wait for the buffer to be sent...\n");
		while(!mutex);
		if(mutex != 1){
			printf("The transmission failed\n");
			return 0;
		}
		printf("Transmision Completada.\n");

		//Wait for the response
		/** Todo: implement a  timeout if nothing is received after a certain amoun of time*/
		printf("Wait for the response\n");
		mutex = 0;
		RF24L01_set_mode_RX();
		while(!mutex);
		if(mutex == 1){
			uint8_t recv_data[32];
			RF24L01_read_payload(recv_data, 32);
			received = *((data_received *) &recv_data);

			asm("nop");//Place a breakpoint here to see memory
		}else{
			printf("Something happened\n");
		}

		//Let's vary the data to send
		to_send.op1++;
		to_send.op2 += 2;
	}
}


void interrupcion(){
	uint8_t sent_info;

	if(bandera == 0)
		Led_SetHigh();
	else
		Led_SetLow();

	bandera = !bandera;

	if((sent_info = RF24L01_was_data_sent())){
		//Packet was sent or max retrie reached
		mutex = sent_info;
		RF24L01_clear_interrupts();
		return;
	}

	if(RF24L01_is_data_available()){
		//Packer was received
		mutex = 1;
		RF24L01_clear_interrupts();
		return;
	}


	RF24L01_clear_interrupts();
}
