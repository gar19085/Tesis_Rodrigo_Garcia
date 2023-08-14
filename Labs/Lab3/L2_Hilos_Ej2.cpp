#include <iostream>
#include <thread>
#include <chrono>

void My_Thread(void *ptr)
{
    char *message;
    message = static_cast<char *>(ptr);

    while (true)
    {
        std::cout << message;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1100000));
    }
}

int main()
{
    std::thread thread2;
    char *message1 = const_cast<char *>("Hello ");
    char *message2 = "World\n";

    thread2 = std::thread(My_Thread, static_cast<void *>(message1));

    while (true)
    {
        std::cout << message2;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }

    return 0;
}