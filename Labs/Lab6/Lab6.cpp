/*
 ============================================================================
 Nombre: Lab6.cpp
 Autor:  Rodrigo Jose Garcia Ambrosy
 ============================================================================
 */

#include <iostream>
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


//#include <fcntl.h>
//#include <getopt.h>


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

//Declarar funciones
int timer_config(int, int);
void wait_period(const int);
void imprimir_y_guardar();

//Defino variables globales
char StringArray[MAX_CADENAS][MAX_LETRAS];
int cont1 = 0, cont2 = 0;


void Primero(char *Buff) {
    Buff = static_cast<char *>(Buff);
    struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler falló, thread 1");
        exit(20);
    }

    FILE *fp_primero = fopen(PRIMERO, "r");

    if (fp_primero == nullptr) {
        std::cout << "El archivo " << PRIMERO << " no se abrió correctamente." << std::endl;
        exit(0);
    }

    int timer_fd = timer_config(PERIODO_1_y_2, INIT_1);
    wait_period(timer_fd);

    while (fgets(Buff, MAX_LETRAS, fp_primero) != nullptr) {
        cont1++;
        wait_period(timer_fd);
    }
    fclose(fp_primero);
}

void Segundo(char *Buff) {
    Buff = static_cast<char *>(Buff);
    struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler falló, thread 2");
        exit(20);
    }

    FILE *fp_segundo = fopen(SEGUNDO, "r");

    if (fp_segundo == nullptr) {
        std::cout << "El archivo " << SEGUNDO << " no se abrió correctamente." << std::endl;
        exit(0);
    }

    int timer_fd = timer_config(PERIODO_1_y_2, INIT_2);
    wait_period(timer_fd);

    while (fgets(Buff, MAX_LETRAS, fp_segundo) != nullptr) {
        cont2++;
        wait_period(timer_fd);
    }
    fclose(fp_segundo);
}

void Tercero(char *Buff) {
    int i = 0;
    Buff = static_cast<char *>(Buff);

    struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler falló, thread 3");
        exit(20);
    }

    int timer_fd = timer_config(PERIODO_3, INIT_3);
    wait_period(timer_fd);

    for (i = 0; i < MAX_CADENAS; i++) {
        std::string(StringArray[i], Buff);
        wait_period(timer_fd);
    }
}

int main(void) {
    std::thread thrd1, thrd2, thrd3;
    char Buffer[MAX_LETRAS];

    thrd1 = std::thread(Primero, Buffer);
    thrd2 = std::thread(Segundo, Buffer);
    thrd3 = std::thread(Tercero, Buffer);

    thrd1.join();
    thrd2.join();
    thrd3.join();

    imprimir_y_guardar();

    std::cout << "\n\nFin del programa...\n";

    return EXIT_SUCCESS;
}

int timer_config(int period, int inittime) {
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd == -1) {
        perror("Error al crear el timer.");
        exit(1);
    }

    if (period > 999)
        period = 999;
    if (period < 1)
        period = 1;

    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = static_cast<long>(period) * MILI_A_NANO;
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = static_cast<long>(inittime) * MILI_A_NANO;

    if (timerfd_settime(fd, 0, &itval, NULL) == -1) {
        perror("Error al iniciar el timer.");
        exit(1);
    }
    return fd;
}

void wait_period(const int fd) {
    uint64_t num_periods = 0;
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

    for (i = 0; i < (cont1 + cont2); i++) {
        fputs(StringArray[i], fp_reconstruido);
        std::cout << StringArray[i];
        fflush(stdout);
    }

    fclose(fp_reconstruido);
}
