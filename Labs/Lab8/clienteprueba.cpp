#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>

#define OPCION_IP 0
#define MSG_SIZE 60
#define IP_LENGTH 15

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void receiving(int sock)
{
    unsigned int length = sizeof(struct sockaddr_in);
    char buffer[MSG_SIZE];
    struct sockaddr_in from;

    while (true)
    {
        memset(buffer, 0, MSG_SIZE);
        int n = recvfrom(sock, buffer, MSG_SIZE, 0, reinterpret_cast<struct sockaddr *>(&from), &length);
        if (n < 0)
            error("Error: recvfrom");

        std::cout << "Esto se recibió: " << buffer << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int sock, n;
    unsigned int length = sizeof(struct sockaddr_in);
    char buffer[MSG_SIZE];
    struct sockaddr_in anybody;
    int boolval = 1;
    std::thread thread_rec;
    char IP_broadcast[IP_LENGTH];
    FILE *file;

    if (argc != 2)
    {
        std::cout << "Uso: " << argv[0] << " puerto" << std::endl;
        exit(1);
    }

#if OPCION_IP == 0
    strcpy(IP_broadcast, "192.168.1.255");
#endif

#if (OPCION_IP == 1) || (OPCION_IP == 2)
    // ...

    file = fopen("ipaddr", "r");
    if (file == nullptr)
    {
        std::cout << "Error opening file" << std::endl;
        exit(-1);
    }
    else
    {
        fscanf(file, "%s", IP_broadcast);
    }

    fclose(file);

    // ...
#endif

    std::cout << "La dirección de broadcast es: " << IP_broadcast << std::endl << std::endl;

    anybody.sin_family = AF_INET;
    anybody.sin_port = htons(atoi(argv[1]));
    anybody.sin_addr.s_addr = htonl(INADDR_ANY);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        error("Error: socket");

    if (bind(sock, reinterpret_cast<struct sockaddr *>(&anybody), sizeof(struct sockaddr_in)) < 0)
    {
        std::cout << "Error binding socket." << std::endl;
        exit(-1);
    }

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        std::cout << "Error setting socket options." << std::endl;
        exit(-1);
    }

    anybody.sin_addr.s_addr = inet_addr(IP_broadcast);

    thread_rec = std::thread(receiving, sock);

    std::cout << "Este programa despliega lo que sea que reciba." << std::endl;
    std::cout << "También transmite lo que el usuario ingrese, máx. " << MSG_SIZE << " caracteres. (! para salir):" << std::endl;

    do
    {
        memset(buffer, 0, MSG_SIZE);
        std::cin.getline(buffer, MSG_SIZE - 1);

        if (buffer[0] != '!')
        {
            n = sendto(sock, buffer, strlen(buffer), 0, reinterpret_cast<const struct sockaddr *>(&anybody), length);
            if (n < 0)
                error("Error: sendto");
        }
    } while (buffer[0] != '!');

    close(sock);
    thread_rec.join();

    return 0;
}
