/*
 ============================================================================
 Nombre: Lab8_cliente.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream> // Librería estándar de entrada/salida
#include <cstdlib>  // Librería estándar para funciones generales
#include <unistd.h> // Librería que proporciona funciones y constantes específicas de sistemas Unix.
#include <cstring>  // Librería para la manipulación de cadenas
#include <sys/types.h> // Librería para definir tipos de datos
#include <sys/socket.h> // Librería para utilizar sockets
#include <netinet/in.h> // Librería para utilizar direcciones de internet
#include <netdb.h>  // Librería para definir estructuras que almacenan información sobre hosts
#include <arpa/inet.h> // Librería para manipular direcciones IP
#include <thread>  // Librería para utilizar hilos
#include <chrono>  // Librería para utilizar la función sleep_for

#define OPCION_IP 1 /// 0 - hard coded
					// 1 - Raspberry Pi
#define MSG_SIZE 60	
#define IP_LENGTH 15 // Tamaño de la dirección IP

void error(const char *msg) 
{
    perror(msg);
    exit(0);
}

void receiving(int sock) // Función para recibir mensajes
{
    unsigned int length = sizeof(struct sockaddr_in); // Tamaño de la estructura sockaddr_in
    char buffer[MSG_SIZE]; // Buffer para almacenar el mensaje
    struct sockaddr_in from; // Estructura para almacenar la dirección del emisor

    while (true) 
    {
        memset(buffer, 0, MSG_SIZE); 
        int n = recvfrom(sock, buffer, MSG_SIZE, 0, reinterpret_cast<struct sockaddr *>(&from), &length); // Recibe el mensaje
        if (n < 0) // Verifica si hubo error
            error("Error: recvfrom");

        std::cout << "Esto se recibió: " << buffer << std::endl; // Imprime el mensaje recibido
    }
}

int main(int argc, char *argv[]) // Función principal
{
    int sock, n; 
    unsigned int length = sizeof(struct sockaddr_in);
    char buffer[MSG_SIZE];
    struct sockaddr_in anybody; // Estructura para almacenar la dirección del receptor
    int boolval = 1; // Variable para configurar el socket
    std::thread thread_rec; // Declaración del hilo
    char IP_broadcast[IP_LENGTH]; // Variable para almacenar la dirección IP
    FILE *file; // Declaración del archivo

    if (argc != 2) // Verifica si se ingresó el puerto
    {
        std::cout << "Uso: " << argv[0] << " puerto" << std::endl;
        exit(1);
    }

#if OPCION_IP == 0 // Configuración de la dirección IP
    std::strcpy(IP_broadcast, "192.168.90.255");
#endif

#if (OPCION_IP == 1) || (OPCION_IP == 2) // Configuración de la dirección IP

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

    std::cout << "La dirección de broadcast es: " << IP_broadcast << std::endl; // Imprime la dirección IP

    anybody.sin_family = AF_INET; // Configura la familia de direcciones
    anybody.sin_port = htons(atoi(argv[1])); // Configura el puerto
    anybody.sin_addr.s_addr = htonl(INADDR_ANY); // Configura la dirección IP

    sock = socket(AF_INET, SOCK_DGRAM, 0); // Crea el socket
    if (sock < 0) // Verifica si hubo error
        error("Error: socket");

    if (bind(sock, reinterpret_cast<struct sockaddr *>(&anybody), sizeof(struct sockaddr_in)) < 0) // Asigna el socket
    {
        std::cout << "Error binding socket." << std::endl;
        exit(-1);
    }

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0) // Configura el socket
    {
        std::cout << "Error setting socket options." << std::endl;
        exit(-1);
    }

    anybody.sin_addr.s_addr = inet_addr(IP_broadcast); // Configura la dirección IP

    thread_rec = std::thread(receiving, sock); // Crea el hilo
 
    std::cout << "Este programa despliega lo que sea que reciba." << std::endl; 
    std::cout << "También transmite lo que el usuario ingrese, máx. " << MSG_SIZE << " caracteres. (! para salir):" << std::endl;

    do // Ciclo para enviar mensajes
    {
        memset(buffer, 0, MSG_SIZE); // Limpia el buffer
        std::cin.getline(buffer, MSG_SIZE - 1); // Lee la entrada del usuario

        if (buffer[0] != '!') // Verifica si el usuario quiere salir
        {
            n = sendto(sock, buffer, strlen(buffer), 0, reinterpret_cast<const struct sockaddr *>(&anybody), length); // Envía el mensaje
            if (n < 0) // Verifica si hubo error
                error("Error: sendto");
        }
    } while (buffer[0] != '!'); 

    close(sock); // Cierra el socket
    thread_rec.join(); // Espera a que el hilo termine

    return 0;
}
