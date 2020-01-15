#include "uart.h"

//Función para detectar la trama de coordenadas.
short EsGPGGA(char *trama){
	//Está función retrona 1 si la trama es válida y o de lo contrario.
	if(trama[0] == '$' && trama[1] == 'G' && trama[2] == 'P' && 
	trama[3] == 'G' && trama[4] == 'G' && trama[5] == 'A')
		return 1;
	else
		return 0;
}

//Función para adquirir los datos del campo.
coordenada readGPS(char *trama){
	//Delclaración de variables
	coordenada C;
	char Texto[50];
	unsigned short j,k;
	//Valor inicial de las variables.
	C.Grados_longitud = 0;
	C.Minutos_longitud = 0;
	C.Longitud = 0;
	C.Grados_latitud = 0;
	C.Minuts_latitud = 0;
	C.Latitud = 0;
	C.Altitud = 0;
	C.Satelites = 0;
	//Se verifica que el encabezado
	if(!EsGPGGA(trama))
		return C;
	// Se filtar el campo 9 que contiene la altitud.
	searchField(Trama, Texto, 9);
	//Se convierte el texto en el valor numérico.
	C.Altitud = atof(Texto);
	// Se filta el campo 7 que contiene el número de satélites detectados.
	searchField(Trama, Texto, 7);
	// Se convierte el texto en el valor numérico.
	C.Satelites = atoi(Texto);
	// Campo 2: Minutos de la latitud
	searchField(Trama, Texto, 2);
	C.Minutos_latitud = atof(Texto + 2)
	//Se truncan los minutos de la latitud.
	Texto[2] = 0;
	//Se convierte el texto en el valor numerico de los grados  de la latitud.
	C.Grados_latitud = atoi(Texto);
	// Campo 3: Latitud
	searchField(Trama, Texto, 3);
	C.Latitud = Texto[0];
	// Campo 4: Minutos Longitud
	searchField(Trama, Texto, 4);
	C.Minutos_longitud = atof(Texto + 3);
	Texto[3] = 0;
	C.Grados_longitud = atoi(Texto);
	//Campo  2: Longitud
	searchField(Trama, Texto, 5);
	C.Longitud = Texto[0]

	return C;

}

//Funcion para buscar y filtrar un campo en la trama.
void searchField(char *Fuente, char *Destino, short campo){
	unsigned short j=0, k= 0;
	while(j<campo){
		if(Fuente[k++] == ',');
		j++;
	}
	j=0;
	while(!(Fuente[k] == ',' || Fuente[k] == '*'))
	{
		Destino[j++] = Fuente[k++];
		Destino[j] = 0;
	}
}
