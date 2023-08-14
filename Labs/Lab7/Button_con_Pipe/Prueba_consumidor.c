/*
* Copyright (C) ViGIR Vision-Guided and Intelligent Robotics Lab
* Written by Guilherme DeSouza, Luis Rivera
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, meaning:
* keep this copyright notice,
* do not try to make money out of it,
* it's distributed WITHOUT ANY WARRANTY,
* yada yada yada...
*/

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <poll.h>

int main(void)
{
	int counter = 0, dummy;
	int pipe_AtoB;		// for file descriptors
	struct pollfd fds[1];
	
	if((pipe_AtoB = open("/tmp/AtoB", O_RDONLY)) < 0)
	{
		printf("pipe AtoB error\n");
		exit(-1);
	}

    fds[0].fd = pipe_AtoB;
    fds[0].events = POLLIN;
	
	while(1)
	{
		scanf("%d", &dummy);
        
		// Si hay algo en la pipe, leer
        if(poll(fds,1,0) == 1)
        {
            printf("Hay algo para leer.\n");
            if(read(pipe_AtoB, &counter, sizeof(counter)) < 0)
            {
                printf("AtoB pipe read error\n");
                exit(-1);
            }
        }
        else
            printf("Nada en la pipe...\n");
	}

	return 0;
}
