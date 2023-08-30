/*
 ============================================================================
 Nombre: L5_bocina.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream>
#include <cstdlib> //Librearía estándar para funciones generales (Se incluye para utilizar la función rand)
#include <unistd.h>
#include <wiringPi.h> //Librería para controlar pines GPIO
#include <thread>
#include <chrono>

#define SPKR 22 //Se define el pin 22 para la bocina
#define BTN1 27 //Se define el pin 27 para el boton

//Función que espera la entrada desde el teclado
void *teclado(void *ptr) {
    char *input = static_cast<char *>(ptr);

    while (*input != 's') {
        std::cin >> *input;
    }
    
}

int main() {
    char opcion = 'r';
    int boton = LOW; //Estado inicial del boton
    std::thread teclado_thr(teclado); //Hilo para leer la entrada del teclado

    if (wiringPiSetup() == -1) {
        std::cerr << "Error initializing WiringPi." << std::endl;
        return 1;
    }

    pinMode(SPKR, OUTPUT); //Configura el pin de bocina como salida
    pinMode(BTN1, INPUT); //Configura el pin del boton como entrada
    pullUpDnControl(BTN1, PUD_DOWN); //Configura el boton como pull-down

    std::cout << "Presione el botón para iniciar el sonido." << std::endl;
    std::cout.flush();


    while (!boton) {
        boton = digitalRead(BTN1); //Lee el estado del boton
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
    //Muestra las opciones de control por teclado
    std::cout << "Opciones del teclado:" << std::endl;
    std::cout << "\np - pausar\nr - reanudar\ns - salir del programa\n\n";
    std::cout.flush();

    //Crea un hilo para leer la entrada del teclado
    teclado_thr std::thread(teclado, static_cast<void *>(ptr));


    while (opcion != 's') {
        if (opcion == 'r') {
            delay(1);
            digitalWrite(SPKR, HIGH); //Enciende la bocina
            delay(1);
            digitalWrite(SPKR, LOW); //Apaga la bocina
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    }
    //Espera a que termine el hilo de lectura del teclado
    teclado_thr.join();
    std::cout << "Saliendo del programa..." << std::endl << std::endl;

    return 0;
}
