/* SPI_Slave.ino
 * Based on code from: https://github.com/linhartr22/Arduino_SPI_Master_Slave_Demo/blob/master/SPI_Slave_Demo/SPI_Slave_Demo.ino
 * Adapted and commented by: Ricardo Girón
 */
 
// Include SPI (Serial Peripheral Interface) library. Does not support SPI Slave.
#include <SPI.h>


//Initialize SPI slave.
void SlaveInit(void) {
  // Initialize SPI pins.
  pinMode(52, INPUT);
  pinMode(51, INPUT);
  pinMode(50, INPUT);
  pinMode(53, INPUT);
  // Enable SPI as slave.
  SPCR = (1 << SPE);
}
// SPI Transfer.
byte SPItransfer(byte value) {
  SPDR = value;
  while(!(SPSR & (1<<SPIF)));
  delay(10);
  return SPDR;
}

int val = 0;
int potD = 0;
// The setup() function runs after reset.
void setup() {

  pinMode(0, INPUT); //Potenciometro
  // Initialize serial for troubleshooting.
  Serial.begin(9600);
  // Initialize SPI Slave.
  SlaveInit();
  Serial.println("Slave Initialized");
}
// The loop function runs continuously after setup().
void loop() {
  val = analogRead(0);
  //Serial.println(String(val*0.0074962518));
  //potD = val*0.0074962518;  
  //Take the MISO pin and set it as an output
  pinMode(MISO, OUTPUT);
  Serial.println("***Slave Enabled.");

  //This byte receives and sends the data via SPI
  byte rx=SPItransfer(val);
  
  Serial.println("rx:" + String(rx)); 
  Serial.println("VOLTAGE:" + String(val)); 
}


