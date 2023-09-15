/* adc_SPI_wiringPi.c
 Basado en código tomado de: https://projects.drogon.net/raspberry-pi/wiringpi/
 Adaptado y comentado por: Luis Alberto Rivera
 
 Programa para comunicar la Raspberry Pi con el integrado MCP3002, que realiza
 conversiones A/D. La comunicación se hace vía SPI.
 
 Nota 1: La Raspberry Pi tiene dos canales SPI.
 Nota 2: El chip MCP3002 tiene 2 canales ADC.
 Cuidado con los voltajes de entrada para el MCP3002. Deben estar entre 0 y VDD.
 Se sugiere usar VDD = 3.3 V de la Raspberry Pi.
 Recuerde conectar VSS del MCP3002 a la tierra de la RPi.
 
 Recuerde compilar usando -lwiringPi
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>			// Para los tipos enteros como uint8_t y uint16_t
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define SPI_CHANNEL	      0	// Canal SPI de la Raspberry Pi, 0 ó 1
#define SPI_SPEED 	1500000	// Velocidad de la comunicación SPI (reloj, en HZ)
                            // Máxima de 3.6 MHz con VDD = 5V, 1.2 MHz con VDD = 2.7V
#define ADC_CHANNEL       0	// Canal A/D del MCP3002 a usar, 0 ó 1

uint16_t get_ADC(int channel);	// prototipo

int main(void)
{
    uint16_t ADCvalue;
	
	// Configura el SPI en la RPi
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0)
	{
		printf("wiringPiSPISetup falló.\n");
		return(-1);
	}

	// Bucle que constantemente lee los valores convertidos del canal seleccionado,
    // y lo despliega en la pantalla.
	// Ésta es una prueba simple, con una frecuencia de muestreo de ~1 Hz. Recordar que sleep()
	// no es una función muy precisa...
	while(1)
	{
		ADCvalue = get_ADC(ADC_CHANNEL);
		printf("Valor de la conversión: %d\n", ADCvalue);
		fflush(stdout);
		sleep(1);
	}
     
  return 0;   
}

// Como se describe en las secciones 5 y 6 del manual del MCP3002, necesitamos
// enviar dos bytes para iniciar la conversión. El primer byte incluye un "start bit",
// y bits que indican el modo y el canal a usar. El segundo byte no importa (pero se
// debe enviar). Al ser enviados los dos bytes de la RPi al MCP3002, dos bytes se
// regresan, los cuales contienen el valor convertido. Pueden leer sobre comunicación
// SPI para más detalles.
// La comunicación podría hacerse "a mano". Habría que mapear registros, configurar
// los puertos GPIO adecuados, enviar datos usando funciones como ioctl(), etc. Sin
// embargo, la utilidad SPI de wiringPi nos facilita el trabajo.
// Pueden implementar la comunicación "a mano" si se sienten aventureros...

// Entrada: ADC_chan -- 0 o 1
// Salida: un entero "unsigned" de 16 bit  con el valor de la conversión. Dado que la
//         resolución del ADC es de 10 bits, el valor retornado estará entre 0 y 1023.
// Asume modo "Single Ended" (no "Pseudo-Differential Mode").
uint16_t get_ADC(int ADC_chan)
{
	uint8_t spiData[2];	// La comunicación usa dos bytes
	uint16_t resultado;
	
	// Asegurarse que el canal sea válido. Si lo que viene no es válido, usar canal 0.
	if((ADC_chan < 0) || (ADC_chan > 1))
		ADC_chan = 0;

	// Construimos el byte de configuración: 0, start bit, modo, canal, MSBF: 01MC1000
	spiData[0] = 0b01101000 | (ADC_chan << 4);  // M = 1 ==> "single ended"
												// C: canal: 0 ó 1
	spiData[1] = 0;	// "Don't care", este valor no importa.
	
	// La siguiente función realiza la transacción de escritura/lectura sobre el bus SPI
	// seleccionado. Los datos que estaban en el buffer spiData se sobreescriben por
	// los datos que vienen por SPI.
	wiringPiSPIDataRW(SPI_CHANNEL, spiData, 2);	// 2 bytes
	
	// spiData[0] y spiData[1] tienen el resultado (2 bits y 8 bits, respectivamente)
	resultado = (spiData[0] << 8) | spiData[1];
	
	return(resultado);
}
