/* 	Nombre     : Enviar_recibir_broadcast_hilos.c
	Autor      : Luis A. Rivera
	Descripción: Programa que despliega todo lo que reciba por un socket,
				 y transmite por broadcast todo lo que el usuario ingrese
				 en la terminal. En el hilo principal se configura el
				 socket y se tiene el ciclo que acepta mensajes del usuario
				 y los envía por broadcast. En un segundo hilo se hace la
				 recepción de los mensajes por la red, y el despliegue de
				 los mismos en la terminal.								*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define OPCION_IP 0	// 0 - dirección IP de broadcast "hard coded"
					// 1 - Funciona para la Raspberry Pi conectada a una WLAN
					// 2 - Funciona para CentOS en el J-305/306

#define MSG_SIZE 40			// message size
#define IP_LENGTH 15

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// Receiving thr: constantly waits for messages. Whatever is received is displayed.
void *receiving(void *ptr)
{
	int *sock, n;
	sock = (int *)ptr;		// socket identifier
	unsigned int length = sizeof(struct sockaddr_in);		// size of structure
	char buffer[MSG_SIZE];	// to store received messages or messages to be sent.
	struct sockaddr_in from;

	while(1)
	{
		memset(buffer, 0, MSG_SIZE);	// "limpia" el buffer
		// receive message
		n = recvfrom(*sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
		if(n < 0)
			error("Error: recvfrom");

		printf("Esto se recibió: %s\n", buffer);
	}

	pthread_exit(0);
}

// Main function
int main(int argc, char *argv[])
{
	int sock, n;
	unsigned int length = sizeof(struct sockaddr_in);	// size of structure
	char buffer[MSG_SIZE];		// to store received messages or messages to be sent.
	struct sockaddr_in anybody;	// for the socket configuration
	int boolval = 1;			// for a socket option
	pthread_t thread_rec;		// thread variable
	char IP_broadcast[IP_LENGTH];  // para la dirección de broadcast
    FILE *file;
    
	if(argc != 2)
	{
		printf("Uso: %s puerto\n", argv[0]);
		exit(1);
	}

#if OPCION_IP == 0
// --- hard coded -------------------------------------------------------------
	strcpy(IP_broadcast, "192.168.1.255");	// Puede que se deba cambiar. Revisar ifconfig
#endif

#if (OPCION_IP == 1) || (OPCION_IP == 2)
// --- get the broadcast address and save it to the ipaddr file ---------------

#if OPCION_IP == 1
	system("ifconfig wlan0 | grep 'inet ' | awk '{ print $6 }' > ipaddr");
#else
	system("ifconfig enp0s31f6 | grep 'inet ' | awk '{ print $6 }' > ipaddr");
#endif

	file = fopen("ipaddr", "r");	// open file with the broadcast address
	if(file == NULL)
	{
		printf("Error opening file");
		exit(-1);
	}
	else
	{
		fscanf(file, "%s", IP_broadcast);
	}

	fclose(file);			// close file
        
	system("rm ipaddr");	// make sure to remove file when done
// ----------------------------------------------------------------------------
#endif
	
	printf("La dirección de broadcast es: %s\n\n", IP_broadcast);

	anybody.sin_family = AF_INET;		// symbol constant for Internet domain
	anybody.sin_port = htons(atoi(argv[1]));		// port field
	anybody.sin_addr.s_addr = htonl(INADDR_ANY);	// para recibir de cualquiera

	sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if(sock < 0)
		error("Error: socket");

	// Sin el bind, no se reciben los mensajes
	if(bind(sock, (struct sockaddr *)&anybody, sizeof(struct sockaddr_in)) < 0)
	{
		printf("Error binding socket.\n");
		exit(-1);
	}

	// change socket permissions to allow broadcast
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
	{
		printf("Error setting socket options.\n");
		exit(-1);
	}

	anybody.sin_addr.s_addr = inet_addr(IP_broadcast);	// broadcast address

	// Se creae el hilo de recepción.
	pthread_create(&thread_rec, NULL, receiving, (void *)&sock);	// for receiving

	printf("Este programa despliega lo que sea que reciba.\n");
	printf("También transmite lo que el usuario ingrese, max. 40 caracteres. (! para salir):\n");
	do
	{
		memset(buffer, 0, MSG_SIZE);	// "limpia" el buffer
		fgets(buffer,MSG_SIZE-1,stdin); // MSG_SIZE-1 'cause a null character is added

		if(buffer[0] != '!')
		{
			// send message to anyone there...
			n = sendto(sock, buffer, strlen(buffer), 0,
					(const struct sockaddr *)&anybody, length);
			if(n < 0)
				error("Error: sendto");
		}
	} while(buffer[0] != '!');

	close(sock);			// close socket.
	return 0;
}
