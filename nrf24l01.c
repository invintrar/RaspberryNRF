#include "nrf24l01.h"


void RF24L01_init(void) {
  wiringPiSetup();

  //CE
  RF24L01_CE_setOutput();
  RF24L01_CE_setLow();

  //SPI
  if( wiringPiSPISetup(0, 5000000) == -1 )
  	printf("SPI0 No se Inicia Correctamente.\n");
  else
  	printf("SPI0 Iniciado Correctamente.\n");
}//Fin Iniciacion


void RF24L01_send_command(uint8_t command) {
  uint8_t aux[2];
  aux[0] = command;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
}//Fin Send Command


uint8_t RF24L01_read_register(uint8_t register_addr) {
  //Send address and read command
  aux[0] = RF24L01_command_R_REGISTER | register_addr;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);

  return aux[1];
}//Fin Read Register


void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length) {
  uint8_t i, aux[length+1];
  //Address and write command
  aux[0] = RF24L01_command_W_REGISTER | register_addr;
  for (i = 1; i < (length+1); i++) {
  	aux[i] = value[i-1];
  }
   //Send data
   wiringPiSPIDataRW( CHANNEL, aux, (length+1) );
}//Fin Werite Register

/* SETUP */
void RF24L01_setup(uint8_t *tx_addr, uint8_t *rx_addr, uint8_t channel) {
  RF24L01_CE_setLow(); //CE -> Low
  /*Setup of Addres Widths*/
  RF24L01_reg_SETUP_AW_content SETUP_AW;
  *((uint8_t *)&SETUP_AW) = 0;
  SETUP_AW.AW = 0x03;//RX/TX Address field width('11' - 5 bytes)
  RF24L01_write_register(RF24L01_reg_SETUP_AW, ((uint8_t *)&SETUP_AW), 1);

  /* Escribimos la direccion rx en pipe0  */
  RF24L01_write_register(RF24L01_reg_RX_ADDR_P0, rx_addr, 5);

  /* Escribimos la direccion en tx  */
  RF24L01_write_register(RF24L01_reg_TX_ADDR, tx_addr, 5);

  /* Enable (Auto Acknowledgment)  */
  RF24L01_reg_EN_AA_content EN_AA;
  *((uint8_t *)&EN_AA) = 0;
  RF24L01_write_register(RF24L01_reg_EN_AA, ((uint8_t *)&EN_AA), 1);

  /*SETUP_RETR */
  RF24L01_reg_SETUP_RETR_content SETUP_RETR;
  *((uint8_t *)&SETUP_RETR) = 0;
  SETUP_RETR.ARC = 0x03;//Auto Retransmit Count(Up to 3 Re-Transmit on fail of AA)
  SETUP_RETR.ARD = 0x01;//Auto Retransmit Delay(wait 500uS)
  RF24L01_write_register(RF24L01_reg_SETUP_RETR, ((uint8_t *)&SETUP_RETR), 1);

  /* Enable RX Address  */
  RF24L01_reg_EN_RXADDR_content RX_ADDR;
  *((uint8_t *)&RX_ADDR) = 0;
  RX_ADDR.ERX_P0 = 1;//Enable data pipe 0
  RF24L01_write_register(RF24L01_reg_EN_RXADDR, ((uint8_t *)&RX_ADDR), 1);

  /* RF Channel  */
  RF24L01_reg_RF_CH_content RF_CH;
  *((uint8_t *)&RF_CH) = 0;
  RF_CH.RF_CH = channel;//Sets the frequency channel nRF24L01 operates on
  RF24L01_write_register(RF24L01_reg_RF_CH, ((uint8_t *)&RF_CH), 1);

  /* RX payload in data pip 0  */
  RF24L01_reg_RX_PW_P0_content RX_PW_P0;
  *((uint8_t *)&RX_PW_P0) = 0;
  RX_PW_P0.RX_PW_P0 = 0x20;//Number of bytes in RX payload in data pipe 0 (32 bytes).
  RF24L01_write_register(RF24L01_reg_RX_PW_P0, ((uint8_t *)&RX_PW_P0), 1);

  /* RF Setup Register */
  RF24L01_reg_RF_SETUP_content RF_SETUP;
  *((uint8_t *)&RF_SETUP) = 0;
  RF_SETUP.RF_PWR = 0x02;//Set RF output power in TX mode('10' - 6dBm)
  RF_SETUP.RF_DR = 0x01;//Air Data Rate  ('1' - 2Mbps)
  RF_SETUP.LNA_HCURR = 0x01;//Setup LNA gain
  RF24L01_write_register(RF24L01_reg_RF_SETUP, ((uint8_t *)&RF_SETUP), 1);

  /* Configuration Register  */
  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  config.PWR_UP = 0;//1: Power up, 0: Power Down
  config.PRIM_RX = 1;//RX/TX control(1: PRX, 0:PTX)
  config.EN_CRC = 1;//Enable CRC. Forced high if one of the bits in the EN_AA is high
  config.MASK_MAX_RT = 0;//Mask interrupt caused by MAX_RT (0:Reflect MAX_RT as active low interrupt on the IRQ pin
  config.MASK_TX_DS = 0;//Mask interrupt caused by TX_DS (0:Reflect TX_DS as active low interrupt on the IRQ pin
  config.MASK_RX_DR = 0;//Mask interrupt caused by RX_DR (0:Reflect RX_DR as active low interrupt on the IRQ pin
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);
}//Fin SETUP


void RF24L01_set_mode_TX(void) {
  RF24L01_send_command(RF24L01_command_FLUSH_TX);

  //CE->LOW
  RF24L01_CE_setLow();

  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  config.PWR_UP = 1;
  config.EN_CRC = 1;
  config.MASK_MAX_RT = 0;
  config.MASK_TX_DS = 0;
  config.MASK_RX_DR = 0;
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);
}//FIN SET_MODE_TX


void RF24L01_set_mode_RX(void) {
  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  config.PWR_UP = 1;
  config.PRIM_RX = 1;
  config.EN_CRC = 1;
  config.MASK_MAX_RT = 0;
  config.MASK_TX_DS = 0;
  config.MASK_RX_DR = 0;
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  //Clear the status register to discard any data in the buffers
  RF24L01_reg_STATUS_content a;
  *((uint8_t *) &a) = 0;
  a.RX_DR = 1;
  a.MAX_RT = 1;
  a.TX_DS = 1;
  RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t *) &a, 1);
  RF24L01_send_command(RF24L01_command_FLUSH_RX);
  //CE -> High
  RF24L01_CE_setHigh();
}//FIN SET_MODE_RX


RF24L01_reg_STATUS_content RF24L01_get_status(void) {
  uint8_t status;
  uint8_t aux[2];

  //Send address and command
  aux[0] = RF24L01_command_NOP;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
  status = aux[1];

  return *((RF24L01_reg_STATUS_content *) &status);
}


void RF24L01_write_payload(uint8_t *data, uint8_t length) {
  uint8_t aux[2], i;
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  if (a.MAX_RT == 1) {
  	printf("Si llega Aki\n");
    //If MAX_RT, clears it so we can send data
    *((uint8_t *) &a) = 0;
    a.TX_DS = 1;
    RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t *) &a, 1);
  }
  //Send address and command
  aux[0] = RF24L01_command_W_TX_PAYLOAD;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);

  //Send data
  for(i =0; i < length; i++){
  	aux[0] = data[i];
  	aux[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, data, 2);
  }

  //Generates an impulsion for CE to send the data
  RF24L01_CE_setHigh();
  uint16_t delay = 0xFF;
  while(delay--);
  RF24L01_CE_setLow();
  printf("Here\n");

}


void RF24L01_read_payload(uint8_t *data, uint8_t length) {
  uint8_t i, status,aux[2];

  //Send address
  aux[0] = RF24L01_command_R_RX_PAYLOAD;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);

  //Get data
  for (i=0; i<length; i++) {
  	aux[0] = 0x00;
  	aux[1] = 0x00;
  	wiringPiSPIDataRW(CHANNEL, aux, 2);
    *(data++) = aux[1];
  }

  RF24L01_write_register(RF24L01_reg_STATUS, &status, 1);
  RF24L01_send_command(RF24L01_command_FLUSH_RX);
}

uint8_t RF24L01_was_data_sent(void) {
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();

  uint8_t res = 0;
  if (a.TX_DS) {
    res = 1;
  }
  else if (a.MAX_RT) {
    res = 2;
  }

  return res;
}

uint8_t RF24L01_is_data_available(void) {
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  return a.RX_DR;
}

void RF24L01_clear_interrupts(void) {
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t*)&a, 1);
}
