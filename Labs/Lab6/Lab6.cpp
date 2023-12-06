/*
 ============================================================================
 Nombre: Lab6.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream> //Librería estándar de entrada/salida
#include <cstdio>   //Librería estándar de entrada/salida
#include <cstdlib>  //Librería estándar para funciones generales
#include <thread>   //Librería para utilizar hilos
#include <chrono>   //Librería para utilizar la función sleep_for
#include <string>   //Librería para la manipulación de cadenas
#include <cstring>  //Librería para usar strcpy
#include <cerrno>   //Librería para utilizar la variable errno
#include <sys/timerfd.h> //Librería para utilizar el timer
#include <unistd.h> //Librería que proporciona funciones y constantes específicas de sistemas Unix.
#include <linux/types.h> // Librería que contiene definiciones de tipos de datos comunes en sistemas Linux
#include <sched.h> //Librería para utilizar la planificación
#include <fstream> //Librería para utilizar archivos



#define MI_PRIORIDAD 10  // Rango entre 1 y 99. A mayor valor, más alta la prioridad
						 // Parece que no deja asignar prioridades mayores a 95 (RPi)
#define MAX_LETRAS 100   // Máximo de letras
#define MAX_CADENAS 60   // Máximo de cadenas
#define MILI_A_NANO 1000000 // Valor para convertir de milisegundos a nanosegundos
#define PRIMERO "Lab6_primero.txt" //Primer archivo
#define SEGUNDO "Lab6_segundo.txt" //Segundo archivo
#define RECONSTRUIDO "Lab6_reconstruido.txt" //Archivo reconstruido
#define PERIODO_1_y_2 20 //Periodo de los hilos 1 y 2
#define PERIODO_3	  10 //Periodo del hilo 3
#define INIT_1	 1 //Tiempo inicial del hilo 1
#define INIT_2	11 //Tiempo inicial del hilo 2
#define INIT_3	 6 //Tiempo inicial del hilo 3

//Declarar funciones
int timer_config(int, int); //Configurar el timer
void wait_period(const int); //Esperar el periodo
void imprimir_y_guardar(); //Imprimir y guardar el archivo reconstruido

//Defino variables globales
char StringArray[MAX_CADENAS][MAX_LETRAS]; //Arreglo de cadenas
int cont1 = 0, cont2 = 0; //Contadores de cadenas


void Primero(char *Buff) { //Función del hilo 1
    Buff = static_cast<char *>(Buff); //Casteo de la variable Buff
    struct sched_param param; //Se define una estructura para la planificación
    param.sched_priority = MI_PRIORIDAD; //Se asigna la prioridad y la política de escalonamiento
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) { //Se asigna la prioridad y la política de escalonamiento
        perror("sched_setscheduler falló, thread 1");
        exit(20);
    }

    FILE *fp_primero = fopen(PRIMERO, "r"); //Se abre el primer archivo

    if (fp_primero == nullptr) { //Condicional para verificar si se abrio el archivo
        std::cout << "El archivo " << PRIMERO << " no se abrió correctamente." << std::endl;
        exit(0);
    }

    int timer_fd = timer_config(PERIODO_1_y_2, INIT_1); //Se configura el timer
    wait_period(timer_fd); //Se espera el periodo

    while (fgets(Buff, MAX_LETRAS, fp_primero) != nullptr) { //Se lee una linea del archivo
        cont1++; //Se incrementa el contador
        wait_period(timer_fd); //Se espera el periodo
    }
    fclose(fp_primero); //Se cierra el archivo
}

void Segundo(char *Buff) { //Función del hilo 2
    Buff = static_cast<char *>(Buff); //Casteo de la variable Buff
    struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler falló, thread 2");
        exit(20);
    }

    FILE *fp_segundo = fopen(SEGUNDO, "r"); //Se abre el segundo archivo

    if (fp_segundo == nullptr) {
        std::cout << "El archivo " << SEGUNDO << " no se abrió correctamente." << std::endl;
        exit(0);
    }

    int timer_fd = timer_config(PERIODO_1_y_2, INIT_2); //Se configura el timer
    wait_period(timer_fd);

    while (fgets(Buff, MAX_LETRAS, fp_segundo) != nullptr) { //Se lee una linea del archivo
        cont2++; //Se incrementa el contador
        wait_period(timer_fd); //Se espera el periodo
    }
    fclose(fp_segundo); //Se cierra el archivo
}

void Tercero(char *Buff) { //Función del hilo 3
    int i = 0; //Se define una variable para el ciclo for
    Buff = static_cast<char *>(Buff); //Casteo de la variable Buff

    struct sched_param param; //Se define una estructura para la planificación
    param.sched_priority = MI_PRIORIDAD;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) { //Se asigna la prioridad y la política de escalonamiento
        perror("sched_setscheduler falló, thread 3");
        exit(20);
    }

    int timer_fd = timer_config(PERIODO_3, INIT_3); //Se configura el timer
    wait_period(timer_fd);

    for (i = 0; i < MAX_CADENAS; i++) { //Se recorre el arreglo de cadenas

        std::strcpy(StringArray[i], Buff); //Se copia la cadena en el arreglo
        wait_period(timer_fd); //Se espera el periodo
    }
}

int main(void) { //Función principal
    std::thread thrd1, thrd2, thrd3; //Se definen los hilos
    char Buffer[MAX_LETRAS]; //Se define el buffer

    thrd1 = std::thread(Primero, Buffer); //Se crean los hilos
    thrd2 = std::thread(Segundo, Buffer); 
    thrd3 = std::thread(Tercero, Buffer); 

    thrd1.join(); 
    thrd2.join(); 
    thrd3.join();

    imprimir_y_guardar(); //Se imprime y guarda el archivo reconstruido

    std::cout << "\n\nFin del programa...\n";

    return EXIT_SUCCESS;
}

int timer_config(int period, int inittime) {
    // Crea un timer y verifica si se creó correctamente
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd == -1) {
        perror("Error al crear el timer.");
        exit(1);
    }

    // Asegura que el periodo esté en un rango válido
    if (period > 999)
        period = 999;
    if (period < 1)
        period = 1;

    struct itimerspec itval;
    // Configura el intervalo y el tiempo inicial del timer
    itval.it_interval.tv_sec = 0; 
    itval.it_interval.tv_nsec = static_cast<long>(period) * MILI_A_NANO; 
    itval.it_value.tv_sec = 0; 
    itval.it_value.tv_nsec = static_cast<long>(inittime) * MILI_A_NANO;

    // Inicia el timer
    if (timerfd_settime(fd, 0, &itval, NULL) == -1) { 
        perror("Error al iniciar el timer.");
        exit(1);
    }
    return fd; // Retorna el file descriptor del timer
}

void wait_period(const int fd) {
    uint64_t num_periods = 0;
    // Espera a que el timer expire y obtiene el número de períodos
    if (read(fd, &num_periods, sizeof(num_periods)) == -1) { 
        perror("Error al leer el timer."); 
        exit(1);
    }

    if (num_periods > 1) { 
        std::cout << "Se pasó de un período." << std::endl;
        exit(1);
    }
}

void imprimir_y_guardar() {
    FILE *fp_reconstruido;
    int i;
    fp_reconstruido = fopen(RECONSTRUIDO, "w");

    // Escribe en el archivo reconstruido y muestra en la consola
    for (i = 0; i < (cont1 + cont2); i++) { 
        fputs(StringArray[i], fp_reconstruido);
        std::cout << StringArray[i];
        fflush(stdout);
    }

    fclose(fp_reconstruido); // Cierra el archivo reconstruido
}
