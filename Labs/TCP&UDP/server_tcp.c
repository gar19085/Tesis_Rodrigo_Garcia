/* 	Nombre     : 	server_tcp.c
	Autor      : 	Luis A. Rivera, basado en el tutorial de
					http://www.linuxhowtos.org/C_C++/socket.htm
	Descripción: 	Servidor simple (TCP)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#define MSG_SIZE 40		// Tamaño (máximo) del mensaje. Puede ser mayor a 40.

void dostuff(int);		// Prototipo

// Función para desplegar mensajes de error
void error(const char *msg)
{
    perror(msg);	// escribe al standard error (la terminal, por defecto)
    exit(1);
}

// Al llamar al programa desde la terminal, se debe indicar el puerto a usar:
// 		./server_tcp num_puerto
// El número de puerto debe ser un entero > 1024
int main(int argc, char *argv[])
{
	int sockfd, newsockfd;	// para los file descriptors de los sockets
	int portno, pid, contador = 0;
	struct sockaddr_in serv_addr, cli_addr;	// estructuras para las direcciones de los sockets
	socklen_t clilen;	// variable para el tamaño de la estructura de arriba
	
	if(argc != 2) {
		fprintf(stderr,"ERROR, no indicó el puerto a usar\n");	// escribe al standard error
		fprintf(stderr,"Uso: %s num_puerto\n", argv[0]);	// que es la terminal, por defecto
		exit(1);
	}

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Crea el socket. "Connection based"
    if(sockfd < 0)
		error("ERROR abriendo el socket");

    // Llenar los campos de las estructuras
 	memset((char *)&serv_addr, 0, sizeof(serv_addr));	// "Limpiar" la estructura (0 todos los bytes)
	
    portno = atoi(argv[1]);	// Obtener el número de puerto del argumento de entrada
    serv_addr.sin_family = AF_INET;			// Constante que indica "Internet domain"
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Para recibir de cualquier interfaz de red
    serv_addr.sin_port = htons(portno);	 	// Convierte el número de puerto al formato requerido

    // Enlaza (binds) el socket a la dirección del anfitrión y el número de puerto
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    	error("ERROR al enlazar");

    listen(sockfd, 5);	// Para escuchar a conexiones que vengan. 5 es el tamaño de la cola para conexiones pend.
    clilen = sizeof(cli_addr);	// tamaño de la estructura

	// Para permitir que el servidor maneje múltiples conexiones simultaneas:
    // bucle infinito y fork.
    while(1)
    {
    	// bloquea hasta que un cliente se conecte al servidor.
		// Establece un nuevo file descriptor que usará el proceso hijo.
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR al aceptar");

        contador++;		// contador para las conexiones que se establecen.
        pid = fork();	// Se crea el proceso hijo, que manejará la conexión nueva
        if(pid < 0)
            error("ERROR en el fork");

        if(pid == 0)	// proceso hijo
        {
        	printf("Conexión #%d creada\n", contador);
            close(sockfd);		// cierra el socket original creado por el padre.
            dostuff(newsockfd);	// función que maneja la comunicación.
            exit(0);
        }
        else			// proceso padre
        {
        	close(newsockfd);	// cierra el socket creado para el hijo
        	signal(SIGCHLD,SIG_IGN);	// para prevenir el problema de procesos "zombie"
			// se ignoran las señales recibidas cuando los procesos hijo "mueren".
        }
    } 	// fin del bucle

    close(sockfd);	// En este programa no se llega aquí, pero conceptualmente se debe
	
    return 0; 		// cerrar el socket cuando se deja de usar.
}

/********************************* DOSTUFF() *****************************
 Hay una instancia distinta de esta función por cada conexión. Maneja la
 comunicación en cuanto se establece la conexión.
 *************************************************************************/
void dostuff (int sock)
{
	int n;
	char buffer[MSG_SIZE];

	memset(buffer, 0, MSG_SIZE);	// "limpia" el buffer
   
	// Leer data del socket.
	n = read(sock, buffer, MSG_SIZE-1);	// No es necesario usar recvfrom(), porque
				// la conexión ya se estableción, y el fd ya tiene la información 
				// necesaria para la comunicación entre el servidor y el cliente.
	if(n < 0)
		error("ERROR al leer del socket");

	printf("Mensaje recibido: %s\n", buffer);
	
	// Escribir un mensaje de vuelta
	n = write(sock, "I got your message", 18);	// No es necesario usar sendto().
				// Se pudo colocar el mensaje en una cadena, y usar strlen para
				// determinar el largo del mensaje.
	if(n < 0)
		error("ERROR al escribir al socket");
}

