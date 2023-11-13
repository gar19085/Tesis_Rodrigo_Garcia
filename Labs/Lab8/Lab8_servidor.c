/* 	Nombre     : 	Lab8_servidor.c
	Author     : 	Luis A. Rivera
	Descripción: 	IE3059 lab
					Lab 8. Sockets: Voto Master/Slave. SERVIDOR				*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <ctype.h>
//#include <sys/mman.h>

#define OPCION_IP 1	// 0 - hard coded
					// 1 - Raspberry Pi / CentOS
					// 2 - CentOS J-305/306
					// 3 - Cygwin
#define TRUE 		   1
#define FALSE 		   0
#define MSG_SIZE	  60
#define VOTO_MAX	  15
#define IP_LENGTH	  15
#define PORT		2000	// change this to 2000

int getIP(char *);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n;
	unsigned int length;
	struct sockaddr_in addr, broadcast_addr;
	char messg[MSG_SIZE], messg_temp[MSG_SIZE];	// max vote value is 10 (2 digits)
	int myvote = 0, myIP, incomingIP, in_vote = 0;
	int boolval = TRUE, master = FALSE;
	char *vote_tokens;
	char IP_buffer[IP_LENGTH], IP_temp[IP_LENGTH], IP_broadcast[IP_LENGTH];
	int puerto = PORT;
	FILE *file;

	if(argc > 2)
	{
		printf("Uso: %s puerto\n", argv[0]);
		exit(1);
	}
	
	if(argc == 2)
		puerto = atoi(argv[1]);

#if OPCION_IP == 0
// --- hard coded -------------------------------------------------------------
	strcpy(IP_buffer, "10.0.0.22");
	strcpy(IP_broadcast, "10.0.0.255");
#endif

#if (OPCION_IP == 1) || (OPCION_IP == 2)
// ----- get the IP address and save to a file (ipaddr) using ifconfig --------
	
#if OPCION_IP == 1
	system("ifconfig wlan0 | grep 'inet ' | awk '{ print $2 }' > ipaddr");
#else
	system("ifconfig enp0s31f6 | grep 'inet ' | awk '{ print $2 }' > ipaddr");
#endif

	file = fopen("ipaddr", "r");	// open file with the IP address
	if(file == NULL)
	{
		printf("Error opening file");
		exit(-1);
	}
	else
	{
		fscanf(file, "%s", IP_buffer);
	}

	fclose(file);					// close file
    
// --- get the broadcast address and save it to the ipaddr file ---
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

	fclose(file);					// close file
	system("rm ipaddr");			// make sure to remove file when done
#endif

#if OPCION_IP == 3
// ---------- get the IP address using gethostbyname --------------------------
	struct hostent *he;
	struct in_addr **addr_list;
	int i = 0;
	char hostname[128];

	gethostname(hostname, sizeof hostname);

	if((he = gethostbyname(hostname)) == NULL) {  // get the host info
		herror("gethostbyname");
		return 2;
	}

// Lo siguiente funciona en Cygwin en mi laptop. Puede que en otras compu-
// tadoras haya más elementos en addr_list.
	addr_list = (struct in_addr **)he->h_addr_list;
	strcpy(IP_buffer, inet_ntoa(*addr_list[3])); // i. Habría que ver cuántos elementos hay.
									// y escoger la opción adecuada. 3 funciona en mi laptop
	strcpy(IP_broadcast, "192.168.1.255");	// Habría que construir la IP de broadcast a partir
											// de la propia.
#endif
// ----------------------------------------------------------------------------

	printf("Mi IP es: %s\n\n", IP_buffer);
	printf("La dirección de broadcast es: %s\n\n", IP_broadcast);

	strcpy(IP_temp, IP_buffer);		// necessary because the next functions modifies
	myIP = getIP(IP_temp); // get last number of the IP address (e.g. 16 in 10.3.52.16)

	srand(time(NULL));				// for the rand calls later on

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
		error("Opening socket");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(puerto);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons(puerto);
	broadcast_addr.sin_addr.s_addr = inet_addr(IP_broadcast);

	length = sizeof(addr);			// size of structure

	// binds the socket to the address of the host and the port number
	if(bind(sockfd, (struct sockaddr *)&addr, length) < 0)
		error("Error binding socket.");

	// change socket permissions to allow broadcast
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
		error("Error setting socket options\n");

	while(1)
	{
		memset(messg, 0, MSG_SIZE);	// sets all values to zero.
		
		// receive from a client
		n = recvfrom(sockfd, messg, MSG_SIZE, 0, (struct sockaddr *)&addr, &length);
	    if(n < 0)
	 		error("recvfrom"); 

		if(strncmp(messg, "QUIEN ES", 8) == 0)
		{
			if(master == TRUE)
			{
				printf("A punto de informar que soy el master\n");
				sprintf(messg, "Luis en %s es el Master", IP_buffer);
				//strcpy(messg, "Luis es el Master");
				sendto(sockfd, messg, sizeof(messg), 0, (struct sockaddr *)&addr,
					   sizeof(struct sockaddr));
			}
			else
			{
				printf("No soy el Master, así que no envío ningún mensaje...\n");
			}
		}
		else if(strncmp(messg, "VOTE", 4) == 0)
		{
			memset(messg, 0, MSG_SIZE);		// "limpia" el buffer
			myvote = rand()%VOTO_MAX + 1;	// El voto aleatorio entre 1 y VOTO_MAX.
			//myvote = 5;
			master = TRUE;			// In case nobody else votes.
			sprintf(messg, "# %s %d", IP_buffer, myvote);	// get message ready

			printf("A punto de mandar mi IP y mi voto: %s\n", messg);
			sendto(sockfd, messg, sizeof(messg), 0, (struct sockaddr *)&broadcast_addr,
				   sizeof(struct sockaddr));
		}
		else if(messg[0] == '#')
		{
			// Extract incoming vote and IP address from messg string...
			strcpy(messg_temp, messg);	// necessary because getIP changes the argument
			incomingIP = getIP(messg_temp);		// get last number of IP

			if(incomingIP != myIP)		// Si es mi propio voto, lo ignoro.
			{
				printf("Alguien más votó: %s\n", messg);
				// Get the vote from the incoming message
				vote_tokens = strtok(messg," #.");
				while(vote_tokens != NULL)
				{
					vote_tokens = strtok(NULL, " #.");
					if(vote_tokens != NULL)
						in_vote = atoi(vote_tokens);
				}

				if(in_vote > myvote)	// Recibí un voto mayor
				{
					if(master == FALSE)
					{
						printf("Voto mayor detectado. No importa, no soy el Master.\n");
					}
					else
					{
						printf("Voto mayor detectado. Dejaré de ser el Master.\n");
						master = FALSE;
					}
				}
				else if(in_vote == myvote)	// Recibí el mismo voto (empate)
				{
					if(master == FALSE)
					{
						printf("Voto igual detectado. No importa, no soy el Master.\n");
					}
					else
					{
						if(incomingIP < myIP)	// Era el Master pero pierdo el tie-break
						{
							master = FALSE;
							printf("Perdí el desempate. Dejaré de ser el Master.\n");
						}
						else	// Era el Master y gano el tie-break
						{
							printf("Gané el desempate. Sigo siendo el Master.\n");
						}
					}
				}
				else 	// Recibí un voto menor
				{
					if(master == FALSE)
						printf("Voto menor detectado. No importa, no soy el Master.\n");
					else
						printf("Voto menor detectado. Sigo siendo el Master.\n");
				}
			}
		}
		else
		{
			printf("Recibí un mensaje inválido: %s\n", messg);
		}
	}

	return 0;
}

// Get the last number of the IP contained in the string IP_buffer
// For example: if IP_buffer = "10.3.52.16", we would get 16
int getIP(char *IP_buffer)
{
	char *IPptr;
	// split strings into pieces (tokens) using strtok
	IPptr = strtok(IP_buffer, ".");
	IPptr = strtok(NULL, ".");
	IPptr = strtok(NULL, ".");
	IPptr = strtok(NULL, ". ");
	return(atoi(IPptr));
}


