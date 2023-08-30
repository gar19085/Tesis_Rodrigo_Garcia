#include <unistd.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <sys/timerfd.h>
#include <fcntl.h>
#include <sched.h>

#define MI_PRIORIDAD 1

    struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;

    
    sched_setscheduler(0, SCHED_FIFO, &param);

    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);

    struct{
        int it_interval.tv_sec = Periodo_segundos;
        int it_interval.tv_nsec = Periodo_nanosegundos;   

        unsigned long it_value.tv_sec = TiempoIni_segundos;

    }itval;

    timerfd_settime(timer_fd, 0, &itval, NULL);

    uint64_t num_periods;

    num_periods 0;
    


    
    