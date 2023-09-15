/* SPI_RPi_simple.c
 Por: Luis Alberto Rivera
 
 Programa que configura la comunicación SPI entre la Rasbperry Pi (maestro)
 y otro dispositivo (esclavo), y continuamente manda y recibe un byte.
 
 Se usan funciones de la librería wiringPi.
 	https://projects.drogon.net/raspberry-pi/wiringpi/
 Recuerde compilar usando -lwiringPi
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>		// Para los tipos enteros como uint8_t
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define SPI_CANAL	      0	// Canal SPI de la Raspberry Pi, 0 ó 1
#define SPI_VEL 	2000000	// Velocidad de la comunicación SPI (reloj, en HZ)
                            // Máxima de 3.6 MHz con VDD = 5V, 1.2 MHz con VDD = 2.7V

uint8_t SPI_manda_recibe(uint8_t);	// prototipo

int main(void)
{    
    uint8_t a_enviar = 0, recibido;	// variables de 1 byte
	
	// Configuración del SPI
	if(wiringPiSPISetup(SPI_CANAL, SPI_VEL) < 0)
	{
		printf("wiringPiSPISetup falló\n");
		return(-1) ;
	}
		
	while(1)
	{
	// Con lo siguiente se especifica el valor que se quiera mandar al esclavo.
	// Dependiendo de qué haga el esclavo, puede ser que lo que se envíe no sea
	// relevante. Puede que el esclavo sólo requiera recibir algo para causar
	// una interrupción, y entonces mandar algo de regreso al maestro.
	// Puede que el esclavo sí requiera algún valor particular, como un byte de
	// inicio o configuración.
		a_enviar++; // a_enviar = LO QUE SE QUIERA ENVIAR;
		
		recibido = SPI_manda_recibe(a_enviar);	// devuelve lo que se recibió por SPI
		printf("Enviado: %d, Recibido: %d\n", a_enviar, recibido);
		fflush(stdout);
		sleep(1);	// Esperar el tiempo que se desee. Se puede usar usleep,
					// o se puede establecer una tarea periódica.
	}
     
  return 0;   
}

// SPI_manda_recibe: función que implementa la comunicación SPI con el esclavo.
// Entrada: byte a mandar al esclavo
// Salida: byte que se recibió del esclavo.
uint8_t SPI_manda_recibe(uint8_t valor)
{
	uint8_t dato[1];	// buffer para la comunicación por SPI
	dato[0] = valor;
	
	// La siguiente función realiza la transacción simultánea de escritura/lectura
	// a través del canal seleccionado de SPI. El dato que estaba en el buffer se
	// sobreescribe con el dato que regresa por el bus SPI.
	wiringPiSPIDataRW(SPI_CANAL, dato, 1);
    
	return(dato[0]);
}
