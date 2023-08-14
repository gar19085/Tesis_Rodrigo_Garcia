/*
 ============================================================================
 Nombre: L2_World.c
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
	printf("World\n");
	fflush(stdout);		// para forzar vaciada de buffers asociados al
						// standard output (consola).
	usleep(1000000);
  }
	
  return 0;
}

