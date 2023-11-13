/*
 ============================================================================
 Nombre: Lab7_parte2.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */
#include <iostream> // Librería estándar de entrada/salida
#include <thread>   // Librería para utilizar hilos
#include <chrono>   // Librería para utilizar la función sleep_for
#include <wiringPi.h> // Librería para controlar pines GPIO
#include <semaphore.h>   // Librería para utilizar semáforos

#define LUZ_1  3 // Pin para la luz 1
#define LUZ_2  4 // Pin para la luz 2
#define LUZ_P  5 // Pin para la luz de parada
#define BTN1  16 // Pin para el botón

#define LUZ_1_PRI 10 // Prioridad de la luz 1
#define LUZ_2_PRI 10 // Prioridad de la luz 2
#define LUZ_P_PRI 10 // Prioridad de la luz de parada

#define PERIOD 750 // Periodo de las luces

sem_t sem; // Declaración del semáforo

class LightController { // Clase para el control de las luces
public:
    LightController(int pin) : pin_(pin) { // Constructor
        pinMode(pin_, OUTPUT);
    }

    void turnOn() { // Enciende la luz
        digitalWrite(pin_, HIGH);
    }

    void turnOff() { // Apaga la luz
        digitalWrite(pin_, LOW);
    }

private:  // Atributos
    int pin_; 
};

class ButtonHandler { // Clase para el control del botón
public:
    ButtonHandler(int pin) : pin_(pin) { 
        pinMode(pin_, INPUT); // Configura el pin como entrada
        pullUpDnControl(pin_, PUD_DOWN); // Configura pull down para el botón
    }

    bool isButtonPressed() { // Verifica si el botón está presionado
        return digitalRead(pin_) == HIGH;
    }

    void waitForButtonRelease() { // Espera a que el botón se suelte
        while (isButtonPressed()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    int pin_;
};

//std::binary_semaphore sem(1); // Declaración del semáforo

void luz(LightController &light, int priority) { // Función para el control de las luces
    struct sched_param param; //Se define una estructura para la planificación
    param.sched_priority = priority; //Se asigna la prioridad y la política de escalonamiento
    if (sched_setscheduler(0, SCHED_RR, &param) == -1) { //Se asigna la prioridad y la política de escalonamiento
        perror("sched_setscheduler falló");
        exit(20);
    }

    while (true) { 
        sem_wait(&sem); //Se espera a que el semáforo esté disponible
        light.turnOn(); //Se enciende la luz
        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));
        light.turnOff(); //Se apaga la luz
        sem_post(&sem);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void peatonal(LightController &light, ButtonHandler &buttonHandler, int priority) { // Función para el control de la luz peatonal
    struct sched_param param;
    param.sched_priority = priority;
    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        perror("sched_setscheduler falló");
        exit(20);
    }

    while (true) {
        sem_wait(&sem);
        if (buttonHandler.isButtonPressed()) { //Se verifica si el botón está presionado
            light.turnOn();
            std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD)); 
            light.turnOff();
            buttonHandler.waitForButtonRelease();
        }
        sem_post(&sem);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() { // Función principal
    sem_init(&sem, 0, 1);
    wiringPiSetupGpio();

    LightController luz1Ctrl(LUZ_1); //Se crea un objeto de la clase LightController
    LightController luz2Ctrl(LUZ_2); //Se crea un objeto de la clase LightController
    LightController luzPCtrl(LUZ_P); //Se crea un objeto de la clase LightController
    ButtonHandler buttonHandler(BTN1); //Se crea un objeto de la clase ButtonHandler

    std::thread hilo_luz1(luz, std::ref(luz1Ctrl), LUZ_1_PRI); //Se crea un hilo para el control de la luz 1
    std::thread hilo_luz2(luz, std::ref(luz2Ctrl), LUZ_2_PRI); //Se crea un hilo para el control de la luz 2
    std::thread hilo_luzP(peatonal, std::ref(luzPCtrl), std::ref(buttonHandler), LUZ_P_PRI); //Se crea un hilo para el control de la luz peatonal
    

    hilo_luz1.join();
    hilo_luz2.join();
    hilo_luzP.join();

    return EXIT_SUCCESS;
}
