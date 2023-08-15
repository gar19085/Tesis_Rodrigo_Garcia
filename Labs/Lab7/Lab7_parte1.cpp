#include <iostream>
#include <thread>
#include <unistd.h>
#include <wiringPi.h>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <ctime>
//Especificos de linux
#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <sched.h>
#include <sys/timerfd.h>
#include <sys/mman.h>

#define LUZ_1  3
#define LUZ_2  4
#define LUZ_P  5
#define BTN1  16

void check_create(int, int);
void check_join(int, int);

void *polled_scheduling(void *ptr){
    while (1)
    {
        // Turn LED 1 on, wait 1 second and turn off
		digitalWrite(LUZ_1, HIGH);
		sleep(1);
		digitalWrite(LUZ_1, LOW);

		// Turn LED 2 on, wait 1 second and turn off
		digitalWrite(LUZ_2, HIGH);
		sleep(1);
		digitalWrite(LUZ_2, LOW);

		// Turn pedestrian LED on if button is pressed then turn off if button isn't pressed go back to LED 1
		if(check_button())
		{
			digitalWrite(LUZ_P, HIGH);
			sleep(1);
			digitalWrite(LUZ_P, LOW);
			clear_button();
		}  
    }
    
}

int main(){
    std::thread polled_thrd(polled_scheduling);

    if (wiringPiSetup() == -1) {
        std::cerr << "Error initializing WiringPi." << std::endl;
        return 1;
    }

    // Set GPIO pins for LEDs and Buttons (Input/Output)
	pinMode(LUZ_1, OUTPUT);	// GPIO3
	pinMode(LUZ_2, OUTPUT);	// GPIO4
	pinMode(LUZ_P, OUTPUT);	// GPIO5
	pinMode(BTN1, INPUT);	// GPIO16

	// Set buttons for reading
	pullUpDnControl(BTN1, PUD_DOWN);

	// Clear LEDs on Pins 2-5
	digitalWrite(LUZ_1, LOW);
	digitalWrite(LUZ_2, LOW);
	digitalWrite(LUZ_P, LOW);

    check_create(polled_thrd std::thread(polled_scheduling), 1); 
    check_join(polled_thrd.join(), 1);

}



void check_create(int thd, int thd_num){
    if (tddh !=0)
    {
        std::cout << "Creating a thread " << thd_num << "failed." << endl;
		if(thd == EAGAIN)
		{
			std::cout << "Insufficient resources to create another thread." <<endl;
		} else if(thd == EINVAL)
		{
			std::cout << "Invalid settings in attr." << endl;
		} else if(thd == EPERM)
		{
			std::cout << "No permission to set the scheduling policy and parameters specified in attr." << endl;
		}
		exit(0);
    }
    
}

void check_join(int thd,  int thd_num)
{
	if(thd != 0)
	{
		std::cout << "Joining thread " << thd_num << "failed." << endl;
		exit(0);
	}
}