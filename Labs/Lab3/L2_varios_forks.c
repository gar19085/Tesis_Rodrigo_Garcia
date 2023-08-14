/*
 ============================================================================
 Nombre: L2_varios_forks.c
 Autor:  Luis Alberto Rivera
 ============================================================================ 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int a;

	a = fork();
	a = fork();
	a = fork();
	
	printf("Mensaje, valor = %d\n", a);
	fflush(stdout);
	
	sleep(10);
	return(0);
}
