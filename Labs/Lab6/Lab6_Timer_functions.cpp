//Ayudame a comentar este codigo
/*
 ============================================================================
 Nombre: Lab6_Timer_functions.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <unistd.h> //Librería que proporciona funciones y constantes específicas de sistemas Unix.
#include <iostream> //Librería estándar de entrada/salida
#include <cstdlib> //Librería estándar para funciones generales (Se incluye para utilizar la función rand)
#include <cstdint> //Librería estándar para tipos de datos enteros
#include <sys/timerfd.h> //Librería para utilizar el timer
#include <sched.h> //Librería para utilizar la planificación

#define MI_PRIORIDAD 1 //Se define la prioridad del proceso
#define Periodo_segundos 1 //Se define el periodo del timer
#define Periodo_nanosegundos 0 //Se define el periodo del timer
#define TiempoIni_segundos 0 //Se define el tiempo inicial del timer
#define TiempoIni_nanosegundos 0 //Se define el tiempo inicial del timer

int main(){
    struct sched_param param; //Se define una estructura para la planificación
    // Asignar prioridad y la política de escalonamiento
    param.sched_priority = MI_PRIORIDAD;
    sched_setscheduler(0, SCHED_FIFO, &param); //Se asigna la prioridad y la política de escalonamiento

    // --- Configurar Timer (se podría crear una función que regrese el file descriptor) ---
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); 

    struct itimerspec itval;
    // El Timer se "dispara" cada ## segundos + ## nanosegundos
    itval.it_interval.tv_sec = Periodo_segundos;      // Revisar el tipo de la variable (¿int, long?)
    itval.it_interval.tv_nsec = Periodo_nanosegundos; // Revisar el tipo de la variable (¿int, long?)
    
    // El Timer empezará en ## segundos + ## nanosegundos desde el momento en que se inicie el Timer
    itval.it_value.tv_sec = TiempoIni_segundos;      // Revisar el tipo de la variable
    itval.it_value.tv_nsec = TiempoIni_nanosegundos; // Revisar el tipo de la variable

    // Arrancar el Timer
    timerfd_settime(timer_fd, 0, &itval, nullptr);

    uint64_t num_periods; //Se define una variable para almacenar el número de periodos

    num_periods = 0;
    read(timer_fd, &num_periods, sizeof(num_periods));   // esta función retorna algo
                      // Siempre es bueno chequear errores. Revisar la función read()
    if(num_periods > 1)
    {
        std::cout << "MISSED WINDOW" << std::endl;  // Manda la cadena al estándar output
        exit(1);
    }
    return 0;
}
    
    

