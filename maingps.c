/*
*	@author: DzhL
*/

#include "maingps.h"

static uint8_t run;
int serialPort;
dataGps data;

//Fuction prototype
void intHandler(int dummy);
uint8_t initWiringPi(void);

int main ()
{
	uint8_t i;
  	run = 1;

  	signal(SIGINT, intHandler);

  	if(initWiringPi())
  	{
  		printf("Error Setup WiringPi\n");
  	}

  	if(initSerial())
  	{
  		printf("Error Setup Serial\n");
  	}

  	while(run)
  	{
  		data = getDataGps();
  		printf("\n%d:%d:%d\n", data.hour, data.minute, data.second);
  		printf("%d/%d/%d\n", data.month, data.day, data.year);
  		printf("%d%s%d%c%d%c %c\n", data.gradosLatitud, "°", data.minutosLatitud, 39, 
  		data.segundosLatitud, 34, data.latitud);
  		printf("%d%s%d%c%d%c %c\n\n", data.gradosLongitud, "°", data.minutosLongitud, 39, 
  		data.segundosLongitud, 34, data.longitud);
  		delay(1000);
	}
}

//Catch Ctrl C
void intHandler(int dummy){
	run = 0;
	serialFlush(serialPort);
	serialClose(serialPort);
	printf("\nBye\n");
}
uint8_t initWiringPi(void){
	if (wiringPiSetup () == -1)
  	{
    	fprintf (stdout, "Unable to start wiringPi: %s\n", strerror 
    	(errno)) ;
    	return 1 ;
  	}
  	else
  	{
  		return 0;
  	}
}

