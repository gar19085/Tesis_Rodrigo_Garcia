#include <Wire.h>

const int BUFFER_SIZE = 64;
char buffer[BUFFER_SIZE];
int bufferIndex = 0;

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char receivedChar = Wire.read();
    
    // Check if the buffer is not full
    if (bufferIndex < BUFFER_SIZE - 1) {
      buffer[bufferIndex++] = receivedChar;
      buffer[bufferIndex] = '\0'; // Null-terminate the buffer
    }
  }
}

void processReceivedData() {
  if (bufferIndex > 0) {
    // Process the received data
    Serial.print("Received data: ");
    Serial.println(buffer);

    // Check for specific characters
    if (buffer[0] == 'A') {
      Serial.println("Turning on the built-in LED.");
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (buffer[0] == 'B') {
      Serial.println("Turning off the built-in LED.");
      digitalWrite(LED_BUILTIN, LOW);
    }
    
    // Clear the buffer
    bufferIndex = 0;
    buffer[0] = '\0';
  }
}

void setup() {
  Wire.begin(0x08);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT); // Set the LED pin as output
}

void loop() {
  processReceivedData();
  delay(100); // Add a small delay to avoid excessive looping
}
