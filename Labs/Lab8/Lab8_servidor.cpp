/*
 ============================================================================
 Nombre: Lab8_servidor.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream> 

#define OPCION_IP 1
#define TRUE 1
#define FALSE 0
#define MSG_SIZE 60
#define VOTO_MAX 15
#define IP_LENGTH 15
#define PORT 2000

int getIP(char *);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in addr, broadcast_addr;
    char messg[MSG_SIZE], messg_temp[MSG_SIZE];
    int myvote = 0, myIP, incomingIP, in_vote = 0;
    int boolval = TRUE, master = FALSE;
    char *vote_tokens;
    char IP_buffer[IP_LENGTH], IP_temp[IP_LENGTH], IP_broadcast[IP_LENGTH];
    int puerto = PORT;
    FILE *file;

    if (argc > 2)
    {
        std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        exit(1);
    }

    if (argc == 2)
        puerto = std::atoi(argv[1]);
    
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
		std::cout << "Error opening file" << std::endl;
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
		std::cout << "Error opening file" << std::endl;;
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

	std::cout << "Mi IP es: " << IP_buffer << std::endl;
	std::cout << "La dirección de broadcast es: " << IP_broadcast << std::endl;

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
				std::cout << "A punto de informar que soy el master" << std::endl;
				
				std::stringstream ss;
				ss << "Luis en " << IP_buffer << " es el Master";
				std::string messg = ss.str();   // Convertir a std::string

				sendto(sockfd, messg.c_str(), messg.length(), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));

			}
			else
			{
				std::cout <<  "No soy el Master, así que no envío ningún mensaje..."<< std::endl;
			}
		}
		else if(strncmp(messg, "VOTE", 4) == 0)
		{
			memset(messg, 0, MSG_SIZE);		// "limpia" el buffer
			myvote = rand()%VOTO_MAX + 1;	// El voto aleatorio entre 1 y VOTO_MAX.
			//myvote = 5;
			master = TRUE;			// In case nobody else votes.

			std::stringstream ss;
			ss << IP_buffer << myvote;
			std::string messg = ss.str();

			std::cout << "A punto de mandar mi IP y mi voto: " << messg << std::endl;
			sendto(sockfd, messg.c_str(), messg.length(), 0, (struct sockaddr *)&broadcast_addr, sizeof(struct sockaddr));

		}
		else if(messg[0] == '#')
		{
			// Extract incoming vote and IP address from messg string...
			strcpy(messg_temp, messg);	// necessary because getIP changes the argument
			incomingIP = getIP(messg_temp);		// get last number of IP

			if(incomingIP != myIP)		// Si es mi propio voto, lo ignoro.
			{
				std::cout << "Alguien más votó: " << messg << std::endl;
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
						std::cout << "Voto mayor detectado. No importa, no soy el Master. " << std::endl;
					}
					else
					{
						std::cout << "Voto mayor detectado. Dejaré de ser el Master." << std::endl;
						master = FALSE;
					}
				}
				else if(in_vote == myvote)	// Recibí el mismo voto (empate)
				{
					if(master == FALSE)
					{
						std::cout << "Voto igual detectado. No importa, no soy el Master." << std::endl;
					}
					else
					{
						if(incomingIP < myIP)	// Era el Master pero pierdo el tie-break
						{
							master = FALSE;
							std::cout << "Perdí el desempate. Dejaré de ser el Master." << std::endl;
						}
						else	// Era el Master y gano el tie-break
						{
							std::cout << "Gané el desempate. Sigo siendo el Master."<< std::endl;
						}
					}
				}
				else 	// Recibí un voto menor
				{
					if(master == FALSE)
						std::cout << "Voto menor detectado. No importa, no soy el Master."<< std::endl;
					else
						std::cout << "Voto menor detectado. Sigo siendo el Master." << std::endl;
				}
			}
		}
		else
		{
			std::cout << "Recibí un mensaje inválido: " << messg << std::endl;
		}
	}

	return 0; 
}

int getIP(const std::string &IP_buffer)
{
    size_t pos = IP_buffer.rfind('.');
    if (pos != std::string::npos)
    {
        std::string IP_part = IP_buffer.substr(pos + 1);
        return std::stoi(IP_part);
    }
    return 0;
}