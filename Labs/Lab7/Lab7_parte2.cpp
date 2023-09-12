/*
 ============================================================================
 Nombre: Lab6_files_y_strings.cpp
 Autor:  Rodrigo Jose Garcia Ambrosy
 ============================================================================
 */
#include <iostream>
#include <thread>
#include <chrono>
#include <wiringPi.h>
#include <semaphore>

#define LUZ_1  3
#define LUZ_2  4
#define LUZ_P  5
#define BTN1  16

#define LUZ_1_PRI 10
#define LUZ_2_PRI 10
#define LUZ_P_PRI 10

#define PERIOD 750

class LightController {
public:
    LightController(int pin) : pin_(pin) {
        pinMode(pin_, OUTPUT);
    }

    void turnOn() {
        digitalWrite(pin_, HIGH);
    }

    void turnOff() {
        digitalWrite(pin_, LOW);
    }

private:
    int pin_;
};

class ButtonHandler {
public:
    ButtonHandler(int pin) : pin_(pin) {
        pinMode(pin_, INPUT);
        pullUpDnControl(pin_, PUD_DOWN);
    }

    bool isButtonPressed() {
        return digitalRead(pin_) == HIGH;
    }

    void waitForButtonRelease() {
        while (isButtonPressed()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    int pin_;
};

std::binary_semaphore sem(1);

void luz(LightController &light, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        perror("sched_setscheduler falló");
        exit(20);
    }

    while (true) {
        sem.acquire();
        light.turnOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));
        light.turnOff();
        sem.release();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void peatonal(LightController &light, ButtonHandler &buttonHandler, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        perror("sched_setscheduler falló");
        exit(20);
    }

    while (true) {
        sem.acquire();
        if (buttonHandler.isButtonPressed()) {
            light.turnOn();
            std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));
            light.turnOff();
            buttonHandler.waitForButtonRelease();
        }
        sem.release();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    wiringPiSetupGpio();

    LightController luz1Ctrl(LUZ_1);
    LightController luz2Ctrl(LUZ_2);
    LightController luzPCtrl(LUZ_P);
    ButtonHandler buttonHandler(BTN1);

    std::thread hilo_luz1(luz, std::ref(luz1Ctrl), LUZ_1_PRI);
    std::thread hilo_luz2(luz, std::ref(luz2Ctrl), LUZ_2_PRI);
    std::thread hilo_luzP(peatonal, std::ref(luzPCtrl), std::ref(buttonHandler), LUZ_P_PRI);

    hilo_luz1.join();
    hilo_luz2.join();
    hilo_luzP.join();

    return EXIT_SUCCESS;
}
