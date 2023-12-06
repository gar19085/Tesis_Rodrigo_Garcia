/*
 ============================================================================
 Nombre: L2_Hilos_Ej1.c
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
	printf("No soy el primer hilo.\n");
	fflush(stdout);
	sleep(2);
	
	pthread_exit(0);	// Para salir correctamente del hilo.
}

// Función principal (primer hilo de ejecución)
int main(void)
{
	pthread_t thread2;	// Variable para identificar el 2do hilo que se creará.
		
	// La siguiente función crea un POSIX thread (pthread), que es un hilo del
	// estándar POSIX. Los argumentos de entrada son: la variable tipo pthread,
	// configuraciones (NULL para usar las default), la función a ejecutar por
	// el hilo, y un puntero para la información que se quiere pasar al nuevo
	// hilo (NULL si no se quiere pasar nada).
	pthread_create(&thread2, NULL, (void*)&My_Thread, NULL);

	printf("Soy el primer hilo.\n");
	fflush(stdout);
	
	// La siguiente función espera que el hilo indicado termine (bloquea).
	// El segundo argumento (puntero) se podría usar para recuperar algo que
	// el hilo devuelva (si el hilo no devuelve nada, se coloca NULL).
	pthread_join(thread2, NULL);
	
	printf("Después de que el 2do hilo haya terminado.\n");
	
	return(0);
}