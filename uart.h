#ifndef UART_H
#define UART_H

#include <stdlib.h>

typedef struct{
	short Grados_longitud;
	float Minutos_longitud;
	char Longitud;
	short Grados_latitud;
	float Minutso_latitud;
	char Latitud
	float Altitud;
	short Stelites;
}coordenada;

short EsGPGGA(char *trama);

#endif

/* End File */
