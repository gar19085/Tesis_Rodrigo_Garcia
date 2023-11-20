#include <Wire.h> // Incluye la librería Wire (I2C)

const int BUFFER_SIZE = 64; // Define el tamaño del buffer
char buffer[BUFFER_SIZE]; // Define el buffer
int bufferIndex = 0; // Define el índice del buffer

void receiveEvent(int howMany) { // Función para recibir datos
  while (Wire.available()) { // Mientras haya datos disponibles
    char receivedChar = Wire.read(); // Lee el dato recibido
    

    if (bufferIndex < BUFFER_SIZE - 1) { // Si el índice del buffer es menor al tamaño del buffer
      buffer[bufferIndex++] = receivedChar;
      buffer[bufferIndex] = '\0'; // Agrega el caracter nulo al final del buffer
    }
  }
}

void processReceivedData() { // Función para procesar los datos recibidos
  if (bufferIndex > 0) {
    // Imprime los datos recibidos
    Serial.print("Received data: ");
    Serial.println(buffer); // Imprime el buffer

    // Chequea si el dato recibido es 'A' o 'B'
    if (buffer[0] == 'A') {
      Serial.println("Turning on the built-in LED."); // Imprime el mensaje
      digitalWrite(LED_BUILTIN, HIGH); // Enciende el LED
    } else if (buffer[0] == 'B') {
      Serial.println("Turning off the built-in LED."); // Imprime el mensaje
      digitalWrite(LED_BUILTIN, LOW); // Apaga el LED
    }

    // Limpia el buffer
    bufferIndex = 0;
    buffer[0] = '\0';
  }
}

void setup() { // Función de configuración
  Wire.begin(0x08); // Inicializa el dispositivo como esclavo con la dirección 0x08
  Wire.onReceive(receiveEvent); // Registra la función para recibir datos
  Serial.begin(9600); // Inicializa el puerto serial
  
  pinMode(LED_BUILTIN, OUTPUT); // Configura el pin del LED como salida
}

void loop() {
  processReceivedData(); // Procesa los datos recibidos
  delay(100); // Espera 100 milisegundos
}
