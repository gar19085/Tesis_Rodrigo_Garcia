/*
 ============================================================================
 Nombre: L3_World.cpp
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
        std::cout << "World "; //Imprime la palabra "World" en la consola
        std::cout.flush();//Limpieza de buffers asociados al estándar output
        std::this_thread::sleep_for(std::chrono::milliseconds(1000000)); //Pausa por 1 segundo
    }   
}