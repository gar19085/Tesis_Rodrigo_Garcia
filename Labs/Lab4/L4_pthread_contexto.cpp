/*
 ============================================================================
 Nombre: L3_pthread_contexto.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

int counter = 0; //Contador global

void My_thread(){ //Función del hilo secundario
    std::cout << "My_thread initial count: " << counter << std::endl;
    counter = 200;
    
    while (1)
    {
        counter++;
        std::cout << "My_thread: " << counter << std::endl;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1100000));
    }
}

int main(){
    std::thread thread2(My_thread); //Creación de un hilo secundario

    counter = 100;
    std::cout << "ParentThread initial count: " << counter << std::endl;

    //Proceso del hilo principal
    while (1) 
    {
        counter++;
        std::cout << "ParentThread: " << counter << std::endl;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }
         
}