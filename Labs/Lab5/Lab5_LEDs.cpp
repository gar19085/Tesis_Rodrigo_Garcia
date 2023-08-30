/*
 ============================================================================
 Nombre: L5_LEDs.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream>
#include <cstdlib> //Librearía estándar para funciones generales (Se incluye para utilizar la función rand)
#include <ctime> //Librería para acceder y manipular el tiempo
#include <chrono>
#include <thread>
#include <wiringPi.h> //Librería para controlar pines GPIO

//Definición de pines para los LEDs
#define LED1  8 //Se define el pin 8 para el LED 1
#define LED2  9 //Se define el pin 9 para el LED 2
#define MAX_RANDOM 1000000 //Valor máximo para la generación de números random

int main() {
    std::rand(std::time(0)); // Semilla de la generación de números aleatorios basada en el tiempo actual

    if (wiringPiSetup() == -1) { //Verificar si se inicializo WiringPi correctamente
        std::cerr << "Error initializing WiringPi." << std::endl;
        return 1;
    }
    //Configuracion de pines como salida
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    //Se apagan los LEDs al inicio
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);

    while (1) {
        std::this_thread::sleep_for(std::chrono::microseconds(500000)); //Pausa de 0.5 segundos
        std::this_thread::sleep_for(std::chrono::microseconds(std::rand() % MAX_RANDOM)); //Pausa aleatoria
        //Encender LED1 y apagar LED2
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);

        std::this_thread::sleep_for(std::chrono::microseconds(500000));
        std::this_thread::sleep_for(std::chrono::microseconds(std::rand() % MAX_RANDOM));
        //Encender LED2 y apagar LED1
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, HIGH);
    }
    return 0;
}