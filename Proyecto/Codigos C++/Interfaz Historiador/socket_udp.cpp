/*
 Nombre: socket_udp.cpp
 Autor: Rodrigo José García Ambrosy
 Proyecto
*/
//Este código contiene las funciones necesarias para el archivo header que contiene la clase. 
#include "socket_udp.h" //Incluye las funciones del socket
#include "mainwindow.h" //Librería de las funciones de la interfaz gráfica

#define IP "192.168.1.255" // Dirección IP


socket_udp::socket_udp() : QObject(), shouldExit(false), udpSocket(new QUdpSocket(this)) { //Constructor de la clase
    shouldExit = false; //Variable para salir del programa
    connect(udpSocket, &QUdpSocket::readyRead, this, &socket_udp::processPendingDatagrams); 
    //Se conecta el socket con la función de procesar mensajes
}

socket_udp::~socket_udp() { //Destructor de la clase
    if (sockfd != -1) {
        close(sockfd);
    }
}

bool socket_udp::initializeSocket(int port) { //Función para inicializar el socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); //Se crea el socket, sin conexión
    if (sockfd < 0) {   //Verifica si hubo error
        std::cerr << "ERROR opening socket" << std::endl;
        return false;
    }
    //Se configura el socket y su estructura
    memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(port); //Se configura el puerto
    return true;
}

bool socket_udp::bindSocket() { //Función para vincular el socket
    if (bind(sockfd, reinterpret_cast<struct sockaddr *>(&socketAddress), sizeof(socketAddress)) < 0) { //Verifica si hubo error
        std::cerr << "ERROR binding socket" << std::endl;
        return false;
    }

    return true;
}

bool socket_udp::setBroadcastOption() { //Función para configurar el socket para utilizar broadcast
    int boolval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0) { //Verifica si hubo error
        std::cerr << "ERROR setting socket options" << std::endl;
        return false;
    }

    return true;
}

bool socket_udp::setBroadcastIP() { //Función para configurar la dirección IP
    socketAddress.sin_addr.s_addr = inet_addr(IP); //Se configura la dirección IP
    return true; 
}

void socket_udp::conexion(const char *message){ //Función para enviar el mensaje de conexión
    char CONEXION[128]; // Variable para almacenar el mensaje
    int n; // Variable para almacenar el número de bytes enviados
    std::strcpy(CONEXION, message); // Copia el mensaje en la variable
    n = sendto(sockfd, CONEXION, strlen(CONEXION), 0, (const struct sockaddr *)&socketAddress, sizeof(socketAddress)); // Envia el mensaje
    if (n < 0){
        std::cerr << "ERROR al enviar" << std::endl; // Verifica si hubo error
    }
    std::cout << "Esto se envió: " << message << std::endl; // Imprime el mensaje enviado
}

void socket_udp::sendTo(const char *message) { //Función para enviar mensajes
    int n; // Variable para almacenar el número de bytes enviados
    n = sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *)&socketAddress, sizeof(socketAddress)); // Envia el mensaje
    if (n < 0) { 
        perror("Error sending message"); // Verifica si hubo error
    } else {
        std::cout << "Esto se envió: " << message << std::endl; // Imprime el mensaje enviado
    }
}

void socket_udp::receiveAndWrite(size_t bufferSize) { //Función para recibir y escribir mensajes
    char buffer[bufferSize]; // Variable para almacenar el mensaje
    while (true) { // Ciclo infinito
        {
            QMutexLocker locker(&mutex);  //Se bloquea el mutex
            if (shouldExit) { //Verifica si debe salir del ciclo
                break;  //Sale del ciclo
            }
        }
    ssize_t bytesRead = recvfrom(sockfd, buffer, bufferSize, 0, nullptr, nullptr); //Recibe el mensaje y lee el número de bytes recibidos
    std::cout << "Esto se recibió: " << buffer << std::endl; // Imprime el mensaje recibido
    if (bytesRead == -1){ //Verifica si hubo error
        std::cerr << "Error reciviendo data" << std::endl;
    } else {
        QString receivedMessage = QString::fromUtf8(buffer, bytesRead); //Convierte el mensaje a QString
        emit receivedMessageSignal(receivedMessage); //Emite la señal de mensaje recibido
    }
    }
}

void socket_udp::processPendingDatagrams() { //Función para procesar mensajes
    while (udpSocket->hasPendingDatagrams()) { //Verifica si hay mensajes pendientes
        QByteArray datagram; // Variable para almacenar el mensaje
        datagram.resize(udpSocket->pendingDatagramSize()); //Se redimensiona la variable
        udpSocket->readDatagram(datagram.data(), datagram.size()); //Se lee el mensaje

        QString receivedMessage = QString::fromUtf8(datagram); //Convierte el mensaje a QString
        emit receivedMessageSignal(receivedMessage); //Emite la señal de mensaje recibido
    }
}

void socket_udp::setShouldExit(bool value) { //Función para salir del programa
    shouldExit = value; //Se asigna el valor
}