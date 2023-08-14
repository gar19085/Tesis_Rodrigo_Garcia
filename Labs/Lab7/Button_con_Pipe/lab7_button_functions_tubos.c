/* lab7_button_functions_tubos.c
 * IE3059 - Electrónica Digital 3
 * Autor: Luis Alberto Rivera
 
 Functions to check if button has been pushed and to clear the event.
 We'll create a static library for students to link to their programs.
 Steps to create the library:
 1) Compile into an object file:
		gcc -c lab7_button_functions_tubos.c 
 2) Create the static library "libIE3059lab7.a":
		ar -cvq libIE3059lab7.a lab7_button_functions_tubos.o
		
 To use this library, students need to include the header file "IE3059lab7.h".
 That's where the prototypes of the functions are. They need to compile their programs
 like this:
	gcc miprograma.c -o miprograma libIE3059lab7.a
		or
	gcc miprograma.c -o miprograma -L/path/to/library-directory -lIE3059lab7
	
 Include -lwiringPi if using the wiringPi utilities.
 
 If using Eclipse, that library can be added (Other objects).
 Antes de correr "miprograma", debe correrse el programa IE3059lab7.
 Los archivos IE3059lab7, libIE3059lab7.a e IE3059lab7.h deben copiarse a la
 Raspberry Pi.
 
 No se necesita sudo para correr miprograma
 */

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <poll.h>

static int config_test = 0;
int pipe_fd;
struct pollfd fds[1];

// Abrir el tubo nombrado y configurar la estructura para el poleo.
void configurar(void)
{
	if((pipe_fd = open("/tmp/Lab7", O_RDONLY)) < 0)
	{
		printf("Error. Corra el ejecutable IE3059lab7 primero.\n");
		exit(-1);
	}

    fds[0].fd = pipe_fd;
    fds[0].events = POLLIN;
}

// Si hay algo en el tubo nombrado, quiere decir que el botón se presionó,
// y eso generó una interrupción en el programa IE3059lab7.
// Retornar 1 en caso que se detecte algo en el tubo,
// retornar 0 en caso contrario.
int check_button(void)
{
	// configuration should be done only once.
	if(config_test == 0){
		configurar();
		config_test = 1;
	}
		
	if(poll(fds,1,0) == 1)     // Hay algo en el tubo
		return 1;
	else
		return 0;
}

// Si hay algo en el tubo nombrado, leerlo (para que se limpie el tubo)
void clear_button(void)
{
    int dummy;
	// configuration should be done only once.
	if(config_test == 0){
		configurar();
		config_test = 1;
	}
	
	if(poll(fds,1,0) == 1)    // Si hay algo en el tubo, leerlo
    {    
        if(read(pipe_fd, &dummy, sizeof(dummy)) < 0)
        {
            printf("Error. Corra el ejecutable IE3059lab7 primero.\n");
            exit(-1);
        }
    }
}
