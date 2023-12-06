/*
 Nombre: mainwindow.h
 Autor: Rodrigo José García Ambrosy
 Proyecto
*/
//Este código es el header que se encarga de establecer las funciones de la interfaz gráfica y crear la clase del mismo. 

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> //Clase para el uso de la interfaz gráfica
#include "socket_udp.h" //Librería para los sockets

QT_BEGIN_NAMESPACE //Se inicia el namespace
namespace Ui { class MainWindow; } 
QT_END_NAMESPACE //Se termina el namespace

class MainWindow : public QMainWindow //Se crea la clase de la interfaz gráfica
{
    Q_OBJECT

public: //Se crea la función pública
    MainWindow(socket_udp *udpSocket, QWidget *parent = nullptr);
    ~MainWindow();

private slots: //Se crean las funciones privadas
    void updateTextEdit(const QString &data); //Función para actualizar el texto de la interfaz gráfica
    void on_Exit_clicked(); //Función para salir del programa
    //Funciones para encender y apagar los LED de las UTR desde la interfaz gráfica
    void on_RTU1L11_clicked();
    void on_RTU1L10_clicked();
    void on_RTU1L21_clicked();
    void on_RTU1L20_clicked();
    void on_RTU1LI1_clicked();
    void on_RTU1LI0_clicked();
    void on_RTU2L11_clicked();
    void on_RTU2L10_clicked();
    void on_RTU2L21_clicked();
    void on_RTU2L20_clicked();
    void on_RTU2LI1_clicked();
    void on_pushButton_7_clicked();

private: //Atributos privados de la clase
    Ui::MainWindow *ui;
    bool shouldExit;
    socket_udp *udpSocketInstance;
};
#endif // MAINWINDOW_H
