//I made a different version of what we did, can you help me check if it's correct? Also I tried to recreate my program named Lab8_servidor.c but in C++
//I'm not sure if it's correct, can you help me check it? I'm not sure if I'm using the right functions and if I'm using them correctly.
//I'm not sure if I'm using the right functions and if I'm using them correctly.
//I'm not sure if I'm using the right functions and if I'm using them correctly.




#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <random>
#include <chrono>

#define IP_LENGTH 15
#define PORT 2000
#define MSG_SIZE 60
#define TRUE 1
#define FALSE 0
#define VOTO_MAX 15
#define OPCION_IP 2

int getIP(std::string& IP_buffer);

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in addr, broadcast_addr;
    std::array<char, MSG_SIZE> messg;
    std::array<char, MSG_SIZE> messg_temp;
    int myvote = 0, myIP, incomingIP, in_vote = 0;
    int boolval = TRUE, master = FALSE;
    std::string vote_tokens;
    std::string IP_buffer, IP_temp, IP_broadcast;
    int puerto = PORT;
    std::ofstream fileOut;

    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        exit(1);
    }

    if (argc == 2)
        puerto = std::atoi(argv[1]);

    #if OPCION_IP == 0
        IP_buffer = "10.0.0.22";
        IP_broadcast = "10.0.0.255";
    #endif

    #if (OPCION_IP == 1) || (OPCION_IP == 2)
        if (getIP(IP_buffer) == -1)
            error("Error getting IP address");

        std::cout << "IP address: " << IP_buffer << std::endl;

        fileOut.open("ipaddr");
        if (!fileOut.is_open()) {
            error("Error opening file for writing");
        }

        fileOut << IP_buffer;
        fileOut.close();

        IP_broadcast = IP_buffer + "255";
    #endif

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_buffer.c_str());
    addr.sin_port = htons(puerto);

    bzero((char *)&broadcast_addr, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr(IP_broadcast.c_str());
    broadcast_addr.sin_port = htons(puerto);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &boolval, sizeof(int));
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(int));

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        error("ERROR on binding");

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    srand(time(NULL));

    myvote = rand() % VOTO_MAX + 1;
    myIP = inet_addr(IP_buffer.c_str());
    incomingIP = 0;
    in_vote = 0;
    std::strcpy(messg.data(), "Hello");
    vote_tokens = strtok(messg.data(), " ");

    while (1) {
        if (sendto(sockfd, messg.data(), strlen(messg.data()), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0)
            error("ERROR in sendto");

        std::memset(messg.data(), 0, MSG_SIZE);
        length = sizeof(addr);
        n = recvfrom(sockfd, messg.data(), MSG_SIZE, 0, (struct sockaddr *)&addr, &length);

        if (n > 0) {
            std::strcpy(messg_temp.data(), messg.data());
            vote_tokens = strtok(messg_temp.data(), " ");
            incomingIP = inet_addr(vote_tokens.c_str());
            vote_tokens = strtok(NULL, " ");
            in_vote = std::atoi(vote_tokens.c_str());

            if ((incomingIP > myIP) && (in_vote > myvote)) {
                myvote = in_vote;
                myIP = incomingIP;
                master = FALSE;
            } else if ((incomingIP == myIP) && (in_vote == myvote)) {
                master = TRUE;
            }
        } else {
            master = TRUE;
        }

        if (master == TRUE) {
            std::cout << "Master: " << IP_buffer << " " << myvote << std::endl;
        } else {
            std::cout << "Slave: " << IP_buffer << " " << myvote << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    close(sockfd);

    return 0;
}

int getIP(std::string& IP_buffer) {
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;

    #if OPCION_IP == 1
        strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);
    #else
        strncpy(ifr.ifr_name, "enp0s31f6", IFNAMSIZ - 1);
    #endif

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    IP_buffer = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

    return 0;
}
