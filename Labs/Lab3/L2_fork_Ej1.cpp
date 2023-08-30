/*
 ============================================================================
 Nombre: L3_fork_Ej1.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h> //Se utiliza para crear nuevos procesos utilizando fork()

void child(){ //Función que se ejecuta en el proceso hijo
    while (1){
        std::cout << "Hello ";
        std::cout.flush();//Limpieza de buffers asociados al standart output
        std::this_thread::sleep_for(std::chrono::microseconds(1100000)); //Pausa por 1.1 segundos
    }
}

int main(){
    int a;

    if ((a = fork()) < 0){ //Se crea un nuevo proceso hijo
        std::cout << "fork error \n";
        exit(-1); 
    }

    if (a == 0){
        child();
    }
    
    while (1){ //Codigo del proceso padre
        std::cout << "World\n";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000)); //Pausa por 1 segundo
    }

    return 0;
}