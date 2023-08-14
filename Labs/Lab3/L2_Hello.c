/*
 ============================================================================
 Nombre: L2_Hello.c
 Autor:  Luis Alberto Rivera
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
  while(1)
  {
	printf("Hello ");
	fflush(stdout);		// para forzar vaciada de buffers asociados al
						// standard output (consola).
	usleep(1100000);
  }
	
  return 0;
}

