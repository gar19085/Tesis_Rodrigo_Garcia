/*
 Nombre: socket_udp.h
 Autor: Rodrigo José García Ambrosy
 Proyecto
*/

//Código para la librería del socket UDP que contiene la classe para utilizar sockets.  
#ifndef SOCKET_UDP_H
#define SOCKET_UDP_H

#include <sys/socket.h> //Librería para utilizar sockets
#include <netinet/in.h> //Librería para utilizar direcciones de internet
#include <QHostAddress> //Clase propia de Qt para utilizar direcciones de internet
#include <QUdpSocket>   //Clase propia de Qt para utilizar sockets UDP
#include <QObject>    //Clase propia de Qt para utilizar objetos
#include <QMutex>   //Clase propia de Qt para utilizar mutex
#include <iostream> //Librería para utilizar entrada y salida de datos
#include <cstring> //Librería para utilizar cadenas de caracteres
#include <unistd.h> //Librería para utilizar funciones de sistema
#include <arpa/inet.h> //Librería para manipular direcciones de internet

class socket_udp : public QObject{ //Classe para utilizar sockets UDP
    Q_OBJECT
public:
    socket_udp(); //Constructor
    ~socket_udp(); //Destructor
 
    bool initializeSocket(int port); //Función para inicializar el socket
    bool bindSocket(); //Función para enlazar el socket
    bool setBroadcastOption(); //Función para establecer la opción de broadcast
    bool setBroadcastIP(); //Función para establecer la dirección de broadcast
    void sendTo(const char *message); //Función para enviar un mensaje
    void receiveAndWrite(size_t bufferSize); //Función para recibir y escribir un mensaje
    void setShouldExit(bool value); //Función para establecer el valor de shouldExit y salir del programa
    void conexion(const char *message); //Función para enviar mensaje que establece la conexión

signals: //Señales para enviar recibir mensajes
    void receivedMessageSignal(const QString& message);

private slots: //Slots privados para procesar mensajes. 
    void processPendingDatagrams();

private: //Atributos privados de la clase
    QMutex mutex;
    bool shouldExit;
    QUdpSocket *udpSocket;
    int sockfd;
    struct sockaddr_in socketAddress;


};



#endif // SOCKET_UDP_H
