/*
 ============================================================================
 Nombre: L5_bocina.cpp
 Autor: Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream>     // Librería estándar de entrada/salida
#include <cstdlib>      // Librería estándar para funciones generales
#include <unistd.h>     // Se utiliza para crear nuevos procesos utilizando fork()
#include <wiringPi.h>   // Librería para controlar pines GPIO
#include <thread>       // Librería para utilizar hilos
#include <chrono>       // Librería para manejo de tiempos


#define SPKR 22 // Se define el pin para la bocina
#define BTN1 27 // Se define el pin para el botón



// Función que espera la entrada desde el teclado
void *teclado(void *ptr) {
    char *input = static_cast<char *>(ptr); // Se obtiene el puntero al caracter ingresado

    while (*input != 's') { // Mientras no se ingrese la tecla 's'
        std::cin >> *input; // Se lee la entrada del teclado
    }
}

int main() { // Función principal
    char opcion = 'r'; 
    int boton = LOW; // Estado del botón
    std::thread teclado_thr(teclado, &opcion); // Se crea un hilo para leer la entrada del teclado

    if (wiringPiSetup() == -1) { // Se inicializa la librería WiringPi
        std::cerr << "Error initializing WiringPi." << std::endl;
        return 1;
    }

    pinMode(SPKR, OUTPUT); // Configura el pin de la bocina como salida
    pinMode(BTN1, INPUT);  // Configura el pin del botón como entrada
    pullUpDnControl(BTN1, PUD_DOWN); // Configura el botón como pull-down

    std::cout << "Presione el botón para iniciar el sonido." << std::endl; // Mensaje de inicio
    std::cout.flush();

    while (!boton) {
        boton = digitalRead(BTN1); // Lee el estado del botón
        std::this_thread::sleep_for(std::chrono::microseconds(1000)); // Espera 1 ms
    }

    // Muestra las opciones de control por teclado
    std::cout << "Opciones del teclado:" << std::endl;
    std::cout << "\np - pausar\nr - reanudar\ns - salir del programa\n\n"; 
    std::cout.flush(); // Limpieza de búfer de salida

    while (opcion != 's') { // Mientras no se ingrese la tecla 's'
        if (opcion == 'r') { // Si se ingresa la tecla 'r'
            std::this_thread::sleep_for(std::chrono::microseconds(1000)); // Espera 1 ms
            digitalWrite(SPKR, HIGH); // Enciende la bocina
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            digitalWrite(SPKR, LOW); // Apaga la bocina
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    }
    // Espera a que termine el hilo de lectura del teclado
    teclado_thr.join();
    std::cout << "Saliendo del programa..." << std::endl << std::endl;

    return 0; // Termina el programa
}
