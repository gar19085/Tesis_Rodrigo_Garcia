#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>

#define SPKR 22
#define BTN1 27

void *teclado(void *ptr) {
    char *input = static_cast<char *>(ptr);

    while (*input != 's') {
        std::cin >> *input;
    }
    
}

int main() {
    char opcion = 'r';
    int boton = LOW;
    std::thread teclado_thr(teclado);

    if (wiringPiSetup() == -1) {
        std::cerr << "Error initializing WiringPi." << std::endl;
        return 1;
    }

    pinMode(SPKR, OUTPUT);
    pinMode(BTN1, INPUT);
    pullUpDnControl(BTN1, PUD_DOWN);

    std::cout << "Presione el botón para iniciar el sonido." << std::endl;
    std::cout.flush();


    while (!boton) {
        boton = digitalRead(BTN1);
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }

    std::cout << "Opciones del teclado:" << std::endl;
    std::cout << "\np - pausar\nr - reanudar\ns - salir del programa\n\n";
    std::cout.flush();

    teclado_thr std::thread(teclado, static_cast<void *>(ptr));

    //pthread_create(&teclado_thr, nullptr, teclado, &opcion);

    while (opcion != 's') {
        if (opcion == 'r') {
            delay(1);
            digitalWrite(SPKR, HIGH);
            delay(1);
            digitalWrite(SPKR, LOW);
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    }

    //pthread_join(teclado_thr, nullptr);
    teclado_thr.join();
    std::cout << "Saliendo del programa..." << std::endl << std::endl;

    return 0;
}
