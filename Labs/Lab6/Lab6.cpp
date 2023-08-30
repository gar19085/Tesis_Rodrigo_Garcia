#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <string>
#include <cerrno>
#include <sys/timerfd.h>
#include <unistd.h>
#include <linux/types.h>
#include <sched.h>


#include <fcntl.h>
#include <getopt.h>


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


int timer_config(int, int);
void wait_period(const int);
void imprimir_y_guardar();

char StringArray[MAX_CADENAS][MAX_LETRAS];
int cont1 = 0, cont2 = 0;


void *Primero(void *ptr){
    
}