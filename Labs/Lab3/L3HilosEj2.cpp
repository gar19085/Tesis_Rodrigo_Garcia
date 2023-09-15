/*
 ============================================================================
 Nombre: L3HilosEj2.cpp
 Autor:  Rodrigo José García Ambrosy
 Este código se utilizo para obtener el uso de CPU y Memoria del programa
 ============================================================================
 */

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <iomanip>
#include <sys/resource.h>

void My_Thread(void *ptr)
{
    char *message;
    message = static_cast<char *>(ptr);

    while (true)
    {
        std::cout << message;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1100000));

        // Obtener el PID del programa actual
        pid_t pid = getpid();

        // Obtener el consumo de CPU y Memoria del programa
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);

        // Información de CPU
        double cpu_usage = (usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0) * 100.0;

        // Información de memoria (en kilobytes)
        long mem_usage = usage.ru_maxrss;

        std::cout << "Uso de CPU del programa: " << std::fixed << std::setprecision(3) << cpu_usage << " %\n";
        std::cout << "Uso de Memoria del programa: " << mem_usage << " KB\n";
    }
}

int main()
{
    std::thread thread2;
    const char *message1 = "Hello ";
    const char *message2 = "World\n";

    thread2 = std::thread(My_Thread, const_cast<char *>(message1));

    while (true)
    {
        std::cout << message2;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }

    return 0;
}
