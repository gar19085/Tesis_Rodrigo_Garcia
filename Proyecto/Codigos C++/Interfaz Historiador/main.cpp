/*
 ============================================================================
 Nombre: main.cpp
 Autor: Rodrigo José García Ambrosy
 Proyecto
 ============================================================================
 */
//Este código es el principal para el funcionamiento general del proyecto creado en Qt Creator

#include "mainwindow.h" //Incluye las funciones de la interfaza gráfica
#include "ui_mainwindow.h" //Incluye la interfaz gráfica
#include <QThread>  //Clase para utilizar hilos especifica de Qt
#include "socket_udp.h" //Incluye las funciones del socket

#define MSG_SIZE 3000 // Tamaño del mensaje
#define IP "192.168.1.255" // Dirección IP
#define OPCION_IP 0 /// 0 - hard coded
#define IP_LENGTH 15 // Tamaño de la dirección IP
int sockfd, n; // Descriptores de archivo
socket_udp udpSocket; //Instancia del socket
int port = 2044; //Puerto de comunicación
bool shouldExit = false; //Variable para salir del programa

void receiving(socket_udp *udpSocket) { // Función para recibir mensajes
    udpSocket->receiveAndWrite(MSG_SIZE); // Recibe el mensaje y se llama a la clase de recivir y escribir del socket
}

int main(int argc, char *argv[]) { //Función principal
    QApplication a(argc, argv); //Instancia de la interfaz gráfica
    QHostAddress address("192.168.1.255"); //Dirección IP
    // Inicializa el socket y lo configura
    udpSocket.initializeSocket(port); 
    udpSocket.bindSocket(); //Se venicula el socket
    udpSocket.setBroadcastOption(); //Se configura el socket para utilizar broadcast
    udpSocket.setBroadcastIP(); //Se configura la dirección IP
    udpSocket.conexion("Conexion"); //Se envía el mensaje de conexión a las UTR
    
    QThread receiveThread; //Se crea un hilo para recibir mensajes
    udpSocket.moveToThread(&receiveThread); //Se mueve el socket al hilo creado
    QObject::connect(&receiveThread, &QThread::started, [&]() { //Se conecta el hilo con la función de recibir mensajes
        receiving(&udpSocket); //Se llama a la función de recibir mensajes
        }
    );

    receiveThread.start(); //Se inicia el hilo
    MainWindow w(&udpSocket); //Se crea la interfaz gráfica y se vincula con los sockets
    w.show();  //Se muestra la interfaz gráfica

    // Inicia el bucle de eventos
    int result = a.exec(); //Se ejecuta la interfaz gráfica

    return result; //Se retorna el resultado
}

