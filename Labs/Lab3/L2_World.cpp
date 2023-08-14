#include <iostream>  // Inclusión de cabecera para entrada/salida en C++
#include <thread>  // Inclusión de cabecera para usar usleep (opcional en C++)
int main()
{
    while (1)
    {
        std::cout << "World ";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }   
}