/*
 ============================================================================
 Nombre: Lab6_Timer_functions.cpp
 Autor:  Rodrigo Jose Garcia Ambrosy
 ============================================================================
 */

#include <unistd.h>
#include <iostream>
//#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <sys/timerfd.h>
//#include <fcntl.h>
#include <sched.h>

#define MI_PRIORIDAD 1
#define Periodo_segundos 1
#define Periodo_nanosegundos 0
#define TiempoIni_segundos 0
#define TiempoIni_nanosegundos 0

int main(){
    struct sched_param param; //Se define una estructura para la planificacion
    // Asignar prioridad y la política de escalonamiento
    param.sched_priority = MI_PRIORIDAD;
    sched_setscheduler(0, SCHED_FIFO, &param);

    // --- Configurar Timer (se podría crear una función que regrese el file descriptor) ---
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);

    struct itimerspec itval;
    // El Timer se "dispara" cada ## segundos + ## nanosegundos
    itval.it_interval.tv_sec = Periodo_segundos;	  // Revisar el tipo de la variable (¿int, long?)
    itval.it_interval.tv_nsec = Periodo_nanosegundos; // Revisar el tipo de la variable (¿int, long?)
	
    // El Timer empezará en ## segundos + ## nanosegundos desde el momento en que se inicie el Timer
    itval.it_value.tv_sec = TiempoIni_segundos;	     // Revisar el tipo de la variable
    itval.it_value.tv_nsec = TiempoIni_nanosegundos; // Revisar el tipo de la variable

    // Arrancar el Timer
    timerfd_settime(timer_fd, 0, &itval, nullptr);

    uint64_t num_periods;

    num_periods = 0;
    read(timer_fd, &num_periods, sizeof(num_periods));	 // esta función retorna algo
		              // Siempre es bueno chequear errores. Revisar la función read()
    if(num_periods > 1)
    {
        std::cout << "MISSED WINDOW" << std::endl;	// Manda la cadena al stdandard output
        exit(1);
    }
    return 0;
}
    
    