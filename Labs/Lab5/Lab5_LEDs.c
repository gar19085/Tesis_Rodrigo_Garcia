/* Lab5_LEDs.c
 * IE3048 - Electrónica Digital 3
 * Autor: Luis Alberto Rivera
  
 Compilar usando -lwiringPi
	gcc Lab5_LEDs.c -o Lab4_LEDs -lwiringPi
		o
	en Eclipse, agregar wiringPi al linker (-l)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		// standard symbolic constants and types (e.g. NULL); usleep
#include <time.h>
#include <wiringPi.h>

// Los siguientes valores corresponden al pinout de wiringPi
// Son distintos del pinout de Broadcom GPIO
#define LED1  8
#define LED2  9
#define MAX_RANDOM 1000000

int main(void)
{
	srand(time(0));	// para el generador de números aleatorios
	
	wiringPiSetup(); // se puede usar wiringPiSetupGpio(), con números adecuados en los #define

	// Configurar puertos
	pinMode(LED1, OUTPUT);	// GPIO2
	pinMode(LED2, OUTPUT);	// GPIO3
	
	// Apagar los LEDs, en caso estuviesen encendidos (no es necesario en este caso...)
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
				
	while(1)
	{
		usleep(500000);
		usleep(rand()%MAX_RANDOM);
		digitalWrite(LED1, HIGH);
		digitalWrite(LED2, LOW);

		usleep(500000);
		usleep(rand()%MAX_RANDOM);
		digitalWrite(LED1, LOW);
		digitalWrite(LED2, HIGH);
	}

	return 0;
}
