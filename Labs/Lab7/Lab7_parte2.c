/*
 ==============================================================================
 Name        : Lab7_parte2.c
 Author      : Brenyn Jungmann
 Modificado  : Luis Alberto Rivera
 Description : Create a program to use priority to schedule a stop light using
               all the LEDs and checking for a button press.

 NOTA: Al parecer, usar timers da problema, seguramente por las interrupciones.
 ==============================================================================
 */
#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <wiringPi.h>
#include <semaphore.h>
#include "IE3059lab7.h"

#define LUZ_1  3
#define LUZ_2  4
#define LUZ_P  5
#define BTN1  16

#define LUZ_1_PRI 10  // Luz 1
#define LUZ_2_PRI 10  // Luz 2
#define LUZ_P_PRI 10  // Luz peatonal

#define PERIOD	 750  // miliseconds

// Global Variables
sem_t sem;
int pr1 = LUZ_1_PRI, pr2 = LUZ_2_PRI, prP = LUZ_P_PRI;

// Function Prototypes
void check_create(int, int);
void check_join(int, int);

// Hilo para la 1a luz (1a dirección de tránsito)
void *luz1(void *ptr)
{
	// Asignar prioridad
	struct sched_param param;
    param.sched_priority = pr1;
    if(sched_setscheduler(0, SCHED_RR, &param) == -1) // Probar con SCHED_FIFO, SCHED_RR
	{
        perror("sched_setscheduler falló, luz 1");
        exit(20);
    }

	while(1)
	{
		sem_wait(&sem);
		digitalWrite(LUZ_1, HIGH);	// Encender luz
		usleep(1000*PERIOD);
		digitalWrite(LUZ_1, LOW);		// Apagar luz
		sem_post(&sem);
		usleep(1000);
	}

	pthread_exit(0);	// Salir del hilo
}

// Hilo para la 2a luz (2a dirección de tránsito)
void *luz2(void *ptr)
{
	// Asignar prioridad
	struct sched_param param;
    param.sched_priority = pr2;
    if(sched_setscheduler(0, SCHED_RR, &param) == -1) // Probar con SCHED_FIFO
	{
        perror("sched_setscheduler falló, luz 2");
        exit(20);
    }

	while(1)
	{
		sem_wait(&sem);
		digitalWrite(LUZ_2, HIGH);	// Encender luz
		usleep(1000*PERIOD);
		digitalWrite(LUZ_2, LOW);		// Apagar luz
		sem_post(&sem);
		usleep(1000);
	}

	pthread_exit(0);	// Salir del hilo
}

// Hilo para la luz peatonal
void *peatonal(void *ptr)
{
	// Asignar prioridad
	struct sched_param param;
    param.sched_priority = prP;
    if(sched_setscheduler(0, SCHED_RR, &param) == -1) // Probar con SCHED_FIFO
	{
        perror("sched_setscheduler falló, luz peatonal");
        exit(20);
    }

	while(1)
	{
		sem_wait(&sem);
		if(check_button())
		{
			digitalWrite(LUZ_P, HIGH);	// Encender luz
			usleep(1000*PERIOD);
			digitalWrite(LUZ_P, LOW);	// Apagar luz
			clear_button();
		}
		sem_post(&sem);
		usleep(1000);
	}

	pthread_exit(0);	// Salir del hilo
}


int main(int argc, char *argv[])
{
	//Declare variables
	pthread_t hilo_luz1, hilo_luz2, hilo_luzP;
	    
	if(argc == 4)	// si se ingresaron las prioridades de las tres luces
	{
		pr1 = atoi(argv[1]);
		pr2 = atoi(argv[2]);
		prP = atoi(argv[3]);
	}
	else if(argc != 1)
	{
		printf("Debe ingresar las tres prioridades (de 1 a 95), o ninguna.\nUsos:\n"
			   "  %s\n  %s pr1 pr2 prP\n", argv[0], argv[0]);
		exit(1);
	}
	
	printf("Prioridad Luz 1: %d\nPrioridad Luz 2: %d\nPrioridad Luz P: %d\n\n",
			pr1, pr2, prP);
	fflush(stdout);
			
	// Create Semaphore to make sure that two tasks aren't trying to write 
	// to the same register at the same time
	 sem_init(&sem, 0, 1);
	
	// Map to registers using WiringPi Library
	wiringPiSetupGpio() ;
	
	// Set GPIO pins for LEDs and Buttons (Input/Output)
	pinMode(LUZ_1, OUTPUT);	
	pinMode(LUZ_2, OUTPUT);	
	pinMode(LUZ_P, OUTPUT);	
	pinMode(BTN1, INPUT);
	
	// Set buttons for reading
	pullUpDnControl(BTN1, PUD_DOWN);
	
	// Clear all the LEDs
	digitalWrite(LUZ_1, LOW);
	digitalWrite(LUZ_2, LOW);
	digitalWrite(LUZ_P, LOW);
	
	//Create pthreads and check to make sure they created correctly
	check_create(pthread_create(&hilo_luz1, NULL, luz1, NULL), 1);
	check_create(pthread_create(&hilo_luz2, NULL, luz2, NULL), 1);
	check_create(pthread_create(&hilo_luzP, NULL, peatonal, NULL), 1);
	
	//Join pthreads and check to make sure they joined correctly
	check_join(pthread_join(hilo_luz1, NULL), 1);
	check_join(pthread_join(hilo_luz2, NULL), 1);
	check_join(pthread_join(hilo_luzP, NULL), 1);

	//Exit main
	return EXIT_SUCCESS;
}

//Function to check pthread_create
void check_create(int thd, int thd_num)
{
	if(thd != 0)
	{
		printf("Creating a thread %d failed.\n", thd_num);
		if(thd == EAGAIN)
		{
			printf("Insufficient resources to create another thread.\n");
		} else if(thd == EINVAL)
		{
			printf("Invalid settings in attr.\n");
		} else if(thd == EPERM)
		{
			printf("No permission to set the scheduling policy and parameters specified in attr.\n");
		}
		exit(0);
	}
}

//Function to check pthread_join
void check_join(int thd,  int thd_num)
{
	if(thd != 0)
	{
		printf("Joining thread %d failed.\n", thd_num);
		exit(0);
	}
}
