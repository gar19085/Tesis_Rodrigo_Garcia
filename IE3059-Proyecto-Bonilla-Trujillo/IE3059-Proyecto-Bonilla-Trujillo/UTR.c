/*
 =======================================================================
 Name        : Muestreo.c
 Author      : Andy Bonilla (19451) y Jose Trujillo (19452)
 Version     :
 Copyright   : Electronica Digital 3
 Description : 
 * 
 =======================================================================

/* =====================================================================
 	 Inclusion de librerias
===================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <stdint.h>			//enteros como uint8_t y uint16_t
#include <wiringPi.h>		//wiringPi para gpio
#include <wiringPiSPI.h>	//wiringPi para conexion SPI
#include <pthread.h>		//para multi-hilos
#include <string.h>			//manejo de string
#include <sys/types.h>
#include <sys/socket.h>		//uso de sockets
#include <netinet/in.h>		//acceso a la red
#include <netdb.h>			//acceso a la red
#include <arpa/inet.h>		//acceso a la red
#include <sys/time.h>
#include <time.h>
#include <sys/timerfd.h>
#include <semaphore.h>
/* =====================================================================
	Directivas a ejecutar
===================================================================== */
//para conexion de integrado de ADC
#define SPI_CHANNEL	      0	// Canal SPI de la Raspberry Pi, 0 ó 1
#define SPI_SPEED 	1500000	// Velocidad de la comunicación SPI (reloj, en HZ)
#define ADC_CHANNEL       0	// Canal A/D del MCP3002 a usar, 0 ó 1
//para el tamano de
//mensajes via tcp
#define MSG_SIZE 100		// Tamaño (máximo) del mensaje
//para el uso de semaforo
#define INIT_VALUE	1		// Para el valor inicial del semáforo
//del timer
#define milis_nanos 1000000
/* =====================================================================
	Prototipos de funciones
===================================================================== */
uint16_t get_ADC(int channel);			//para adc
void funcion_adc(void *ptr);			//para muestreo con adc
void funcion_envio(void *ptr);			//para mandar datos via udp
void funcion_recibir(void *ptr);			//para recibir datos via udp
void funcion_eventos(int ev);
void error(const char *msg);			//errores en comunicacion tcp
void espera(int tiempo);
/* =====================================================================
	variables globales a implementar
===================================================================== */
//para semaforo
sem_t my_semaphore;
//para antirrebores de entradas digitaes
int antirrebote1,antirrebote2,antirrebote3=0,antirrebote4=0;
int inter1,inter2,bot1,bot2;	//para antirrebotes
int led1,led2;
//para mandar los mensajes
char datos_udp[50];								//para mensajes tcp
int eventos=0;
//para conversion de voltaje en adc
uint16_t ADCvalue;
uint16_t conversion;
float voltaje;
uint16_t i;
//para comunicacion via sockets
int sockfd, n;
unsigned int length;
struct sockaddr_in server, from;
struct hostent *hp;
char buffer[MSG_SIZE];
//obtener el tiempo
static int seconds_last=99;
char TimeString[128];
struct timeval current_time;
double delta,total,tiempo_inicial,tiempo_actual;
//arreglo de eventos
char recopilacion[1000000][50];
char datos_eventos[100];
int contador=0;
//para ver si hubo actualizacion o no
int overshoot=0,undershoot=0;
int bandera;
/* =====================================================================
	Interrupts Service Routine (ISR)
===================================================================== */
void ISR()
{
	//----------------------para cambio de estado de interruptor 1
	if (digitalRead(12)==1)
	{
		inter1=1;
		usleep(250000);
		funcion_eventos(1);
	}
	else if (digitalRead(12)==0)
	{
		usleep(250000);
		inter1=0;
	}
	//----------------------para cambio de estado de interruptor 2
	if (digitalRead(16)==1 )
	{
		inter2=1;
		usleep(250000);
		funcion_eventos(2);
	}
	else if (digitalRead(12)==0)
	{
		usleep(250000);
		inter2=0;
	}
	//----------------------para cambio de estado boton 1
	if (digitalRead(20)==0 )
	{
		bot1=1;
		usleep(250000);
		funcion_eventos(3);
	}
	
	//----------------------para cambio de estado boton 2
	if (digitalRead(18)==0 )
	{
		bot2=1;
		usleep(250000);
		funcion_eventos(4);
	}
	//----------------------para evento IoT 1
	if (digitalRead(22)==1 )
	{
		led1=1;
		usleep(250000);
		funcion_eventos(7);
		digitalWrite(23,1);
	}
	else if  (digitalRead(22)==0)
	{
		usleep(250000);
		digitalWrite(23,0);
		led1=0;
	}
	//----------------------para evento IoT 2
	if (digitalRead(27)==1 )
	{
		led2=1;
		usleep(250000);
		funcion_eventos(8);
		digitalWrite(24,1);
	}
	else if (digitalRead(27)==0)
	{
		usleep(250000);
		led2=0;
		digitalWrite(24,0);
	}
}

/* =====================================================================
	main
===================================================================== */
int main(int argc, char *argv[])
{
    //----------------------recepcion de argumentos de entrada
    if(argc != 3)	
    {
		fprintf(stderr,"ERROR, Uso correcto: %s IP_servidor num_puerto\n", argv[0]);
		exit(0);
    }
    //----------------------creacion de socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	if(sockfd < 0)
		error("socket");
	//----------------------jala la ip del server
	server.sin_family = AF_INET;	
	hp = gethostbyname(argv[1]);	
	if(hp == 0)
		error("Unknown host");
    //----------------------creacion de puerto y direcciones
    memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));	
	length = sizeof(struct sockaddr_in);	
	//----------------------configuracion de pines
	wiringPiSetupGpio();
	pinMode(12,INPUT);						//modo interruptor 1
	pinMode(16,INPUT);						//modo interruptor 2
	pinMode(20,INPUT);						//modo boton 1
	pinMode(18,INPUT);						//modo boton 2
	pinMode(23,OUTPUT);						//modo led1
	pinMode(24,OUTPUT);						//modo led2
	pinMode(25,OUTPUT);						//modo buzzer
	pinMode(22,OUTPUT);						//modo IoT1
	pinMode(27,OUTPUT);						//modo IoT2
	pullUpDnControl(12,PUD_UP);				//pull up interruptor 1
	pullUpDnControl(16,PUD_UP);				//pull up interruptor 2
	pullUpDnControl(20,PUD_UP);				//pull up boton 1
	pullUpDnControl(18,PUD_UP);				//pull up boton 2
	pullUpDnControl(22,PUD_UP);				//pull up IOT 1
	pullUpDnControl(27,PUD_UP);				//pull up IOT 2
	wiringPiISR(12, INT_EDGE_BOTH,ISR);	//interrupcion interruptor 1
	wiringPiISR(16, INT_EDGE_BOTH,ISR);	//interrupcion interruptor 2
	wiringPiISR(20, INT_EDGE_RISING,ISR);	//interrupcion boton 1
	wiringPiISR(18, INT_EDGE_RISING,ISR);	//interrupcion boton 2
	wiringPiISR(22, INT_EDGE_BOTH,ISR);	//interrupcion IoT 1
	wiringPiISR(27, INT_EDGE_BOTH,ISR);	//interrupcion IoT 2
	//----------------------iniciar funcion tiempo
	gettimeofday(&current_time,NULL);
	tiempo_inicial=current_time.tv_sec + current_time.tv_usec/1000000.0;
	//----------------------configuracion de ADC
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0)
	{
		printf("wiringPiSPISetup falló.\n");
		return(-1);
	}
	//----------------------inicializacion de semaforo
	sem_init(&my_semaphore, 0, INIT_VALUE);
	//----------------------inicializacion de variables para hilos
	pthread_t var_adc,var_switch,var_envio,var_recibir;	
	//----------------------inicializacion de hilos
	//funcion_eventos(0);
	pthread_create(&var_adc, NULL, (void*)&funcion_adc, NULL);
	pthread_create(&var_envio, NULL, (void*)&funcion_envio, NULL);
	pthread_create(&var_recibir, NULL, (void*)&funcion_recibir, NULL);
	//----------------------pedazo para unir los hilos
	pthread_join(var_adc,NULL);
	pthread_join(var_envio,NULL);
	pthread_join(var_recibir,NULL);
	printf("Ya se terminaron los hilos\n");
	return 0;
	exit(0);
}
/* =====================================================================
	Funciones para hilos
===================================================================== */

/* ==========
	Funcion para jalar valores ADC
=============*/
uint16_t get_ADC(int ADC_chan)
{
	//----------------------bytes a usar 
	uint8_t spiData[2];	
	uint16_t resultado;	
	//----------------------validacion de canal
	if((ADC_chan < 0) || (ADC_chan > 1))
		ADC_chan = 0;
	//----------------------byte de configuración
	spiData[0] = 0b01101000 | (ADC_chan << 4);  
	spiData[1] = 0;									
	//----------------------escritura/lectura de SPI.
	wiringPiSPIDataRW(SPI_CHANNEL, spiData, 2);	// 2 bytes
	resultado = (spiData[0] << 8) | spiData[1];
	return(resultado);
}
/* ==========
	Funcion para muestreo de ADC
=============*/
void funcion_adc(void *ptr)
{
	//----------------------configuracion del timer
	int timer_fd1 = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec itval;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 100000000;		//100ms
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 1000000;			//1ms
	if(	timerfd_settime(timer_fd1, 0, &itval, NULL) == 1)
	{
		perror("Error al hacer el timer.\n");
		exit(0);
	}
	//----------------------ciclo de ejecucion
	while(1)
	{
		//----------------------conversion de adc
		ADCvalue = get_ADC(ADC_CHANNEL);
		fflush(stdout);
		voltaje=(ADCvalue*3.3)/1203.0;
		//----------------------verificacion dentro de rango 
		if (voltaje>0.5 && voltaje<2.5)
		{
			if (bandera==0)
			{
				funcion_eventos(6);
				bandera=1;
			}
		}
		//----------------------verificacion fuera de rango
		if (voltaje>2.5 || voltaje<0.5)
		{
			digitalWrite(25,1);
			if (bandera==1)
			{
				
				funcion_eventos(5);
				bandera=0;
			}
		}
		else
			digitalWrite(25,0);
		//----------------------configuracion y arranque de timer
		espera(timer_fd1);
	}
	//----------------------salida del hilo
	pthread_exit(0);
}

/* ==========
	Funcion para hilo de envio 
=============*/
void funcion_envio(void *ptr)
{
	puts("U E Tiempo                   Sw Bt Led V \n");
	while(1)
	{			
		//inter1=0,inter2=0,bot1=0,bot2=0;
		funcion_eventos(0);	
		//----------------------si en caso hubo algun evento
		for(int j=0;j<contador+1;j++)
		{
			
			//----------------------mandar datos via udp
			n = sendto(sockfd, recopilacion[j], strlen(recopilacion[j]), 0, (const struct sockaddr *)&server, length);
			if(n < 0)
				error("Sendto");
			
			puts(recopilacion[j]);
			fflush(stdout);
			
		}
		//funcion_eventos(0);
		contador=0;
		sleep(2);
	}
	//----------------------salida del hilo
	pthread_exit(0);
}

/* ==========
	Funcion para hilo de comunicacion
=============*/
void funcion_recibir(void *ptr)
{
	while(1)
	{	
		//----------------------se recibe del server
		n = recvfrom(sockfd, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
		if(n < 0)
			error("recvfrom");
		//printf(buffer);
		//----------------------para prender/apagar led1
		if (strcmp("111\n",buffer)==0)
		{
			digitalWrite(23,1);
			led1=1;
		}
		else if (strcmp("011\n",buffer)==0)
		{
			digitalWrite(23,0);
			led1=0;
		}
		//----------------------para prender/apagar led2
		if (strcmp("112\n",buffer)==0)
		{
			digitalWrite(24,1);
			led2=1;
		}	
		else if (strcmp("012\n",buffer)==0)
		{
			digitalWrite(24,0);
			led2=1;
		}
		//----------------------para prender/apagar led2
		if (strcmp("all\n",buffer)==0)
		{
			digitalWrite(23,1);
			digitalWrite(24,1);
			led1=1;
			led2=1;
		}
		if (strcmp("000\n",buffer)==0)
		{
			digitalWrite(23,0);
			digitalWrite(24,0);
			led1=0;
			led2=0;
		}		
	}
}

/* ==========
	Funcion para hilo de eventos
=============*/
void funcion_eventos(int ev)
{
	//memset(datos_eventos, 0, MSG_SIZE);	// sets all values to zero
	//----------------------obtener el tiempo actual
	time_t mytime=time(NULL);
	char * time_str=ctime(&mytime);
	time_str[strlen(time_str)-1]='\0';
	//----------------------escritura de arreglo
	memset(datos_eventos, 0, MSG_SIZE);	// sets all values to zero
	sprintf(datos_eventos,"1 %d %s %d%d %d%d %d%d %f\n",ev,time_str,inter1,inter2,bot1,bot2,led1,led2,voltaje);
	fflush(stdout);
	//----------------------se copia a estructura
	strcpy(recopilacion[contador],datos_eventos); 	//copia buffer en array
	fflush(stdout); 		
	contador++;		
}
/* ==========
	Funcion para error en comunicacion
=============*/
void error(const char *msg)
{
	perror(msg);
    exit(0);
}
/* ==========
	Funcion para tiempo de espera
=============*/
void espera(int tiempo)
{
	uint64_t periodos=0;

	if(read(tiempo,&periodos,sizeof(periodos))==-1)
	{
		perror("Error al leer el timer");
		exit(1);
	}
	if (periodos>1)
	{
		puts("Se pasó el tiempo");
		exit(1);
	}
}
