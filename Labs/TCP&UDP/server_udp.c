/* 	Nombre     : 	server_udp.c
	Autor      : 	Luis A. Rivera
	Descripción: 	Servidor simple (UDP)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MSG_SIZE 40		// Tamaño (máximo) del mensaje. Puede ser mayor a 40.

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, length, n;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buffer[MSG_SIZE];

	if(argc != 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if(sockfd < 0)
		error("Opening socket");

	length = sizeof(server);			// length of structure
	memset((char *)&server, 0, length); // sets all values to zero.
	server.sin_family = AF_INET;		// symbol constant for Internet domain
	server.sin_addr.s_addr = htonl(INADDR_ANY);	// para recibir de cualquier interfaz de red
	server.sin_port = htons(atoi(argv[1]));	// port number

	// binds the socket to the address of the host and the port number
	if(bind(sockfd, (struct sockaddr *)&server, length) < 0)
       error("binding");

	fromlen = sizeof(struct sockaddr_in);	// size of structure

	while(1)
	{
		memset(buffer, 0, MSG_SIZE);	// sets all values to zero.
		
		// receive from client
		// Usamos recvfrom(), indicando de quién esperamos el mensaje. A diferencia
		// de TCP, acá no se establece la conexión. Necesitamos indicar de quién
		// recibimos cada vez.
		n = recvfrom(sockfd, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &fromlen);
		if(n < 0)
	 		error("recvfrom");

		printf("Received a datagram: %s\n", buffer);
		 
		// send message to client
		// Usamos sendto(), indicando a quién mandamos el mensaje.
		n = sendto(sockfd, "Got your message\n", MSG_SIZE, 0, (struct sockaddr *)&from, fromlen);
		if(n < 0)
	 		error("sendto");
	}

	return 0;
 }

