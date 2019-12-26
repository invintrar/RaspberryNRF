/*
*
*Author: darwinzh
*
*/
#include "nrf24l01.h"

/*------------------ Function Prototype ----------*/

void RF24L01_send_command(uint8_t command);
void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length);
uint8_t RF24L01_read_register(uint8_t register_addr);
RF24L01_reg_STATUS_content RF24L01_get_status(void);
void ClearStatus(void);
void WritePayload(uint8_t value[], uint8_t size);
/*------------------ Functions -----------------*/


/** Function init  */
void RF24L01_init(void) {
  wiringPiSetup();

  //CE
  RF24L01_CE_setOutput();
  RF24L01_CE_setLow();

  //SPI
  if( wiringPiSPISetup(0, 10000000) == -1 )
  	printf("SPI0 No se Inicia Correctamente.\n");

}//End init



/** Función Setup */
void RF24L01_setup(uint8_t *tx_addr, uint8_t *rx_addr, uint8_t channel) {
  RF24L01_CE_setLow(); //CE -> Low

 // Enable Auto Acknowledgment(0x01)
  RF24L01_reg_EN_AA_content EN_AA;
  *((uint8_t *)&EN_AA) = 0;
  // Enable Auto ACK: Pipe0
  EN_AA.ENAA_P0 = 1;
  RF24L01_write_register(RF24L01_reg_EN_AA, ((uint8_t *)&EN_AA), 1);

  // Enable RX Address(0x02)
  RF24L01_reg_EN_RXADDR_content RX_ADDR;
  *((uint8_t *)&RX_ADDR) = 0;
  //Enable data Pipe0
  RX_ADDR.ERX_P0 = 1;
  RF24L01_write_register(RF24L01_reg_EN_RXADDR, ((uint8_t *)&RX_ADDR), 1);

  // Setup of Addres Widths(0x03)
  RF24L01_reg_SETUP_AW_content SETUP_AW;
  *((uint8_t *)&SETUP_AW) = 0;
  //RX/TX Address field width('11' - 5 bytes)
  SETUP_AW.AW = 0x03;
  RF24L01_write_register(RF24L01_reg_SETUP_AW, ((uint8_t *)&SETUP_AW), 1);

  // Setup of Automatic Retransmission(0x04)
  RF24L01_reg_SETUP_RETR_content SETUP_RETR;
  *((uint8_t *)&SETUP_RETR) = 0;
  // Auto Retransmit Count(Up to 10 Re-Transmit on fail of AA)
  SETUP_RETR.ARC = 0x0A;
  //Auto Retransmit Delay(wait 500uS)
  SETUP_RETR.ARD = 0x01;
  RF24L01_write_register(RF24L01_reg_SETUP_RETR, ((uint8_t *)&SETUP_RETR), 1);

  // RF Channel(0x05)
  RF24L01_reg_RF_CH_content RF_CH;
  *((uint8_t *)&RF_CH) = 0;
  //Sets the frequency channel nRF24L01 operates on
  RF_CH.RF_CH = channel;
  RF24L01_write_register(RF24L01_reg_RF_CH, ((uint8_t *)&RF_CH), 1);

  // RF Setup Register(0x06)
  RF24L01_reg_RF_SETUP_content RF_SETUP;
  *((uint8_t *)&RF_SETUP) = 0;
  //Set RF output power in TX mode('11' - 0dBm)
  RF_SETUP.RF_PWR     = 0x03;
  //Speed data rata 2Mbps
  RF_SETUP.RF_DR_HIGH = 0x01;
  RF24L01_write_register(RF24L01_reg_RF_SETUP, ((uint8_t *)&RF_SETUP), 1);

  // Escribimos la direccion RX en Pipe0(0x0A)
  RF24L01_write_register(RF24L01_reg_RX_ADDR_P0, rx_addr, 5);

  // Escribimos la direccion en TX(0x10)
  RF24L01_write_register(RF24L01_reg_TX_ADDR, tx_addr, 5);

  // RX payload in data pipe0 (0x11) */
  RF24L01_reg_RX_PW_P0_content RX_PW_P0;
  *((uint8_t *)&RX_PW_P0) = 0;
  //Number of bytes in RX payload in data Pipe0 (8 bytes).
  RX_PW_P0.RX_PW_P0 = 0x08;
  RF24L01_write_register(RF24L01_reg_RX_PW_P0, ((uint8_t *)&RX_PW_P0), 1);

  // Configuration Register(0x00)
  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  //RX/TX control(1: PRX, 0:PTX)
  config.PRIM_RX = 1;
  //1: Power up, 0: Power Down
  config.PWR_UP  = 1;
  //CRC (0-1 byte, 1-2 bytes)
  config.CRCO    = 1;
  //Enable CRC. Forced high if one of the bits in the EN_AA is high
  config.EN_CRC  = 1;
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  // Delay 2 ms
  delay(2);
  RF24L01_CE_setHigh(); //CE -> High
  // Delay 150 us
  delayMicroseconds(150);


}//End Setup



/** Function send command */
void RF24L01_send_command(uint8_t command) {
  uint8_t aux[2];
  aux[0] = command;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
}//End Send Command



/** Function Write Register */
void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length) {
  uint8_t i, aux[length+1];
  //Address and write command
  aux[0] = RF24L01_command_W_REGISTER | register_addr;
  for (i = 1; i < (length+1); i++) {
  	aux[i] = value[i-1];
  }
   //Send data
   wiringPiSPIDataRW( CHANNEL, aux, (length+1) );
}//End Werite Register



// Function Write Payload
void WritePayload(uint8_t value[], uint8_t size){
	uint8_t i, aux[size+1];
	aux[0]= RF24L01_command_W_TX_PAYLOAD;
	for(i = 1; i< (size+1); i++){
		aux[i]= value[i-1];
	}
	//Send for SPI Data
	wiringPiSPIDataRW(CHANNEL, aux, (size+1));
}//End Write Payload



/** Function read register */
uint8_t RF24L01_read_register(uint8_t register_addr) {
  //Send address and read command
  aux[0] = RF24L01_command_R_REGISTER | register_addr;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);

  return aux[1];

}//End Read Register



/** Function Set Mode TX */
void RF24L01_set_mode_TX(void) {
  RF24L01_send_command(RF24L01_command_FLUSH_TX);

  //CE->LOW
  RF24L01_CE_setLow();

  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  config.PRIM_RX 	 = 0;//PTX
  config.PWR_UP  	 = 1;//Power Up
  config.CRCO    	 = 1;//Enconding 2 Bytes
  config.EN_CRC      = 1;//Enable CRC
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  //Clear the status register to discard any data in the buffers
  ClearStatus();

}//End Set Mode TX



/** Function Set Mode RX */
void RF24L01_set_mode_RX(void) {
  // CE->Low
  RF24L01_CE_setLow();

  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  // RX/TX Control (1:PRX, 0:PTX)
  config.PRIM_RX 	 = 1;
  // 1:PowerUp, 0:Power Down
  config.PWR_UP 	 = 1;
  // Encode (0:1Byte, 1:2Bytes
  config.CRCO    	 = 1;
  // Enable CRC
  config.EN_CRC 	 = 1;
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  //CE -> High
  RF24L01_CE_setHigh();
  // Delay 150 us
  delayMicroseconds(150);

}//End Set Mode RX



// Function Send Data
void sendData(uint8_t data[],uint8_t size ){
	//CE->Low
	RF24L01_CE_setLow();

	//CLEAR STATUS
	ClearStatus();

	//W_TX_PAYLOAD
	WritePayload(data, size);

	//CONFIG  TX Mode
	RF24L01_reg_CONFIG_content config;
	*((uint8_t *)&config) = 0;
	// RX/TX Control (1:PRX, 0:PTX)
	config.PRIM_RX 	 = 0;
	// 1:PowerUp, 0:Power Down
	config.PWR_UP 	 = 1;
	// Encode (0:1Byte, 1:2Bytes
	config.CRCO    	 = 1;
	// Enable CRC
	config.EN_CRC 	 = 1;
    RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

    RF24L01_CE_setHigh();
    // Delay 15us
    delayMicroseconds(15);

}//End Fuction Send Data



/** Function Get Status  */
RF24L01_reg_STATUS_content RF24L01_get_status(void) {
  uint8_t status;
  uint8_t aux[2];

  //Send address and command
  aux[0] = RF24L01_reg_STATUS;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
  status = aux[1];

  return *((RF24L01_reg_STATUS_content *) &status);

}//End Get Status



/** Function Write Payload */
void RF24L01_write_payload(uint8_t *data, uint8_t length) {
  RF24L01_CE_setLow();
  uint8_t aux[33], i;

  RF24L01_send_command(RF24L01_command_FLUSH_TX);

  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  if (a.MAX_RT == 1) {
    //If MAX_RT, clears it so we can send data
    *((uint8_t *) &a) = 0;
    a.RX_DR = 1;
    a.TX_DS = 1;
    a.MAX_RT = 1;
    RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t *) &a, 1);
  }

  //Send address and command
  aux[0] = RF24L01_command_W_TX_PAYLOAD;

  //Send data
  for(i = 1; i < length+1; i++){
  	aux[i] = data[i-1];
  }

  wiringPiSPIDataRW(CHANNEL, aux, sizeof(aux));

  RF24L01_send_command(RF24L01_command_W_ACK_PAYLOAD);

  uint16_t delay = 0x28;
  //Generates an impulsion for CE to send the data
  RF24L01_CE_setHigh();
  //delay = 0xFA;
  //while(delay--);
  //RF24L01_CE_setLow();

  //RF24L01_CE_setHigh();

}//End Write Payload



/** Function Read Payload  */
void RF24L01_read_payload(uint8_t *data, uint8_t length) {
  uint8_t i;

  //Command of Read RX Payload
  aux[0] = RF24L01_command_R_RX_PAYLOAD;

  //Get data
  for (i=1; i < length+1; i++) {
  	aux[i] = 0x00;
  }

  wiringPiSPIDataRW(CHANNEL, aux, sizeof(aux));

  for(i = 0; i<length; i++){
  	*(data++) = aux[i+1];
  }

  RF24L01_send_command(RF24L01_command_FLUSH_RX);

}//End Read Payload



/** Function Was Data Sent */
uint8_t RF24L01_was_data_sent(void) {
  uint8_t res = 0;
  RF24L01_reg_STATUS_content a;

  a = RF24L01_get_status();

  if (a.TX_DS) {
    res = 2;
  }
  else if (a.MAX_RT) {
    res = 3;
  }

  return res;

}//End Was Data Sent


uint8_t RF24L01_status(void){
	uint8_t res = 0;
	RF24L01_reg_STATUS_content status;

	status = RF24L01_get_status();

	if(status.TX_DS){
		res = 1;
	}else if(status.RX_DR){
		res = 2;
	}else if(status.MAX_RT){
		res = 3;
	}

	return res;
}


/** Function Is Data Available */
uint8_t RF24L01_is_data_available(void) {
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();

  return a.RX_DR;

}//End Is Data Available



/** Function Clear Interrupts */
void RF24L01_clear_interrupts(void) {
  RF24L01_send_command(RF24L01_command_FLUSH_TX);

  RF24L01_reg_STATUS_content status;

  status = RF24L01_get_status();

  status.RX_DR = 1;
  status.TX_DS = 1;
  status.MAX_RT = 1;
  RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t*) & status, 1);

}//End Clear Interrupts

void ClearStatus(void){
	RF24L01_reg_STATUS_content status;
	*((uint8_t *) & status) = 0;
	status.RX_DR = 1;
	status.TX_DS = 1;
	status.MAX_RT = 1;
	RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t*) & status, 1);
}
