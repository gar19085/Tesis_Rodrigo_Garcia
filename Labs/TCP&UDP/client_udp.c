/* 	Nombre     : 	client_udp.c
	Autor      : 	Luis A. Rivera
	Descripción: 	Cliente simple (UDP)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MSG_SIZE 40		// Tamaño (máximo) del mensaje. Puede ser mayor a 40.

void error(const char *);

int main(int argc, char *argv[])
{
	int sockfd, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[MSG_SIZE];

	if(argc != 3)
	{
		printf("usage %s hostname port\n", argv[0]);
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if(sockfd < 0)
		error("socket");

	server.sin_family = AF_INET;	// symbol constant for Internet domain
	hp = gethostbyname(argv[1]);	// converts hostname input (e.g. 192.168.1.101)
	if(hp == 0)
		error("Unknown host");

	memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);
	
	server.sin_port = htons(atoi(argv[2]));	// port field
	length = sizeof(struct sockaddr_in);	// size of structure

	printf("Please enter the message: ");
	memset(buffer, 0, MSG_SIZE);	// sets all values to zero
	fgets(buffer,MSG_SIZE-1,stdin); // MSG_SIZE-1 because a null character is added

	// send message to server
	// Usamos sendto(), indicando a quién mandamos el mensaje. A diferencia
	// de TCP, acá no se establece la conexión. Necesitamos indicar a quién
	// enviamos cada vez.
	n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&server, length);
	if(n < 0)
		error("Sendto");

	// receive from server
	// Usamos recvfrom(), indicando de quién recibimos el mensaje.
	n = recvfrom(sockfd, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
	if(n < 0)
		error("recvfrom");

	printf("Got an acknowledgment: %s\n", buffer);
	close(sockfd);	// close socket.
	
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}
