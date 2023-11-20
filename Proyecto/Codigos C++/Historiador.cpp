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
#include <cstdio>  //Librería estándar de entrada/salida
#include <cstdlib> //Librería estándar para funciones generales
#include <cstring> //Librería para la manipulación de cadenas
#include <unistd.h> // Librería que proporciona funciones y constantes específicas de sistemas Unix.
#include <sys/types.h> // Librería para definir tipos de datos
#include <sys/socket.h> // Librería para utilizar sockets
#include <netinet/in.h> // Librería para utilizar direcciones de internet
#include <arpa/inet.h>  // Librería para manipular direcciones IP
#include <fstream>      //Librería para utilizar archivos


#define MSG_SIZE 3000 // Tamaño del mensaje
#define IP "192.168.1.255" // Dirección IP
#define OPCION_IP 0 /// 0 - hard coded
#define IP_LENGTH 15 // Tamaño de la dirección IP
int sockfd, n; // Descriptores de archivo
unsigned int length = sizeof(struct sockaddr_in);// Tamaño de la estructura sockaddr_in
struct sockaddr_in RTU, from1; // Estructuras para almacenar las direcciones de los sockets
char buffer[MSG_SIZE];	// Buffer para almacenar el mensaje
int boolval = 1;		// Opción de socket
char IP_broadcast[IP_LENGTH]; 

bool shouldExit = false; 

void enviar(void*ptr); // Declaración de la función enviar

void error(const char *msg) // Función para imprimir errores
{
    perror(msg);
    exit(0);
}


void receiving(int sock, std::ofstream& outputFile) // Función para recibir mensajes
{
    char buffer[MSG_SIZE]; // Buffer para almacenar el mensaje
    struct sockaddr_in from; // Estructura para almacenar la dirección del emisor

    while (!shouldExit) 
    {
        memset(buffer, 0, MSG_SIZE); 
        int n = recvfrom(sock, buffer, MSG_SIZE, 0, reinterpret_cast<struct sockaddr *>(&from), &length); // Recibe el mensaje
        if (n < 0) // Verifica si hubo error
            error("Error: recvfrom");

        std::cout << "Esto se recibió: " << buffer << std::endl; // Imprime el mensaje recibido
        outputFile << buffer << std::endl; // Escribe el mensaje en el archivo de salida
        if (std::strcmp(buffer, "!") == 0) // Verifica si el mensaje es "!"
        {
            shouldExit = true; // Establece shouldExit en true para salir del programa
            break;
        }
    }
}


int main(int argc, char *argv[]) // Función principal
{   
    std::ofstream outputFile("Lista de Eventos.txt");

    // Check if the file is successfully opened
    if (!outputFile.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return 1; // Return an error code
    }

    std::thread hilo1; // Declaración del hilo

    if (argc != 2) // Verifica si se ingresó el puerto
    {
        std::cout << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 0;
    }
#if OPCION_IP == 0 // Configuración de la dirección IP
    std::strcpy(IP_broadcast, "192.168.1.255");
#endif

    std::cout << "La dirección de broadcast es: " << IP_broadcast << std::endl;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Crea el socket. Es sin conexión.
    if (sockfd < 0) error("ERROR al abrir el socket");

    RTU.sin_family = AF_INET; // constante de símbolo para dominio de Internet
    RTU.sin_port = htons(atoi(argv[1])); // campo de puerto
    //RTU.sin_addr.s_addr = inet_addr(IP); // campo de dirección IP. Para el servidor, será la dirección IP de la máquina en la que se ejecuta este programa.
    
    
    if (bind(sockfd, reinterpret_cast<struct sockaddr *>(&RTU), length) < 0) // Asigna el socket
    {
        std::cout << "Error binding socket." << std::endl;
        exit(-1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0) // Configura el socket
    {
        std::cout << "error al configurar opciones del socket" << std::endl;
        exit(-1);
    }


    hilo1 = std::thread(enviar, (void *)0); // Crea el hilo

    std::cout << "Los comandos son los siguientes:" << std::endl; // Imprime los comandos
    std::cout << "RTU# LED# 0 o 1" << std::endl;
    std::cout << "RTU# LEDIoT 0 o 1" << std::endl;
    std::cout << "Utilizar ! para salir: " << std::endl; 
    RTU.sin_addr.s_addr = inet_addr(IP);
    char CONEXION[128]; // Variable para almacenar el mensaje
    
    std::strcpy(CONEXION, "Conexión"); // Copia el mensaje en la variable
    n = sendto(sockfd, CONEXION, strlen(CONEXION), 0, (const struct sockaddr *)&RTU, length); // Envia el mensaje
    if (n < 0){
        std::cerr << "ERROR al enviar" << std::endl; // Verifica si hubo error
    } 
    while (!shouldExit)
    {
        memset(buffer, 0, MSG_SIZE);
        receiving(sockfd, outputFile);
    }
    hilo1.join();
    outputFile.close(); // Cierra el archivo de salida
    std::cout << "Saliendo del Programa" << std::endl;
    return 0;
}

void enviar(void*ptr) // Función para enviar mensajes
{
    while(!shouldExit){
        memset(buffer, 0, MSG_SIZE); // Limpia el buffer
        std::cin.getline(buffer, MSG_SIZE - 1); // Lee el mensaje

        std::cout << "Mandando mensaje: " << buffer << std::endl;
        n = sendto(sockfd, buffer, strlen(buffer), 0,
        (const struct sockaddr *)&RTU, length);
        if (n < 0) error("ERROR sendto");

        if((std::strcmp(buffer, "RTU1 LED1 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED1 de la RTU1
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU1 LED1 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED1 de la RTU1
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU1 LED2 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED2 de la RTU1
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU1 LED2 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED2 de la RTU1
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU2 LED1 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED1 de la RTU2
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU2 LED1 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED1 de la RTU2
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU2 LED2 1\n")) == 0){ // Verifica si se ingresó el comando para encender el LED2 de la RTU2
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU2 LED2 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED2 de la RTU2
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU1 LEDIoT 1\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED2 de la RTU2
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }
        if((std::strcmp(buffer, "RTU1 LEDIoT 0\n")) == 0){ // Verifica si se ingresó el comando para apagar el LED2 de la RTU2
            n = sendto(sockfd, buffer, strlen(buffer), 0, 
            (const struct sockaddr *)&RTU, length);
            if (n < 0) error("ERROR sendto");
        }  
    }
}