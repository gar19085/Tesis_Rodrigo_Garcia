/*
 ============================================================================
 Nombre: L3_fork_contexto.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

//Variable global que funciona como contador
int counter = 0;

void child(){ //Funcion del proceso hijo
    std::cout << "Child initial count: " << counter << std::endl;
    counter = 200;

    while (1)
    {
        counter++;
        std::cout << "Child counter = " << counter << std::endl; //Contador del proceso hijo
        std::cout.flush();
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

    if (a == 0) // child process
        child();

    //Proceso padre
    while (1)
    {
        counter++;
        std::cout << "Parent counter = " << counter << std::endl; //Contador del proceso padre
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }

    return 0;
}