#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>
#include <string.h>
#include <wiringSerial.h>

int main()
{
    char gps[65];     //gps string array
    int fd, flag = 0; //handler
    char arr[] = "$GPGGA";
    printf("raspberry gps programme");
    if (wiringPiSetup() < 0) //setup wiringpi
        return -1;
    else
        printf("setup is ok\n");
    if ((fd = serialOpen("/dev/ttyAMA0", 9600)) < 0)
    {
        fprintf(stderr, "unable to open serial device%s\n", strerror(errno));
    }
    else
    {
        printf("serial UART is Ok\n");
    }
    while (1)
    {
        int i = 0;
        int c;
        if (c = serialGetchar(fd) == 13 || 10)
        {
            for (i = 0; i < 6; i++)
            {
                if (serialGetchar(fd) == arr)
                    flag++;
            }
        }
        if (flag == 6)
        {
            flag = 0;
            for (i = 0; i <= 65; i++)
                gps = serialGetchar(fd);
        }

        printf("%d", gps);
    }
    fflush(stdout);
}
