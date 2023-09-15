/*
 ============================================================================
 Nombre: Lab7_parte1.cpp
 Autor:  Rodrigo Jose Garcia Ambrosy
 ============================================================================
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <wiringPi.h>

#define LUZ_1  3
#define LUZ_2  4
#define LUZ_P  5
#define BTN1   16

class ButtonHandler { //Se crea una clase para el control del botón
public:
    ButtonHandler() {
        pinMode(BTN1, INPUT);
        pullUpDnControl(BTN1, PUD_DOWN);
    }

    bool isButtonPressed() {
        return digitalRead(BTN1) == HIGH; // Devuelve true si el botón está presionado
    }

    void waitForButtonRelease() {
        while (isButtonPressed()) {
            // Se espera a que el botón se suelte
        }
    }
};

// Hilo a tiempo real para un blink de los LEDs con un algoritmo de escalonamiento por Poleo
void polled_scheduling(ButtonHandler &buttonHandler)
{
    while (true)
    {
        digitalWrite(LUZ_1, HIGH);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        digitalWrite(LUZ_1, LOW);

        digitalWrite(LUZ_2, HIGH);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        digitalWrite(LUZ_2, LOW);

        if (buttonHandler.isButtonPressed())
        {
            digitalWrite(LUZ_P, HIGH);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            digitalWrite(LUZ_P, LOW);
            buttonHandler.waitForButtonRelease();
        }
    }
}

int main()
{
    wiringPiSetupGpio();

    pinMode(LUZ_1, OUTPUT);
    pinMode(LUZ_2, OUTPUT);
    pinMode(LUZ_P, OUTPUT);

    digitalWrite(LUZ_1, LOW);
    digitalWrite(LUZ_2, LOW);
    digitalWrite(LUZ_P, LOW);

    ButtonHandler buttonHandler;

    std::thread polled_thread(polled_scheduling, std::ref(buttonHandler));

    polled_thread.join();

    return EXIT_SUCCESS;
}
