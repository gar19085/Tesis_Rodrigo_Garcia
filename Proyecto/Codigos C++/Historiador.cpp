
/*
 ============================================================================
 Nombre: Historiador.cpp
 Autor:  Rodrigo José García Ambrosy
 Proyecto
 ============================================================================
 */
#include <iostream>
#include <thread>
#include <chrono>
#include <wiringPi.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

#define MSG_SIZE 301
#define IP "10.0.0.255"

int sockfd1,sockfd2, n;
unsigned int length;
struct sockaddr_in RTU, from1;
char buffer[MSG_SIZE];	// to store received messages or messages to be sent.
int boolval = 1;		// for a socket option

void enviar(void*ptr);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    std::thread thread1;

    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return 0;
    }

    sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);	// Creates socket. Connectionless.
    if (sockfd1 < 0) error("ERROR opening socket");

    RTU.sin_family = AF_INET;			// symbol constant for Internet domain
    RTU.sin_port = htons(atoi(argv[1]));		// port field
    RTU.sin_addr.s_addr = inet_addr(IP);		// IP address field. For server, this will be the IP address of the machine this program runs o
    length = sizeof(struct sockaddr_in);		// size of structure

    if (bind(sockfd1, (struct sockaddr *)&RTU, length)<0)	// Binds socket to an address
        error("ERROR on binding");

    if(setsockopt(sockfd1, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        std::cout << "error setting socket options" << std::endl;
        exit(-1);
    }

    thread1 = std::thread(enviar, (void*)0);

    std::cout << "Los comandos son los siguientes:" << std::endl;
    std::cout << "RTU# LED# 0 o 1" << std::endl;

    char CONNECT[128];
    std::strcpy(CONNECT, "Conexión");
    n = sendto(sockfd1, CONNECT, strlen(CONNECT), 0, (const struct sockaddr *)&RTU, length);
    if (n < 0) error("ERROR sendto");

    while(1){
        memset(buffer, 0, MSG_SIZE);
        n = recvfrom(sockfd1, buffer, MSG_SIZE, 0, (struct sockaddr *)&from1, &length);
        if (n < 0) error("ERROR recvfrom");
        std::cout << buffer << std::endl;
    }
}

void enviar(void*ptr)
{
    while(1){
        memset(buffer, 0, MSG_SIZE);
        std::cout << "Please enter the message (! to exit): " << std::endl;
        std::cin.getline(buffer, MSG_SIZE);
        if(buffer[0] == '!'){
            if((std::strcmp(buffer, "RTU1 LED1 1\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU1 LED1 0\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU1 LED2 1\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU1 LED2 0\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED1 1\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED1 0\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED2 1\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
            if((std::strcmp(buffer, "RTU2 LED2 0\n")) == 0){
                n = sendto(sockfd1, buffer, strlen(buffer), 0, (const struct sockaddr *)&RTU, length);
                if (n < 0) error("ERROR sendto");
            }
        } 
    }
}