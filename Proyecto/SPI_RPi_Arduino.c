/* SPI_RPi_Arduino.c
 * Basado en ejemplos de: https://projects.drogon.net/raspberry-pi/wiringpi/
 * Modificado y comentado por: Luis Alberto Rivera
 *  
 * Recuerde compilar usando -lwiringPi
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>		// for the integer types
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define SPI_CHANNEL	      0	// 0 ó 1
#define SPI_SPEED 	2000000	// Vel. máx es 3.6 MHz cuando VDD = 5 V

uint16_t get_ADC(void);
//uint8_t get_ADC(void);

int main(void)
{
    uint16_t ADCvalue;
    //uint8_t ADCvalue;
	
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
		printf("wiringPiSPISetup falló\n");
		return -1 ;
	}
		
	while(1){
		ADCvalue = get_ADC();
		printf("Valor ADC: %d\n", ADCvalue);
		fflush(stdout);
		usleep(900);
        //sleep(1);
	}
     
  return 0;   
}

// Salida: entero de 16 bits con el valor de la conversión AD. El ADC del Arduino
//         es de 10 bits, por lo que el valor retornado debe estar entre 0 and 1023.
uint16_t get_ADC(void)
//uint8_t get_ADC(void)
{
	uint8_t spiData0[1], spiData1[1];
	
	// The next function performs a simultaneous write/read transaction over the selected
	// SPI bus. Data that was in the spiData buffer is overwritten by data returned from
	// the SPI bus.
	wiringPiSPIDataRW(SPI_CHANNEL, spiData0, 1);
    usleep(100);
    wiringPiSPIDataRW(SPI_CHANNEL, spiData1, 1);
//	printf("spiData1: %d, spiData0: %d, ", spiData1[0], spiData0[0]);
	return ((spiData1[0] << 8) | spiData0[0]);
    //return spiData[0];
}
