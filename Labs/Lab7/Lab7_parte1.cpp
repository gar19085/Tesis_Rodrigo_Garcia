#include <iostream>
#include <thread>
#include <unistd.h>
#include <wiringPi.h>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <ctime>
//Especificos de linux
#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <sched.h>
#include <sys/timerfd.h>
#include <sys/mman.h>

#define LUZ_1  3
#define LUZ_2  4
#define LUZ_P  5
#define BTN1  16

