/*
*	UART communication on Raspberry Pi using C (WiringPi Library)
*	@author: DzhL
*/

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>

int serial_port ;
static uint8_t run;

void intHandler(int dummy);

int main ()
{
  	uint8_t i;
  	char ch;
  	run = 1;

  	signal(SIGINT, intHandler);

  	if ((serial_port = serialOpen ("/dev/ttyS0", 9600)) < 0)
  	{
  		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    	return 1 ;
    }

    if (wiringPiSetup () == -1)
  	{
    	fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    	return 1 ;
  	}
  	i=0;

  	while(run)
  	{
  		if(serialDataAvail(serial_port))
		{
			ch = serialGetchar(serial_port);
			printf("%c", ch);
			fflush(stdout);
			//serialPutchar(serial_port, ch);
		}
	}

}

//Catch Ctrl C
void intHandler(int dummy){
	run = 0;
	serialClose(serial_port);
	printf("\nBye\n");
}
