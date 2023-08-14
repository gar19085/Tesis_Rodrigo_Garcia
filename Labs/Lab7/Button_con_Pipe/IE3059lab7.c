/*
 * IE3059lab7.c 
 * IE3059 - Electrónica Digital 3
 * Autor: Luis Alberto Rivera
 * 
 * Programa que reemplaza al módulo de Kernel.
 * Configura la interrupción del botón, y manda un dato a un tubo nombrado, para
 * que las funciones check_button y clear_button (en la librería aparte) puedan
 * revisar si hubo un evento de botón.

 Compilar así:
	gcc IE3059lab7.c -o IE3059lab7 -lwiringPi
 * 
 */

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>

int pipe_fd;		// for file descriptors
struct pollfd fds[1];

// Handler de la interrupción
void evento_boton(void)
{
    int dummy;
	// Sólo quiero escribir en el tubo si éste está vacío. Así que poleo el fd
	// asociado, para ver si está vacío. De ser así, escribo algo (no importa qué).
	// La función check_button también polea el tubo para ver si hay algo.
	// Si hay, lo lee, para vaciar el tubo.
    if(poll(fds, 1, 0) == 0)
    {
        if(write(pipe_fd, &dummy, sizeof(dummy)) != sizeof(dummy))
        {
            printf("Error al escribir al tubo nombrado.\n");
            exit(-1);
        }
    }
}


int main(int argc, char *argv[])
{
    int dummy;
	int boton;
    
	if(argc == 2)	// si se ingresó un valor
		boton = atoi(argv[1]);
	else
	{
		printf("Debe ingresar el puerto para el botón (pinout GPIO)\nUso: %s #puerto\n", argv[0]);
		exit(1);
	}		
	
    wiringPiSetupGpio();
        
    // chequear si el tubo existe. Si no, crearlo
	if((pipe_fd = open("/tmp/Lab7", O_RDWR)) < 0)
	{
        dummy = system("mkfifo /tmp/Lab7");
        
        if((pipe_fd = open("/tmp/Lab7", O_RDWR)) < 0)
        {
            printf("Error al abrir el tubo nombrado.\n");
            exit(-1);
        }
	}

	// Configurar la estructura para chequear el tubo
	fds[0].fd = pipe_fd;
    fds[0].events = POLLIN;
    
    // Configurar interrupción y asignar handler
    if(wiringPiISR(boton, INT_EDGE_RISING, &evento_boton) < 0)
    {
        fprintf (stderr, "Error al configurar el ISR del botón: %s\n", strerror(errno));
        return 1;
    }
    
    while(1)
	{
        usleep(100000); // perder tiempo...
    }

    return 0;
}

