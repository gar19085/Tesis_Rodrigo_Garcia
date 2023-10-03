
/*
 ============================================================================
 Nombre: Historiador.cpp
 Autor:  Rodrigo José García Ambrosy
 Proyecto
 ============================================================================
 */
#include <iostream> // Librería estándar de entrada/salida
#include <thread>   // Librería para utilizar hilos
#include <chrono>   // Librería para utilizar la función sleep_for
#include <wiringPi.h> // Librería para controlar pines GPIO
#include <cstdio>  //Librería estándar de entrada/salida
#include <cstdlib> //Librería estándar para funciones generales
#include <cstring> //Librería para la manipulación de cadenas
#include <unistd.h> // Librería que proporciona funciones y constantes específicas de sistemas Unix.
#include <sys/types.h> // Librería para definir tipos de datos
#include <sys/socket.h> // Librería para utilizar sockets
#include <netinet/in.h> // Librería para utilizar direcciones de internet
#include <arpa/inet.h>  // Librería para manipular direcciones IP

#define MSG_SIZE 301 // Tamaño del mensaje
#define IP "10.0.0.255" // Dirección IP

int sockfd1,sockfd2, n; // Descriptores de archivo
unsigned int length;    // Tamaño de la estructura sockaddr_in
struct sockaddr_in RTU, from1; // Estructuras para almacenar las direcciones de los sockets
char buffer[MSG_SIZE];	// Buffer para almacenar el mensaje
int boolval = 1;		// Opción de socket

void enviar(void*ptr); // Declaración de la función enviar

void error(const char *msg) // Función para imprimir errores
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) // Función principal
{
    std::thread hilo1; // Declaración del hilo

    if (argc != 2) // Verifica si se ingresó el puerto
    {
        std::cout << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 0;
    }

    sockfd1 = socket(AF_INET, SOCK_DGRAM, 0); // Crea el socket. Es sin conexión.
    if (sockfd1 < 0) error("ERROR al abrir el socket");

    RTU.sin_family = AF_INET; // constante de símbolo para dominio de Internet
    RTU.sin_port = htons(atoi(argv[1])); // campo de puerto
    RTU.sin_addr.s_addr = inet_addr(IP); // campo de dirección IP. Para el servidor, será la dirección IP de la máquina en la que se ejecuta este programa.
    length = sizeof(struct sockaddr_in); // tamaño de la estructura

    if (bind(sockfd1, (struct sockaddr *)&RTU, length) < 0) // Vincula el socket a una dirección
        error("ERROR al vincular");

    if (setsockopt(sockfd1, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0) // Configura el socket
    {
        std::cout << "error al configurar opciones del socket" << std::endl;
        exit(-1);
    }

    hilo1 = std::thread(enviar, (void *)0); // Crea el hilo

    std::cout << "Los comandos son los siguientes:" << std::endl; // Imprime los comandos
    std::cout << "RTU# LED# 0 o 1" << std::endl;

    char CONEXION[128]; // Variable para almacenar el mensaje
    std::strcpy(CONEXION, "Conexión"); // Copia el mensaje en la variable
    n = sendto(sockfd1, CONEXION, strlen(CONEXION), 0, (const struct sockaddr *)&RTU, length); // Envia el mensaje
    if (n < 0) error("ERROR al enviar"); // Verifica si hubo error

    while (1)
    {
        memset(buffer, 0, MSG_SIZE); // Limpia el buffer
        n = recvfrom(sockfd1, buffer, MSG_SIZE, 0, (struct sockaddr *)&from1, &length); // Recibe el mensaje
        if (n < 0) error("ERROR al recibir"); // Verifica si hubo error
        std::cout << buffer << std::endl;
    }
}

void enviar(void*ptr) // Función para enviar mensajes
{
    while(1){
        memset(buffer, 0, MSG_SIZE); // Limpia el buffer
        std::cout << "Please enter the message (! to exit): " << std::endl; 
        std::cin.getline(buffer, MSG_SIZE); // Lee el mensaje
        if(buffer[0] == '!'){ // Verifica si se ingresó el comando para salir
            if((std::strcmp(buffer, "RTU1 LED1 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED1 de la RTU1
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU1 LED1 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED1 de la RTU1
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU1 LED2 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED2 de la RTU1
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU1 LED2 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED2 de la RTU1
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED1 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED1 de la RTU2
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED1 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED1 de la RTU2
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED2 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED2 de la RTU2
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED2 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED2 de la RTU2
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
        } 
    }
}