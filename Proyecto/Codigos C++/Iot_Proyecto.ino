
/*
UNIVERSIDAD DEL VALLE DE GUATEMALA
RODRIGO GARCIA, 19085
PROYECTO FINAL
Arduino IoT
*/ 

#include <ArduinoBLE.h>

long previousMillis = 0;
int interval = 0;
int ledState = LOW;

BLEService ledService("180A"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);

void setup() {
  pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
  Serial.begin(9600);
  while (!Serial);

  // set built in LED pin to output mode
  //pinMode(LED_BUILTIN, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Nano 33 IoT");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        switch (switchCharacteristic.value()) {   // any value other than 0
          case 01:
            Serial.println("LED on");
            digitalWrite(A1, HIGH);            // will turn the LED on
            digitalWrite(A2, HIGH); 
            break;
          default:
            Serial.println(F("LED off"));
            digitalWrite(A1, LOW);          // will turn the LED off
            digitalWrite(A2, LOW); 
            break;
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(A1, LOW);         // will turn the LED off
  }
}