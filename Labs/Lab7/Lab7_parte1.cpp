//============================================================================
// Nombre: Lab7_parte1.cpp
// Autor: Rodrigo José García Ambrosy
//============================================================================

#include <iostream> // Librería estándar de entrada/salida
#include <thread>   // Librería para utilizar hilos
#include <chrono>   // Librería para utilizar la función sleep_for
#include <wiringPi.h> // Librería para controlar pines GPIO

#define LUZ_1  3 // Pin para la luz 1
#define LUZ_2  4 // Pin para la luz 2
#define LUZ_P  5 // Pin para la luz de parada
#define BTN1   16 // Pin para el botón

class ButtonHandler { // Clase para el control del botón
public:
    ButtonHandler() {
        pinMode(BTN1, INPUT);
        pullUpDnControl(BTN1, PUD_DOWN); // Configura pull down para el botón
    }

    bool isButtonPressed() { // Verifica si el botón está presionado
        return digitalRead(BTN1) == HIGH;
    }

    void waitForButtonRelease() {
        while (isButtonPressed()) {
            // Espera a que el botón se suelte
        }
    }
};

// Hilo a tiempo real para el parpadeo de los LEDs con un algoritmo de escalonamiento por Poleo
void polled_scheduling(ButtonHandler &buttonHandler) {
    while (true) {
        digitalWrite(LUZ_1, HIGH);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        digitalWrite(LUZ_1, LOW);

        digitalWrite(LUZ_2, HIGH);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        digitalWrite(LUZ_2, LOW);

        if (buttonHandler.isButtonPressed()) {
            digitalWrite(LUZ_P, HIGH);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            digitalWrite(LUZ_P, LOW);
            buttonHandler.waitForButtonRelease();
        }
    }
}

int main() {
    wiringPiSetup(); // Inicializa la librería wiringPi

    pinMode(LUZ_1, OUTPUT);
    pinMode(LUZ_2, OUTPUT);
    pinMode(LUZ_P, OUTPUT);

    digitalWrite(LUZ_1, LOW);
    digitalWrite(LUZ_2, LOW);
    digitalWrite(LUZ_P, LOW);

    ButtonHandler buttonHandler; // Crea un objeto de la clase ButtonHandler
    std::thread polled_thread(polled_scheduling, std::ref(buttonHandler)); // Crea un hilo para el algoritmo de escalonamiento por Poleo

    polled_thread.join(); 

    return EXIT_SUCCESS;
}
