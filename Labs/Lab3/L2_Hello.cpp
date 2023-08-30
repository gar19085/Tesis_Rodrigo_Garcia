/*
 ============================================================================
 Nombre: L3_Hello.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream> //Librería estándar de entrada/salida
#include <thread>   //Librería para utilizar hilos
#include <chrono>   //Librería para manejo de tiempos
int main()
{
    while (1)
    {
        std::cout << "Hello ";
        std::cout.flush(); //Limpieza de buffers asociados al standart output
        std::this_thread::sleep_for(std::chrono::microseconds(1100000));
        //Configuración para que ese hilo haga una pausa durante 1.1 segundos
    }   
}

