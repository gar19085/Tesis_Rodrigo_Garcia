#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <wiringPi.h>

#define LED1  8
#define LED2  9
#define MAX_RANDOM 1000000

int main() {
    srand(time(0));

    if (wiringPiSetup() == -1) {
        std::cerr << "Error initializing WiringPi." << std::endl;
        return 1;
    }

    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);

    while (1) {
        usleep(500000);
        usleep(rand() % MAX_RANDOM);
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);

        usleep(500000);
        usleep(rand() % MAX_RANDOM);
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, HIGH);
    }

    return 0;
}