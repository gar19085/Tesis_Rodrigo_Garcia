#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>

void child(){
    while (1){
        std::cout << "Hello ";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1100000));
    }
}

int main(){
    int a;

    if ((a = fork()) < 0){
        std::cout << "fork error \n";
        exit(-1); 
    }

    if (a == 0){
        child();
    }
    
    while (1){
        std::cout << "World\n";
        std::cout.flush();
        std::chrono::microseconds(1000000);
    }

    return 0;
}