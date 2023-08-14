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

// Función que el proceso hijo llamará
void child(void)
{
	while(1)
	{
		printf("Hello ");
		fflush(stdout);
		usleep(1100000);
	}
}

int main(void)
{
	int a;

	// A continuación se ejecuta "a = fork()" y se determina si la función
	// ejecuta correctamente o no. Si no, (a = fork()) tiene un valor negativo.
	// Si fork es exitoso, se crea un proceso hijo, que es un clon del proceso
	// padre (el original). Ambos procesos continúan ejecutando a partir de la
	// siguiente linea de código después de fork.
	// El valor de la variable "a", que es lo que regresa la función fork, es
	// distinta para el padre y el hijo:
	// Para el hijo, la variable "a" tiene un valor de cero.
	// Para el padre, la variable "a" tiene el valor del PID del proceso hijo
	// recién creado. Esa diferencia permite controlar qué hará el hijo y qué
	// hará el padre a continuación.
	if((a = fork()) < 0)
	{
		printf("fork error\n");
		exit(-1);
	}

	if(a == 0)  // condición cierta para el proceso hijo.
		child();

	// Código que sólo el proceso padre ejecutará, ya que en la función child
	// se tiene un while(1).
	while(1)
	{
		printf("World\n");
		fflush(stdout);
		usleep(1000000);
	}

	return(0);
}

