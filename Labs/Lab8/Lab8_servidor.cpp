/*
 ============================================================================
 Nombre: Lab8_servidor.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream>	// Librería estándar de entrada/salida
#include <cstdlib>	// Librería estándar para funciones generales
#include <ctime>	// Librería para utilizar la función time
#include <cstring>	// Librería para la manipulación de cadenas
#include <cstdio>	// Librería para utilizar la función sprintf
#include <unistd.h>	// Librería que proporciona funciones y constantes específicas de sistemas Unix.
#include <fcntl.h>	// Librería para utilizar la función open
#include <sys/types.h>	// Librería para definir tipos de datos
#include <netdb.h>	// Librería para definir estructuras que almacenan información sobre hosts
#include <sys/socket.h>	// Librería para utilizar sockets
#include <arpa/inet.h>	// Librería para manipular direcciones IP
#include <sstream> 		// Librería para utilizar la función stringstream

#define OPCION_IP 1 // 0 - hard coded
					// 1 - Raspberry Pi / CentOS
					// 2 - CentOS J-305/306
#define TRUE 1	// Definición de TRUE
#define FALSE 0	// Definición de FALSE
#define MSG_SIZE 60	// Tamaño del mensaje
#define VOTO_MAX 15	// Voto máximo
#define IP_LENGTH 15	// Tamaño de la dirección IP
#define PORT 2044	// Puerto de comunicación

int getIP(char *);	// Función para obtener la dirección IP

void error(const char *msg) // Función para imprimir errores
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){ // Función principal
    int sockfd, n; // Declaración de variables
    unsigned int length; 
    struct sockaddr_in addr, broadcast_addr; // Estructuras para almacenar las direcciones
    char messg[MSG_SIZE], messg_temp[MSG_SIZE]; // Variables para almacenar el mensaje
    int myvote = 0, myIP, incomingIP, in_vote = 0; // Variables para almacenar el voto y la dirección IP
    int boolval = TRUE, master = FALSE; // Variables para configurar el socket
    char *vote_tokens;  // Variable para almacenar el voto
    char IP_buffer[IP_LENGTH], IP_temp[IP_LENGTH], IP_broadcast[IP_LENGTH]; // Variables para almacenar la dirección IP
    int puerto = PORT; // Variable para almacenar el puerto
    FILE *file; // Declaración del archivo

    if (argc > 2) // Verifica si se ingresó el puerto
    {
        std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        exit(1);
    }

    if (argc == 2) 
        puerto = std::atoi(argv[1]);
    
#if OPCION_IP == 0 //
// --- hard coded -------------------------------------------------------------
	strcpy(IP_buffer, "192.168.20.227");
	strcpy(IP_broadcast, "192.168.20.255");
#endif

#if (OPCION_IP == 1) || (OPCION_IP == 2) 
// ----- Obtener la dirección IP y guardarla en un archivo (ipaddr) usando ifconfig --------
	
#if OPCION_IP == 1 // Se obtiene la IP de wlan0
	system("ifconfig wlan0 | grep 'inet ' | awk '{ print $2 }' > ipaddr");
#else
	system("ifconfig enp0s31f6 | grep 'inet ' | awk '{ print $2 }' > ipaddr");
#endif

	file = fopen("ipaddr", "r");	// Abre el archivo con la IP
	if(file == NULL) // Verifica si se abrio el archivo
	{
		std::cout << "Error opening file" << std::endl;
		exit(-1);
	}
	else
	{
		fscanf(file, "%s", IP_buffer);
	}

	fclose(file);					// Cierra el archivo
   
// --- Obtener la dirección de broadcast y guardarla en el archivo ipaddr ---
#if OPCION_IP == 1
	system("ifconfig wlan0 | grep 'inet ' | awk '{ print $6 }' > ipaddr"); // Ejecutar comando para obtener la dirección de broadcast
#else
	system("ifconfig enp0s31f6 | grep 'inet ' | awk '{ print $6 }' > ipaddr"); 
#endif

	file = fopen("ipaddr", "r");	// Abrir archivo con la dirección de broadcast
	if(file == NULL)
	{
		std::cout << "Error al abrir el archivo" << std::endl;;
		exit(-1);
	}
	else
	{
		fscanf(file, "%s", IP_broadcast); // Leer la dirección de broadcast del archivo
	}

	fclose(file);					// Cerrar archivo
	system("rm ipaddr");			// Asegurarse de eliminar el archivo cuando se termine

#endif

	std::cout << "Mi IP es: " << IP_buffer << std::endl; // Imprime la dirección IP
	std::cout << "La dirección de broadcast es: " << IP_broadcast << std::endl; // Imprime la dirección de boradcast

	strcpy(IP_temp, IP_buffer);		// Necesario porque getIP cambia el argumento
	myIP = getIP(IP_temp); // Obtener el último número de la IP

	srand(time(NULL));				// Inicializa el generador de números aleatorios

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Crea el socket
	if(sockfd < 0) // Verifica si hubo error
		error("Opening socket");

	addr.sin_family = AF_INET; // Configura la familia de direcciones
	addr.sin_port = htons(puerto); // Configura el puerto
	addr.sin_addr.s_addr = htonl(INADDR_ANY); // Configura la dirección IP

	broadcast_addr.sin_family = AF_INET; 
	broadcast_addr.sin_port = htons(puerto); 
	broadcast_addr.sin_addr.s_addr = inet_addr(IP_broadcast); 

	length = sizeof(addr);			// Tamaño de la dirección

	// Vincula el socket a la dirección del host y al número de puerto
	if(bind(sockfd, (struct sockaddr *)&addr, length) < 0)
    	error("Error al vincular el socket.");

	// Cambia los permisos del socket para permitir el broadcast
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    	error("Error al configurar las opciones del socket\n");

	while(1)
	{
		memset(messg, 0, MSG_SIZE);	// Limpia el buffer
		
		// Recibe un mensaje del cliente
		n = recvfrom(sockfd, messg, MSG_SIZE, 0, (struct sockaddr *)&addr, &length);
	    if(n < 0)
	 		error("recvfrom"); 
 
		if(strncmp(messg, "QUIEN ES", 8) == 0) // Verifica si el mensaje es "QUIEN ES"
		{
			if(master == TRUE) // Verifica si es el Master
			{
				std::cout << "A punto de informar que soy el master" << std::endl;
				
				std::stringstream ss;
				ss << "Rodrigo RP4 en " << IP_buffer << " es el Master";
				std::string messg = ss.str();   // Convertir a std::string

				sendto(sockfd, messg.c_str(), messg.length(), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));

			}
			else
			{
				std::cout <<  "No soy el Master, así que no envío ningún mensaje..."<< std::endl;
			}
		}
		else if(strncmp(messg, "VOTE", 4) == 0) // Verifica si el mensaje es "VOTE"
		{
			memset(messg, 0, MSG_SIZE);		// Limpia el buffer
			myvote = rand()%VOTO_MAX + 1;	// El voto aleatorio entre 1 y VOTO_MAX.
			//myvote = 5;
			master = TRUE;			// En caso de que nadie más vote, yo seré el Master

			std::stringstream ss; // Convertir a std::string
			ss << "#" << IP_buffer << " " << myvote; 
			std::string messg = ss.str(); 

			std::cout << "A punto de mandar mi IP y mi voto: " << messg << std::endl; // Imprime la dirección IP y el voto
			sendto(sockfd, messg.c_str(), messg.length(), 0, (struct sockaddr *)&broadcast_addr, sizeof(struct sockaddr)); // Envía el mensaje

		}
		else if(messg[0] == '#') // Comprueba si el mensaje es un voto
		{
    		// Extrae el voto entrante y la dirección IP del mensaje...
    		strcpy(messg_temp, messg); // necesario porque getIP cambia el argumento
    		incomingIP = getIP(messg_temp); // obtiene el último número de la IP

    		if(incomingIP != myIP) // Si es mi propio voto, lo ignoro.
    		{
        		std::cout << "Alguien más votó: " << messg << std::endl;
        		// Obtén el voto del mensaje entrante
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

int getIP(char *IP_buffer) // Función para obtener la dirección IP

{

	char *IPptr;
	IPptr = strtok(IP_buffer, ".");
	IPptr = strtok(NULL, ".");
	IPptr = strtok(NULL, ".");
	IPptr = strtok(NULL, ". ");
	return (atoi(IPptr));

}