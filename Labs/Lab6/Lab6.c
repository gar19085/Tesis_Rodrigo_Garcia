/* ============================================================================
   Nombre: Lab6.c
   Autor:  Luis Alberto Rivera
   ========================================================================= */
#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define MI_PRIORIDAD 10  // Rango entre 1 y 99. A mayor valor, más alta la prioridad
						 // Parece que no deja asignar prioridades mayores a 95 (RPi)
#define MAX_LETRAS 100
#define MAX_CADENAS 60
#define MILI_A_NANO 1000000
#define PRIMERO "Lab6_primero.txt"
#define SEGUNDO "Lab6_segundo.txt"
#define RECONSTRUIDO "Lab6_reconstruido.txt"
#define PERIODO_1_y_2 20
#define PERIODO_3	  10
#define INIT_1	 1
#define INIT_2	11
#define INIT_3	 6

// Function Prototypes
int timer_config(int, int);
void wait_period(const int);
void imprimir_y_guardar();

//Define global variables
char StringArray[MAX_CADENAS][MAX_LETRAS];
int cont1 = 0, cont2 = 0;

void *Primero(void *ptr)
{
	char *Buff;
	Buff = (char *)ptr;	// recupera la información pasada a este hilo
	
	// Asignar prioridad
	struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1)
	{
        perror("sched_setscheduler falló, thread 1");
        exit(20);
    }

	FILE *fp_primero = fopen(PRIMERO, "r");	// Abrir archivo

	// Revisar que el archivo se abrió correctamente
	if(fp_primero == NULL)
	{
		printf("El archivo %s no se abrió correctamente.\n", PRIMERO);
		exit(0);
	}

	// Configurar e iniciar el timer.
	int timer_fd = timer_config(PERIODO_1_y_2, INIT_1);
	wait_period(timer_fd);	// SIN ESTO, LOS HILOS PUEDEN EMPEZAR EN CUALQUIER ORDEN!!
	
	// Bucle para leer las líneas del texto. El período de lectura está determinado
	// por el timer. Las líneas se guardan en el buffer común. El bucle termina
	// cuando se llega al final del archivo.
	while(fgets(Buff, MAX_LETRAS, fp_primero) != NULL)
	{
		cont1++;
//		printf("Hilo 1: %s", Buff);  // temporal, para debugging
//		fflush(stdout);
		wait_period(timer_fd);
	}
	fclose(fp_primero);
	
	pthread_exit(0);	// Salir del hilo
}

void *Segundo(void *ptr)
{
	char *Buff;
	Buff = (char *)ptr;	// recupera la información pasada a este hilo
	
	// Asignar prioridad
	struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1)
	{
        perror("sched_setscheduler falló, thread 2");
        exit(20);
    }

	FILE *fp_segundo = fopen(SEGUNDO, "r");	// Abrir archivo

	// Revisar que el archivo se abrió correctamente
	if(fp_segundo == NULL)
	{
		printf("El archivo %s no se abrió correctamente.\n", SEGUNDO);
		exit(0);
	}

	// Configurar e iniciar el timer.
	int timer_fd = timer_config(PERIODO_1_y_2, INIT_2);
	wait_period(timer_fd);	// SIN ESTO, LOS HILOS PUEDEN EMPEZAR EN CUALQUIER ORDEN!!

	// Bucle para leer las líneas del texto. El período de lectura está determinado
	// por el timer. Las líneas se guardan en el buffer común. El bucle termina
	// cuando se llega al final del archivo.
	while(fgets(Buff, MAX_LETRAS, fp_segundo) != NULL)
	{
		cont2++;
//		printf("Hilo 2: %s", Buff);  // temporal, para debugging
//		fflush(stdout);
		wait_period(timer_fd);
	}
	fclose(fp_segundo);
	
	pthread_exit(0);	// Salir del hilo
}

void *Tercero(void *ptr)
{
	int i = 0;
	char *Buff;
	Buff = (char *)ptr;	// recupera la información pasada a este hilo
	
	// Asignar prioridad
	struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1)
	{
        perror("sched_setscheduler falló, thread 3");
        exit(20);
    }
	
	// Configurar e iniciar el timer.
	int timer_fd = timer_config(PERIODO_3, INIT_3);
	wait_period(timer_fd);	// SIN ESTO, LOS HILOS PUEDEN EMPEZAR EN CUALQUIER ORDEN!!

	// Copiar las cadenas al string array
	for(i = 0; i < MAX_CADENAS; i++)
	{
		strcpy(StringArray[i], Buff);
//		printf("Hilo 3: %s", StringArray[i]);  // temporal, para debugging
//		fflush(stdout);
		wait_period(timer_fd);
	}

	pthread_exit(0);	// Salir del hilo
}


int main(void)
{
	// Variables
	pthread_t thrd1, thrd2, thrd3;
	char Buffer[MAX_LETRAS]; // Buffer común

// --- Lo siguiente no es necesario para el lab. ------------------------------
	// Rangos de las prioridades para SCHED_FIFO:
	printf("Rangos SCHED_FIFO: min = %d, max = %d\n\n\n", sched_get_priority_min(SCHED_FIFO),
	       sched_get_priority_max(SCHED_FIFO));
// ----------------------------------------------------------------------------

	// Creación de los hilos adicionales
	pthread_create(&thrd1, NULL, Primero, (void *)Buffer);
	pthread_create(&thrd2, NULL, Segundo, (void *)Buffer);
	pthread_create(&thrd3, NULL, Tercero, (void *)Buffer);

	// Esperar hasta que todos los hilos terminen.
	pthread_join(thrd1, NULL);
	pthread_join(thrd2, NULL);
	pthread_join(thrd3, NULL);

	imprimir_y_guardar();  // No es necesario crear una función aparte.
	
	puts("\n\nFin del programa...\n");

	return EXIT_SUCCESS;
}

// NO ERA OBLIGACIÓN CREAR UNA FUNCIÓN COMO ESTA
// timer_config
// Función que configura el timer.
// Entradas: period - período, en milisegundos (entre 1 y 999)
//		   inittime - tiempo a esperar antes de iniciar el conteo, luego de
//					  iniciar el timer, también en milisegundos (entre 1 y 999)
// Salida: el file descriptor correspondiente al timer configurado.
int timer_config(int period, int inittime)
{
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(fd == -1)
	{
        perror("Error al crear el timer.");
        exit(1);
    }

	// Asegurarse que el período está en el intervalo deseado
	if(period > 999)
		period = 999;
	if(period < 1)
		period = 1;
	
    struct itimerspec itval;
    // Tiempo en que se dispara el timer
    itval.it_interval.tv_sec = 0;	// se asumen períodos menores a 1 segundo
    itval.it_interval.tv_nsec = (long)period*MILI_A_NANO;
    // Tiempo a esperar antes de empezar los ciclos
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = (long)inittime*MILI_A_NANO;

    // Iniciar el timer
    if(timerfd_settime(fd, 0, &itval, NULL) == -1)
	{
        perror("Error al iniciar el timer.");
        exit(1);
    }
    return fd;
}

// NO ERA OBLIGACIÓN CREAR UNA FUNCIÓN COMO ESTA
// Función que espera el tiempo restante antes que venza el timer.
// Entrada: fd - file descriptor correspondiente al timer.
void wait_period(const int fd)
{
    // read tells you how many periods it's been since you last checked in
    uint64_t num_periods = 0;
    if(read(fd, &num_periods, sizeof(num_periods)) == -1)
	{
        perror("Error al leer el timer.");
        exit(1);
    }

    if(num_periods > 1)
	{
        puts("Se pasó de un período.");
        exit(1);
    }
}

// NO ERA OBLIGACIÓN CREAR UNA FUNCIÓN COMO ESTA
// Función que imprime el texto reconstruido en la terminal, y lo
// guarda en un archivo nuevo.
void imprimir_y_guardar()
{
	FILE *fp_reconstruido;
	int i;
	fp_reconstruido = fopen(RECONSTRUIDO, "w");
		
	for(i = 0; i < (cont1+cont2); i++)
	{
		fputs(StringArray[i], fp_reconstruido);	// escribe una línea en el archivo
		printf(StringArray[i]);
		fflush(stdout);
	}

	fclose(fp_reconstruido);	// cierra el archivo
}
