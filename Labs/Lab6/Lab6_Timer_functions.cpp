#include <unistd.h>
#include <iostream>
#include <ctime>
//#include <cstdlib>
#include <cstdint>
#include <sys/timerfd.h>
#include <fcntl.h>
#include <sched.h>

#define MI_PRIORIDAD 1

    struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;

    
    sched_setscheduler(0, SCHED_FIFO, &param);
