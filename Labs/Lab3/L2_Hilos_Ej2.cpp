/*
 ============================================================================
 Nombre: L3_Hilos_Ej2.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream> //Librería estándar de entrada/salida
#include <thread>   //Librería para utilizar hilos
#include <chrono>   //Librería para manejo de tiempos

void My_Thread(void *ptr)//Función que se ejecutara por el hilo
{
    char *message;
    message = static_cast<char *>(ptr); //Se convierte el puntero void* a char*

    while (true)
    {
        std::cout << message; //Se imprime el mensaje
        std::cout.flush();    //Limpieza de búfer de salida
        std::this_thread::sleep_for(std::chrono::microseconds(1100000)); //Configuración para pausar por 1.1 segundos
    }
}

int main()
{
    std::thread thread2; //Se declara una variable para el hilo
    char *message1 = const_cast<char *>("Hello "); //Puntero a "Hello "
    char *message2 = "World\n"; //Puntero a "World"

    thread2 = std::thread(My_Thread, static_cast<void *>(message1)); //Se crea el hilo y se pasa el mensaje "Hello "

    while (true)
    {
        std::cout << message2; //Se imprime el mensaje "World"
        std::cout.flush();     //Limpieza de búfer de salida
        std::this_thread::sleep_for(std::chrono::microseconds(1000000)); //Pausa de 1 segundo
    }

    return 0; //Indicador para salidas exitosas
}