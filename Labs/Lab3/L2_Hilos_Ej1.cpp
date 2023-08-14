#include <iostream>
#include <thread>  // Inclusión de cabecera para utilizar std::thread
#include <chrono>  // Inclusión de cabecera para utilizar std::this_thread::sleep_for


// Código a ejecutar por el segundo hilo
void My_Thread()
{
    std::cout << "No soy el primer hilo." << std::endl;
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

// Función principal (primer hilo de ejecución)
int main()
{
    std::thread thread2;  // Variable para identificar el 2do hilo que se creará.

    // La siguiente función crea un hilo usando std::thread.
    thread2 = std::thread(My_Thread);

    std::cout << "Soy el primer hilo." << std::endl;
    std::cout.flush();
    // La función join espera que el hilo indicado termine (bloquea).
    thread2.join();

    std::cout << "Después de que el 2do hilo haya terminado." << std::endl;

    return 0;
}
