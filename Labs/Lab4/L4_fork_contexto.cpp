#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>


int counter = 0;

void child(){
    std::cout << "Child initial count: " << counter << std::endl;
    counter = 200;

    while (1)
    {
        counter++;
        std::cout << "Child counter = " << counter << std::endl;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1100000)); 
    }    
}

int main()
{
    int a;
    counter = 100;

    std::cout << "Parent initial count: " << counter << std::endl;

    if ((a = fork()) < 0)
    {
        std::cout << "fork error" << std::endl;
        exit(-1);
    }

    if (a == 0) // child process
        child();

    while (1)
    {
        counter++;
        std::cout << "Parent counter = " << counter << std::endl;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }

    return 0;
}