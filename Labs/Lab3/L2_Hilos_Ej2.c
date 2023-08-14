/*
 ============================================================================
 Nombre: L2_Hilos_Ej2.c
 Autor:  Luis Alberto Rivera
 ============================================================================
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Código a ejecutar por el segundo hilo
void My_Thread(void *ptr)
{
	char *message;	// puntero, para la cadena de caracteres (string)
	message = (char *)ptr; // recupera la información pasada a este hilo

	while(1)
	{
		printf("%s", message);
		fflush(stdout);
		usleep(1100000);
	}
	
	// Lo siguiente no se ejecutará, pero es buena costumbre incluirlo
	pthread_exit(0);	// Para salir correctamente del hilo
}

// Función principal (primer hilo de ejecución)
int main(void)
{
	pthread_t thread2;	// variable para identificar el 2do hilo que se creará
	// Los siguientes son dos strings
	char *message1 = "Hello ";	
	char *message2 = "World\n";

	// La siguiente función crea un POSIX thread (pthread), que es un hilo del
	// estándar POSIX. Los argumentos de entrada son: la variable tipo pthread,
	// configuraciones (NULL para usar las default), la función a ejecutar por
	// el hilo, y un puntero para la información que se quiere pasar al nuevo
	// hilo (NULL si no se quiere pasar nada).
	pthread_create(&thread2, NULL, (void*)&My_Thread, (void*)message1);

	while(1)
	{
		printf("%s", message2);
		fflush(stdout);
		usleep(1000000);
	}

	return(0);
}