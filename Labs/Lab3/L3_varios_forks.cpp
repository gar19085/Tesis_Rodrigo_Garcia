/*
 ============================================================================
 Nombre: L3_varios_forks.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream> //Librería estándar de entrada/salida
#include <thread>   //Librería para utilizar hilos
#include <chrono>   //Librería para manejo de tiempos
#include <unistd.h> //Se utiliza para crear nuevos procesos utilizando fork()

int main(){
    int a;

    a = fork(); //Se crea el primer proceso hijo
    a = fork(); //Se crean dos procesos hijos más
    a = fork();

    //Todos los procesos ejecutan esta línea de código
    std::cout << "Mensaje, valor =" << a << std::endl; 
    std::cout.flush();

    //Pausa de 10 segundos antes de que terminen los procesos
    std::this_thread::sleep_for(std::chrono::seconds(10)); 
    return 0;       
}