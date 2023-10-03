/*
 ============================================================================
 Nombre: L3_fork_contexto.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream> //Librería estándar de entrada/salida
#include <thread>   //Librería para utilizar hilos
#include <chrono>   //Librería para manejo de tiempos
#include <unistd.h> //Se utiliza para crear nuevos procesos utilizando fork()

//Variable global que funciona como contador
int counter = 0;

void child(){ //Función del proceso hijo
    std::cout << "Child initial count: " << counter << std::endl;
    counter = 200;

    while (1)
    {
        counter++;
        std::cout << "Child counter = " << counter << std::endl; //Contador del proceso hijo
        std::cout.flush(); // Limpieza de búfer de salida
        std::this_thread::sleep_for(std::chrono::microseconds(1100000)); 
    }    
}

int main()
{
    int a;
    counter = 100;

    std::cout << "Parent initial count: " << counter << std::endl;

    //Se crea un nuevo proceso hijo
    if ((a = fork()) < 0)
    {
        std::cout << "fork error" << std::endl;
        exit(-1);
    }

    if (a == 0) // Proceso hijo
        child(); // Se inicia la ejecución de la función del proceso hijo

    //Proceso padre
    while (true)
    {
        counter++;
        std::cout << "Parent counter = " << counter << std::endl; //Contador del proceso padre
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }

    return 0;
}