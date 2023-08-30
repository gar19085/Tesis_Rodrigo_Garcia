/* 	Nombre     : 	client_udp_broadcast1.c
	Autor      : 	Luis A. Rivera
	Descripción: 	Cliente simple (broadcast), sin bind().
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

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n;
	unsigned int length;
	struct sockaddr_in anybody, from;
	char buffer[MSG_SIZE];	// to store received messages or messages to be sent.
	int boolval = 1;		// for a socket option

	if(argc != 2)
	{
		printf("usage: %s port\n", argv[0]);
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if(sockfd < 0)
		error("Error: socket");

	anybody.sin_family = AF_INET;		// symbol constant for Internet domain
	anybody.sin_port = htons(atoi(argv[1]));		// port number
	anybody.sin_addr.s_addr = inet_addr("192.168.1.255");	// broadcast address
				// Es posible que se deba cambiar. Podría ser 10.0.0.255 u otra.
				// Revisar con ifconfig/ipconfig

	length = sizeof(struct sockaddr_in);		// size of structure

	// change socket permissions to allow broadcast
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
		error("Error setting socket options\n");

	do
	{
		memset(buffer, 0, MSG_SIZE);	// sets all values to zero
		printf("Please enter the message (! to exit): ");
		fgets(buffer,MSG_SIZE-1,stdin); // MSG_SIZE-1 because a null character is added

		if(buffer[0] != '!')
		{
			// send message to anyone out there...
			n = sendto(sockfd, buffer, strlen(buffer), 0,
					  (const struct sockaddr *)&anybody, length);
			if(n < 0)
				error("Sendto");

            memset(buffer, 0, MSG_SIZE);
			// receive message
			n = recvfrom(sockfd, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
			if(n < 0)
				error("recvfrom");

			printf("Received something: %s\n", buffer);
		}
	} while(buffer[0] != '!');

	close(sockfd);	// close socket.
	
	return 0;
}
