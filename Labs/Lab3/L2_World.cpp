#include <iostream> 
#include <thread>  
#include <chrono>
int main()
{
    while (1)
    {
        std::cout << "World ";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }   
}