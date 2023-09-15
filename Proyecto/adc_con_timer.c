/* adc_con_timer.c
 Basado en código tomado de: https://projects.drogon.net/raspberry-pi/wiringpi/
 Adaptado y comentado por: Luis Alberto Rivera
 
 Programa para comunicar la Raspberry Pi con el integrado MCP3002, que realiza
 conversiones A/D. La comunicación se hace vía SPI.
 
 El período de muestreo se determina con un timer. Para que los períodos sean
 consistentes, es mejor asignar una prioridad > 0 al hilo del ADC. El programa
 acepta como parámetro el valor de la prioridad:
 
    ./adc_con_timer P
    
Si no se ingresa la prioridad P, o si está fuera del intervalo [1, 95], la prioridad se deja
como 0, por lo que no se llama a sched_setscheduler. Si se ingresa una prioridad entre
1 y 95, se le asigna esa prioridad al hilo del ADC.
 
 Recuerde compilar usando -lwiringPi
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <sched.h>
#include <sys/timerfd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define SPI_CHANNEL	      0	// Canal SPI de la Raspberry Pi, 0 ó 1
#define SPI_SPEED 	1500000	// Velocidad de la comunicación SPI (reloj, en HZ)
                            // Máxima de 3.2 MHz con VDD = 5V, 1.2 MHz con VDD = 2.7V
#define ADC_CHANNEL       0	// Canal A/D del MCP3002 a usar, 0 ó 1
#define PERIODO           1 // en milisegundos
#define INIT             10 // en milisegundos
#define MILI_A_NANO 1000000
#define PRIORIDAD_ADC     0

// prototipos
uint16_t get_ADC(int channel);
int timer_config(int, int);
void wait_period(const int);

struct itimerspec itval;

int main(int argc, char *argv[])
{
    uint16_t ADCvalue;
    struct timeval tinicial, tfinal;
    float dt;
    int cont_print = 0;
	int prioridad = PRIORIDAD_ADC;	// default value
	
	if(argc == 2){
		prioridad = atoi(argv[1]);	// user input for initial sem value
	}
    
	// Configura el SPI en la RPi
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
		printf("wiringPiSPISetup falló.\n");
		return -1 ;
	}

	if((prioridad > 0) && (prioridad < 95)) {
        // Asignar prioridad
        struct sched_param param;
        param.sched_priority = prioridad;
        if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
            perror("sched_setscheduler falló, thread 1");
            exit(20);
        }
    }
	
    // Configurar e iniciar el timer.
	int timer_fd = timer_config(PERIODO, INIT);
	
	// Bucle que constantemente lee los valores convertidos del canal seleccionado,
    // y lo despliega en la pantalla.
	// Ésta es una prueba simple, con una frecuencia de muestreo de 1 kHz. Recordar que usleep()
	// no es una función muy precisa, por lo que se prefiere usar un timer.
    dt = 0.0;
	while(1) {
        gettimeofday(&tinicial, 0);
		ADCvalue = get_ADC(ADC_CHANNEL);
        
        if(cont_print == 0) {
                printf("Valor de la conversión: %d, dt anterior = %f ms\n", ADCvalue, dt);
                fflush(stdout);
        }
		cont_print = (cont_print + 1)%((int)(1*1000/PERIODO));
        
        wait_period(timer_fd);
        //usleep(1000);
        
        gettimeofday(&tfinal, 0);
        dt = tfinal.tv_sec*1000.0 + tfinal.tv_usec/1000.0 - tinicial.tv_sec*1000.0 - tinicial.tv_usec/1000.0;
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

// Entrada: ADC_chan -- 0 ó 1
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


// timer_config
// Función que configura el timer.
// Entradas: period - período, en milisegundos (entre 1 y 999)
//		   inittime - tiempo a esperar antes de iniciar el conteo, luego de
//					  iniciar el timer, también en milisegundos (entre 1 y 999)
// Salida: el file descriptor correspondiente al timer configurado.
int timer_config(int period, int inittime)
{
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(fd == -1) {
        perror("Error al crear el timer.");
        exit(1);
    }

	// Asegurarse que el período está en el intervalo deseado
	if(period > 999)
		period = 999;
	if(period < 1)
		period = 1;
	
    //struct itimerspec itval;
    // Tiempo en que se dispara el timer
    itval.it_interval.tv_sec = 0;	// se asumen períodos menores a 1 segundo
    itval.it_interval.tv_nsec = (long)period*MILI_A_NANO;
    // Tiempo a esperar antes de empezar los ciclos
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = (long)inittime*MILI_A_NANO;

    // Iniciar el timer
    if(timerfd_settime(fd, 0, &itval, NULL) == -1) {
        perror("Error al iniciar el timer.");
        exit(1);
    }
    return fd;
}

// Función que espera el tiempo restante antes que venza el timer.
// Entrada: fd - file descriptor correspondiente al timer.
void wait_period(int fd)
{
    // read tells you how many periods it's been since you last checked in
    uint64_t num_periods = 0;
    if(read(fd, &num_periods, sizeof(num_periods)) == -1) {
        perror("Error al leer el timer.");
        exit(1);
    }

    if(num_periods > 1) {
        printf("Se pasó de un período: %.0f\n", 1.0*num_periods); // como float, para no hacer overflow
        fflush(stdout);
    }
}


