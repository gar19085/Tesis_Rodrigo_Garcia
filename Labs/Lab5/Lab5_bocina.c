/* Lab5_bocina.c
 * IE3048 - Electrónica Digital 3
 * Autor: Luis Alberto Rivera
  
 Compilar usando -lwiringPi y -lpthread
	gcc Lab5_bocina.c -o Lab4_bocina -lwiringPi -lpthread
		o
	en Eclipse, agregar wiringPi y pthread al linker (-l)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		// standard symbolic constants and types (e.g. NULL); usleep
#include <wiringPi.h>
#include <pthread.h>

// Los siguientes valores corresponden al pinout de wiringPi
// Son distintos del pinout de Broadcom GPIO
#define SPKR 22
#define BTN1 27

void teclado(void *ptr)
{
	char *input = (char *)ptr;
		
	while(*input != 's') {
		scanf(" %c", input);
	}
	
	// Pude haber llamado a la función exit() acá...
	pthread_exit(0);
}


int main(void)
{
	char opcion = 'r';	// modo "reanudar", que es lo mismo que modo "bocina activa"
	int boton = LOW;    // Por si uso el aux board, que es normalmente bajo (pull-downs)
    //int boton = HIGH; // Por si conecto usando pull ups
	pthread_t teclado_thr;
	
	wiringPiSetup(); // wiringPiSetupGpio() could be used, with appropriate numbers for the #defines
	
	// Configurar puertos
	pinMode(SPKR, OUTPUT);	// GPIO6
	pinMode(BTN1, INPUT);	// GPIO16
	
	// Configura la resistencia de Pull Down (o Pull Up) para el botón
	pullUpDnControl(BTN1, PUD_DOWN); // aux. board, que tiene pulldowns
    //pullUpDnControl(BTN1, PUD_UP); // circuito con pull-ups
    
	printf("Presione el botón para iniciar el sonido.\n\n");
	fflush(stdout);
	
	// Ciclo para leer el valor del botón. 
	while(!boton) {    // aux. boar, normalmente bajo
    //while(boton) {   // circuito normalmente alto     
		boton = digitalRead(BTN1);
		usleep(1000);	// para perder un poco de tiempo
	}
	
	printf("Opciones del teclado:\n");
	printf("\np - pausar\nr - reanudar\ns - salir del programa\n\n");
    fflush(stdout);
    
	// Crea el pthread para esperar entradas del teclado
	pthread_create(&teclado_thr, NULL, (void*)&teclado, (void*)&opcion);
    				
	while(opcion != 's') // hasta que no se ingrese la opción de salir
	{
		if(opcion == 'r')
		{
			delay(1);	// wiringPi function, input in ms
			digitalWrite(SPKR, HIGH);
			delay(1);	// wiringPi function, input in ms
			digitalWrite(SPKR, LOW);
		}
		else
			usleep(1000);
        
        //printf("%c ", opcion);
        //fflush(stdout);
	}
	
	pthread_join(teclado_thr, NULL);
	
	printf("Saliendo del programa...\n\n");
	
	return 0;
}
