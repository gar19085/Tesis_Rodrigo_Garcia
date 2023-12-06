/*
 Nombre: mainwindow.cpp
 Autor: Rodrigo José García Ambrosy
 Proyecto
*/
//Código que contiene las funciones para el funcionamiento de la interfaz gráfica y contenido para la librería mainwindow.h
#include "mainwindow.h" //Se incluye el header con las funciones declaradas para crear la librería
#include "ui_mainwindow.h" //Se incluye la interfaz gráfica
#include "socket_udp.h" //Se incluye la librería del socket

MainWindow::MainWindow(socket_udp *udpSocket, QWidget *parent) //Se crea el constructor de la interfaz gráfica
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      udpSocketInstance(udpSocket)

{
    ui->setupUi(this);
    connect(udpSocketInstance, &socket_udp::receivedMessageSignal, this, &MainWindow::updateTextEdit); 
    //Se conecta el socket con la función de actualizar el texto de la interfaz gráfica
}

MainWindow::~MainWindow() //Se crea el destructor de la interfaz gráfica
{
    delete ui; //Se elimina la interfaz gráfica
}

void MainWindow::updateTextEdit(const QString &data) //Se crea la función para actualizar el texto de la interfaz gráfica
{
    ui->textEdit->append(data); //Se actualiza el texto de la interfaz gráfica
}

void MainWindow::on_Exit_clicked() //Se crea la función para salir del programa
{
    udpSocketInstance->setShouldExit(true); //Se cambia el valor de la variable para salir del programa
    close(); //Se cierra el programa
}

//Funciones de los botones para encender y apagar los LED de las UTR desde la interfaz gráfica
void MainWindow::on_RTU1L11_clicked()
{
    udpSocketInstance->sendTo("RTU1 LED1 1");
}

void MainWindow::on_RTU1L10_clicked()
{
    udpSocketInstance->sendTo("RTU1 LED1 0");
}

void MainWindow::on_RTU1L21_clicked()
{
    udpSocketInstance->sendTo("RTU1 LED2 1");
}

void MainWindow::on_RTU1L20_clicked()
{
    udpSocketInstance->sendTo("RTU1 LED2 0");
}

void MainWindow::on_RTU1LI1_clicked()
{
    udpSocketInstance->sendTo("RTU1 LEDIoT 1");
}

void MainWindow::on_RTU1LI0_clicked()
{
    udpSocketInstance->sendTo("RTU1 LEDIoT 0");
}

void MainWindow::on_RTU2L11_clicked()
{
    udpSocketInstance->sendTo("RTU2 LED1 1");
}

void MainWindow::on_RTU2L10_clicked()
{
   udpSocketInstance->sendTo("RTU2 LED1 0");
}

void MainWindow::on_RTU2L21_clicked()
{
   udpSocketInstance->sendTo("RTU2 LED2 1");
}

void MainWindow::on_RTU2L20_clicked()
{
   udpSocketInstance->sendTo("RTU2 LED2 0");
}

void MainWindow::on_RTU2LI1_clicked()
{
   udpSocketInstance->sendTo("RTU2 LEDIoT 1");
}

void MainWindow::on_pushButton_7_clicked()
{
   udpSocketInstance->sendTo("RTU2 LEDIoT 0");
}