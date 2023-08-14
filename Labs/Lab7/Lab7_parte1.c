/*
 ============================================================================
 Name        : Lab7_parte1.c
 Author      : Brenyn Jungmann
 Description : Create a program to poll through all the LEDs and check for a
               button press.
 Modificado por: Luis Alberto Rivera
	Este programa no usa timers.
 ============================================================================
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
#include "IE3059lab7.h"

#define LUZ_1  3
#define LUZ_2  4
#define LUZ_P  5
#define BTN1  16

// Function Prototypes
void check_create(int, int);
void check_join(int, int);

// Real time thread to blink the lights on a polled scheduling algorithm
void *polled_scheduling(void *ptr)
{
	//Loop to get all of the string lines from file and wait for second thread
	while(1)
	{
		// Turn LED 1 on, wait 1 second and turn off
		digitalWrite(LUZ_1, HIGH);
		sleep(1);
		digitalWrite(LUZ_1, LOW);

		// Turn LED 2 on, wait 1 second and turn off
		digitalWrite(LUZ_2, HIGH);
		sleep(1);
		digitalWrite(LUZ_2, LOW);

		// Turn pedestrian LED on if button is pressed then turn off if button isn't pressed go back to LED 1
		if(check_button())
		{
			digitalWrite(LUZ_P, HIGH);
			sleep(1);
			digitalWrite(LUZ_P, LOW);
			clear_button();
		}
	}

	//Exit pthread
	pthread_exit(0);
}

int main(void)
{
	//Declare variables
	pthread_t polled_thrd;

	// Map to registers using WiringPi Library
	wiringPiSetupGpio();

	// Set GPIO pins for LEDs and Buttons (Input/Output)
	pinMode(LUZ_1, OUTPUT);	// GPIO3
	pinMode(LUZ_2, OUTPUT);	// GPIO4
	pinMode(LUZ_P, OUTPUT);	// GPIO5
	pinMode(BTN1, INPUT);	// GPIO16

	// Set buttons for reading
	pullUpDnControl(BTN1, PUD_DOWN);

	// Clear LEDs on Pins 2-5
	digitalWrite(LUZ_1, LOW);
	digitalWrite(LUZ_2, LOW);
	digitalWrite(LUZ_P, LOW);

	//Create pthreads and check to make sure they created correctly
	check_create(pthread_create(&polled_thrd, NULL, polled_scheduling, NULL), 1);

	//Join pthreads and check to make sure they joined correctly
	check_join(pthread_join(polled_thrd,NULL), 1);

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
