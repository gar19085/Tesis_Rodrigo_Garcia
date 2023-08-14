/*
* Copyright (C) ViGIR Vision-Guided and Intelligent Robotics Lab
* Written by Guilherme DeSouza, Luis Alberto Rivera
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, meaning:
* keep this copyright notice,
* do not try to make money out of it,
* it's distributed WITHOUT ANY WARRANTY,
* yada yada yada... 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int counter = 0;	// variable global

void My_thread(void *ptr)
{
	printf("My_thread initial count: %d\n", counter);
	counter = 200;

	while(1)
	{
		counter++;
		printf("My_thread: %d \n", counter);
		fflush(stdout);
		usleep(1100000);
	}

	pthread_exit(0);
}

int main(void)
{
	pthread_t thread2;
	pthread_create(&thread2, NULL, (void*)&My_thread, NULL);

	counter = 100;
	printf("ParentThread initial count: %d\n", counter);

	while(1)	// el main es el 1er hilo
	{
		counter++;
		printf("ParentThread: %d \n", counter);
		fflush(stdout);
		usleep(1000000);
	}
}
