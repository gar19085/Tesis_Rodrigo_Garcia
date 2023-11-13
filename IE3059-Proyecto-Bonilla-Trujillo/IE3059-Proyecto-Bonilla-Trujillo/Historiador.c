/*
 =======================================================================
 Name        : Historiador.c
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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>	
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <time.h>
#include <sys/wait.h>
/* =====================================================================
	Directivas a ejecutar
===================================================================== */
#define MSG_SIZE 60		// Tamaño (máximo) del mensaje
#define INIT_VALUE	1
#define DataBaseUTRs "DataBaseUTRs.txt"
/* =====================================================================
	Prototipos de funciones
===================================================================== */
void hilo_comunicacion(void *ptr);	//para hilo de comunicacion
void hilo_menu(void *ptr);			//para hilo de evento iot
void error(const char *msg);		//prototipo de funcion de error
char* copyString(char s[]);
/* =====================================================================
	variables globales a implementar
===================================================================== */
sem_t my_semaphore;
//udp
int sockfd, length, n;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;
char buffer[100];
//para datos recibidos
char StringArray[1000][100];		//capaz de registrar 1 hora con 2 UTRs
int puerto,contador=0;
int terminal;
//parte de menu
char input[4];
char mensaje[4];
int continuo=0;
//obtener el tiempo
static int seconds_last=99;
char TimeString[128];
struct timeval current_time;
double delta, total,tiempo_inicial,tiempo_actual;
//

/* =====================================================================
	Interrupts Service Routine (ISR)
===================================================================== */
void ISR()
{
	
}
/* =====================================================================
	main, 2do argumento es el puerto >1024
===================================================================== */
int main(int argc, char *argv[])
{
	continuo=0;
	//----------------------recepcion de argumentos de entrada
	if(argc != 2) 
	{
		fprintf(stderr,"ERROR, no indicó el puerto a usar\n");	
		fprintf(stderr,"Uso: %s num_puerto\n", argv[0]);	
		exit(1);
	}
	
	//udp 
	//----------------------inicializacion de sockets
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if(sockfd < 0)
		error("Opening socket");;
	//----------------------protocolos de conexion
	length = sizeof(server);			// length of structure
	memset((char *)&server, 0, length); // sets all values to zero.
	server.sin_family = AF_INET;		// symbol constant for Internet domain
	server.sin_addr.s_addr = htonl(INADDR_ANY);	// para recibir de cualquier interfaz de red
	server.sin_port = htons(atoi(argv[1]));	// port number
	//----------------------union de socket a la ip y puerto
	if(bind(sockfd, (struct sockaddr *)&server, length) < 0)
       error("binding");

	fromlen = sizeof(struct sockaddr_in);	// size of structure
	//----------------------inicializacion de valores en semaforo
	sem_init(&my_semaphore, 0, INIT_VALUE);
	//----------------------inicializacion de variables para hilos
	pthread_t var_udp,var_menu;
	//----------------------inicializacion de hilos
	pthread_create(&var_udp, NULL, (void*)&hilo_comunicacion, NULL);
	pthread_create(&var_menu, NULL, (void*)&hilo_menu, NULL);
	//----------------------retorno de hilos
	pthread_join(var_udp,NULL);
	pthread_join(var_menu,NULL);
	//----------------------creacion y union de lineas impares y pares
	printf("Ya se acabaron los hilos\n");
	return 0; 		// cerrar el socket cuando se deja de usar.
	exit(0);
}
/* =====================================================================
	Funciones 
===================================================================== */

/* ==========
	Funcion para hilo de comunicacion
=============*/
void hilo_comunicacion(void *ptr)
{
	
	/* ================================================================
		Loop de comunicacion
	================================================================ */
	while(1)
	{
		/*time_t mytime = time(NULL);
		char * time_str = ctime(&mytime);
		time_str[strlen(time_str)-1] = '\0';
		printf("Current Time : %s\n", time_str);
		sleep(1);*/
		//----------------------buffer se hace 0
		//memset(buffer, 0, MSG_SIZE);
		
		//----------------------recepcion de valores recibidos desde UTRs
		n = recvfrom(sockfd, buffer, 50, 0, (struct sockaddr *)&from, &length);
	    if(n < 0)
	 		error("recvfrom recibido"); 
		//----------------------guarda valores recibidos en base de datos
		sem_wait(&my_semaphore);
		memcpy(StringArray[contador],buffer,strlen(buffer));
		sem_post(&my_semaphore);
		
		//----------------------ver si se despliegue desde menu
		if (continuo==0)
		{
			contador++;
		}
		else if(continuo==1)
		{
			puts("U E Tiempo                   Sw Bt Led V \n");
			printf("%s\n",StringArray[contador]);
			contador++;
			fflush(stdout);
		}
		//----------------------mensaje de recibido
		n = sendto(sockfd, mensaje, MSG_SIZE, 0, (struct sockaddr *)&from, fromlen);
		if(n < 0)
	 		error("sendto");
		if (contador>999)
			contador=0;
	} 	
	

	pthread_exit(0);
}

/* ==========
	Funcion para hilo de evento IoT
=============*/
void hilo_menu(void *ptr)
{
	FILE *datos = fopen(DataBaseUTRs, "w");
	fputs("U E Tiempo   Sw Bot LED V\n", datos);
	/* =======================================================================
		Loop de eventos IoT
	========================================================================= */
	while(1)
	{
		//----------------------menu principal
		printf("--------------------------------------\n");
		printf("--------------------------------------\n");
		printf("  ¡BIENVENIDO AL HISOTRIADOR!\n");
		printf("  Ingrese el menu que desea desplegar\n");
		printf("  Despliegue continuo: con\n");
		printf("  Encender LEDs en UTRs: uon\n");
		printf("  Apagar LEDs en UTRs: off\n");
		printf("  Desplegar todo el historial: ver\n");
		printf("--------------------------------------\n");
		printf("--------------------------------------\n");
		scanf("%9s", input);
		//----------------------despliegue de datos continuo
		if (strncmp(input, "con",3)==0)		
		{
			continuo=1;
			printf("  DESPLIEGUE CONTINUO\n");
			puts("U E Tiempo   Sw Bt Led V n");
			//continuo=1;
		}
		else 
			continuo=0;
		//----------------------control de leds en UTRs
		if (strcmp(input, "uon")==0)	
		{
			continuo=0;
			printf("--------------------------------------\n");
			printf("  Seleccione los LEDs que desea encender\n");	
			printf("  UTR1 LED1: L11\n");
			printf("  UTR1 LED2: L12\n");
			printf("  UTR1 LED1: L21\n");
			printf("  UTR2 LED2: L22\n");
			printf("  Todas: all\n");
			printf("--------------------------------------\n");
			scanf("%9s", input);
			if (strcmp(input, "L11")==0)
			{
				sprintf(mensaje,"111\n");
				printf("Haz prendido el Led1 UTR1\n");
			}
			else if (strcmp(input, "L12")==0)
			{
				sprintf(mensaje,"112\n");
				printf("Haz prendido el Led2 UTR1\n");
			}
			else if (strcmp(input, "L21")==0)
			{
				sprintf(mensaje,"121\n");
				printf("Haz prendido el Led1 UTR2\n");
			}
			else if (strcmp(input, "L22")==0)
			{
				sprintf(mensaje,"122\n");
				printf("Haz prendido el Led2 UTR2\n");
			}
			else if (strcmp(input, "all")==0)
			{
				sprintf(mensaje,"all\n");
				printf("Haz prendido todos los LEDs\n");
			}
		}
		//----------------------apagar UTRS
		if (strcmp(input, "off")==0)			
		{
			continuo=0;
			printf("--------------------------------------\n");
			printf("Seleccione los LEDs que desea apagar\n");	
			printf("  UTR1 LED1: L11\n");
			printf("  UTR1 LED2: L12\n");
			printf("  UTR1 LED1: L21\n");
			printf("  UTR2 LED2: L22\n");
			printf("  Todas: all\n");
			printf("--------------------------------------\n");
			scanf("%9s", input);
			if (strcmp(input, "L11")==0)
			{
				sprintf(mensaje,"011\n");
				printf("Haz apagado el Led1 UTR1\n");
			}
			else if (strcmp(input, "L12")==0)
			{
				sprintf(mensaje,"012\n");
				printf("Haz apagado el Led2 UTR1\n");
			}
			else if (strcmp(input, "L21")==0)
			{
				sprintf(mensaje,"021\n");
				printf("Haz apagado el Led1 UTR2\n");
			}
			else if (strcmp(input, "L22")==0)
			{
				sprintf(mensaje,"022\n");
				printf("Haz apagado el Led2 UTR2\n");
			}
			if (strcmp(input, "all")==0)
			{
				sprintf(mensaje,"000\n");
				printf("Haz apagado el Led2 UTR2\n");
			}
		}
		//----------------------despliegue de datos almacenados
		if (strncmp(input, "ver",3)==0)		
		{
			printf("Desplegando historial\n");
			//fputs("U E Tiempo   Sw Bot LED V\n", datos);
			for(int i = 0; i < 1000 ; i++)
			{
				printf("%s\n",StringArray[i]); 
				printf("\n");
				//fputs(StringArray[i], datos);
				fputs(StringArray[i], datos);
			}
			fclose(datos); //se cierra el nuevo archivo
			
		}

	}
	//----------------------se sale del hilo
	pthread_exit(0);
}


/* ==========
	Funcion para desplegar mensajes de error
=============*/
void error(const char *msg)
{
    perror(msg);	// escribe al standard error (la terminal, por defecto)
    exit(1);
}


char* copyString(char s[])
{
    char* s2;
    s2 = (char*)malloc(20);
 
    strcpy(s2, s);
    return (char*)s2;
}