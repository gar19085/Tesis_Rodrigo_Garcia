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

int counter = 0;	// variable global

void child(void)
{
	printf("Child initial count: %d\n", counter);
	counter = 200;

	while(1)
	{
		counter++;
		printf("Child counter = %d\n", counter);
		fflush(stdout);
		usleep(1100000);
	}
}

int main(void)
{
	int a;
	counter = 100;

	printf("Parent initial count: %d\n", counter);

	if((a = fork()) < 0)
	{
		printf("fork error\n");
		exit(-1);
	}

	if(a == 0) // child process
		child();

	while(1)
	{
		counter++;
		printf("Parent counter = %d\n", counter);
		fflush(stdout);
		usleep(1000000);
	}
}