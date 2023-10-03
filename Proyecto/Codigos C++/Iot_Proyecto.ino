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

BLEService servicioLED("180A"); // Servicio BLE para el LED

// Característica de Interruptor LED BLE - UUID personalizado de 128 bits, lectura y escritura por parte del central
BLEByteCharacteristic caracteristicaInterruptor("2A57", BLERead | BLEWrite);

void setup() {
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  Serial.begin(9600);
  while (!Serial);

  // Comienza la inicialización
  if (!BLE.begin()) {
    Serial.println("¡Inicio de BLE fallido!");

    while (1);
  }

  // Establece el nombre local anunciado y el UUID del servicio:
  BLE.setLocalName("Nano 33 IoT");
  BLE.setAdvertisedService(servicioLED);

  // Agrega la característica al servicio
  servicioLED.addCharacteristic(caracteristicaInterruptor);

  // Agrega el servicio
  BLE.addService(servicioLED);

  // Establece el valor inicial para la característica:
  caracteristicaInterruptor.writeValue(0);

  // Comienza a anunciarse
  BLE.advertise();

  Serial.println("Periférico LED BLE");
}

void loop() {
  // Escucha por periféricos BLE para conectar:
  BLEDevice central = BLE.central();

  // Si un central se conecta al periférico:
  if (central) {
    Serial.print("Conectado al central: ");
    // imprime la dirección MAC del central:
    Serial.println(central.address());

    // mientras el central sigue conectado al periférico:
    while (central.connected()) {
      // si el dispositivo remoto escribió en la característica,
      // usa el valor para controlar el LED:
      if (caracteristicaInterruptor.written()) {
        switch (caracteristicaInterruptor.value()) {   // cualquier valor que no sea 0
          case 01:
            Serial.println("LED encendido");
            digitalWrite(A1, HIGH);            // enciende el LED
            digitalWrite(A2, HIGH); 
            break;
          default:
            Serial.println(F("LED apagado"));
            digitalWrite(A1, LOW);          // apaga el LED
            digitalWrite(A2, LOW); 
            break;
        }
      }
    }

    // cuando el central se desconecta, imprímelo:
    Serial.print(F("Desconectado del central: "));
    Serial.println(central.address());
    digitalWrite(A1, LOW);         // apaga el LED
  }
}