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

//Catch Ctrl C
void intHandler(int dummy){
	run = 0;
	serialClose(serial_port);
	printf("\nBye\n");
}



int main ()
{
  char dat;
  char buffer[82];
  short j;
  run = 1;
  j=0;

  signal(SIGINT, intHandler);

  if ((serial_port = serialOpen ("/dev/ttyS0", 9600)) < 0)		/* open serial port */
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiSetup () == -1)							/* initializes wiringPi setup */
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  while(run){

		 if(serialDataAvail (serial_port) )
		  {
		    dat = serialGetchar (serial_port);
		    if(dat == '$'){
		    	dat = serialGetchar(serial_port);
		    	if(dat == 'G'){
		    		dat = serialGetchar(serial_port);
		    		if(dat == 'P'){
		    			printf("$GP");
		    		}
		    	}
		    }
		    printf("%c",dat);
		    fflush (stdout) ;
		  }
	}

}
