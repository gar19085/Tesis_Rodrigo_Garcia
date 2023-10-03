#include <wiringPi.h>

#define LED_PIN 22
#define BUTTON_PIN 27

int main() {
    wiringPiSetup();
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    pullUpDnControl(BUTTON_PIN, PUD_DOWN);

    while (true) {
        if (digitalRead(BUTTON_PIN) == HIGH) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }
    }

    return 0;
}