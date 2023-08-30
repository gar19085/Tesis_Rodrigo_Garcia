#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>


int main(){
    int a;

    a = fork();
    a = fork();
    a = fork();

    std::cout << "Mensaje, valor =" << a << std::endl;
    std::cout.flush();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;       
}