/*
 ============================================================================
 Nombre: L3_pthread_contexto.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream> //Librería estándar de entrada/salida
#include <thread>   //Librería para utilizar hilos
#include <chrono>   //Librería para manejo de tiempos
#include <unistd.h> //Se utiliza para crear nuevos procesos utilizando fork()

int counter = 0; //Contador global

void My_thread(){ //Función del hilo secundario
    std::cout << "My_thread initial count: " << counter << std::endl;
    counter = 200;
    
    while (true)
    {
        counter++;
        std::cout << "My_thread: " << counter << std::endl;
        std::cout.flush(); // Limpieza de búfer de salida
        std::this_thread::sleep_for(std::chrono::microseconds(1100000));
    }
}

int main(){
    std::thread thread2(My_thread); //Creación de un hilo secundario

    counter = 100;
    std::cout << "ParentThread initial count: " << counter << std::endl; 

    //Proceso del hilo principal
    while (true) 
    {
        counter++;
        std::cout << "ParentThread: " << counter << std::endl; //Contador del hilo principal
        std::cout.flush(); // Limpieza de búfer de salida
        std::this_thread::sleep_for(std::chrono::microseconds(1000000)); //Espera de 1 segundo
    }
         
}