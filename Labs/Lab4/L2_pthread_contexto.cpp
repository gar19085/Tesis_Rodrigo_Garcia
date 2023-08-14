#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

int counter = 0;

void My_thread(){
    std::cout << "My_thread initial count: " << counter << std::endl;
    counter = 200;
    
    while (1)
    {
        counter++;
        std::cout << "My_thread: " << counter << std::endl;
        std::cout.flush();
        usleep(1100000);
    }
}

int main(){
    std::thread thread2(My_thread);

    counter = 100;
    std::cout << "ParentThread initial count: " << counter << std::endl;

    while (1)
    {
        counter++;
        std::cout << "ParentThread: " << counter << std::endl;
        std::cout.flush();
        usleep(1000000);
    }
         
}