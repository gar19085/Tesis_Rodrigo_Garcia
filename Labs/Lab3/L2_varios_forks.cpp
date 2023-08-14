#include <iostream>
#include <unistd.h>
#include <chrono>

int main(){
    int a;

    a = fork();
    a = fork();
    a = fork();

    std::cout << "Mensaje, valor =" << a << std::end1;
    std::cout.flush();

    std::chrono::seconds(10);
    return 0;       
}