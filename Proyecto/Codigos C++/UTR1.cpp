/*
 ============================================================================
 Nombre: UTR1.cpp
 Autor:  Rodrigo José García Ambrosy
 Proyecto
 ============================================================================
 */
#include <wiringPi.h> // Librería para controlar pines GPIO
#include <iostream> // Librería estándar de entrada/salida
#include <thread>   // Librería para utilizar hilos
#include <chrono>   // Librería para utilizar la función sleep_for
#include <semaphore>    // Librería para utilizar semáforos
#include <cstdio>   // Librería estándar de entrada/salida
#include <cstdlib>  // Librería estándar para funciones generales
#include <ctime>    // Librería para utilizar la función time
#include <cstring>  // Librería para la manipulación de cadenas
#include <wiringPiSPI.h> // Librería para utilizar el bus SPI
#include <string>   // Librería para utilizar cadenas
#include<sys/socket.h>  // Librería para utilizar sockets
#include<arpa/inet.h>   // Librería para manipular direcciones IP


#define SPI_CHAN 0  // Canal del bus SPI
#define SPI_SPEED 1500000   // Velocidad del bus SPI

#define ADC_CHANNEL 0   // Canal del ADC  
#define MSG_SIZE 40     // Tamaño del mensaje
#define IP "10.0.0.255" // Dirección IP

#define LUZ_1   20  // Pin para la luz 1
#define LUZ_2   21  // Pin para la luz 2
#define BTN1    5   // Pin para el botón 1
#define BTN2    17  // Pin para el botón 2
#define Switch1 19  // Pin para el switch 1
#define Switch2 26  // Pin para el switch 2
#define Alarm   13  // Pin para la alarma
#define Arduino 12  // Pin para el Arduino


uint16_t get_ADC(int channel);  // Declaración de la función get_ADC
float voltajeadc=0; // Variable para almacenar el voltaje del ADC
uint32_t time_on=0; // Variable para almacenar el tiempo encendido
char timestamp_str [26];    // Variable para almacenar el timestamp
uint32_t numero_eventos=0;  // Variable para almacenar el número de eventos
#define MAX_NEVENTOS 300    // Número máximo de eventos
char eventos_pendientes[MAX_NEVENTOS+1][64];    // Eventos pendientes
uint16_t n_eventos_pendientes_envio =0; // Número de eventos pendientes de envío
uint8_t boton1 =0;  // Variable para almacenar el estado del botón 1
uint8_t boton2 =0;  // Variable para almacenar el estado del botón 2
uint8_t  switch1=0; // Variable para almacenar el estado del switch 1
uint8_t  switch2=0; // Variable para almacenar el estado del switch 2
uint8_t   Myswitch1 =0; // Variable para almacenar el estado del switch 1
uint16_t tiempo_buzzer_on = 0;  // Variable para almacenar el tiempo de encendido del buzzer
uint8_t status_led_1= 0;    // Variable para almacenar el estado del LED 1
char linea_status [128];    // Variable para almacenar el estado
char linea_evento[128]; // Variable para almacenar el evento

int A; 
int PUSH1 = 0;
int PUSH2 = 0;
int FLG1 = 0;
int FLG2 = 0;
int FLGB = 0;
int LD1 = 0;
int LD2 = 0;

int sockfd, n;  
unsigned int length; 
struct sockaddr_in server, addr;
char buffer[MSG_SIZE];	
int boolval = 1;	


class LightController { // Clase para el control de las luces
public:
    LightController(int pin) : pin_(pin) {
        pinMode(pin_, OUTPUT);
    }

    void turnOn() {
        digitalWrite(pin_, HIGH);
    }

    void turnOff() {
        digitalWrite(pin_, LOW);
    }

private:
    int pin_;
};

class ButtonHandler {   // Clase para el control del botón
public:
    ButtonHandler(int pin) : pin_(pin) {
        pinMode(pin_, INPUT);
        pullUpDnControl(pin_, PUD_UP);
    }

    bool isButtonPressed() {
        return digitalRead(pin_) == HIGH;
    }

    void waitForButtonRelease() {
        while (isButtonPressed()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    int pin_;
};

class SwitchHandler {   // Clase para el control del switch
public:
    SwitchHandler(int pin) : pin_(pin) {
        pinMode(pin_, INPUT);
        pullUpDnControl(pin_, PUD_UP);
    }

    bool isSwitchOn() {
        return digitalRead(pin_) == HIGH;
    }

    void waitForSwitchOff() {
        while (isSwitchOn()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    int pin_;
};

class AlarmHandler {    // Clase para el control de la alarma
public:
    AlarmHandler(int pin) : pin_(pin) {
        pinMode(pin_, OUTPUT);
    }

    void turnOn() {
        digitalWrite(pin_, HIGH);
    }

    void turnOff() {
        digitalWrite(pin_, LOW);
    }

private:
    int pin_;
};

void error(const char *msg) {   // Función para imprimir errores
    perror(msg);
    exit(1);
}

std::string timestamp() {   // Función para obtener el timestamp
    std::time_t t = std::time(nullptr);
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%T", std::localtime(&t));
    return mbstr;
}

std::string update_timestep() { // Función para actualizar el timestamp
    std::time_t t = std::time(nullptr);
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%T", std::localtime(&t));
    return mbstr;
}

int main() {
    wiringPiSetupGpio();    // Inicializa la librería wiringPi
    //Crea los objetos de las clases
    LightController luz1Ctrl(LUZ_1);    
    LightController luz2Ctrl(LUZ_2);    
    ButtonHandler button1(BTN1);   
    ButtonHandler button2(BTN2);    
    SwitchHandler switch1(Switch1); 
    SwitchHandler switch2(Switch2);
    AlarmHandler alarm(Alarm);
    A = (1/10900)/2; // Constante para el cálculo de la resistencia
    
    //Configura las interrupciones
    wiringPiISR(LUZ_1, INT_EDGE_BOTH, (void*)&PUSH1);
    wiringPiISR(LUZ_2, INT_EDGE_BOTH, (void*)&PUSH2);
    wiringPiISR(Switch1, INT_EDGE_BOTH, (void*)&SWT1);
    wiringPiISR(Switch2, INT_EDGE_BOTH, (void*)&SWT2);
    wiringPiISR(Arduino, INT_EDGE_BOTH, (void*)&iotarduino);

    std::srand(time(NULL)); // Inicializa la semilla para la función rand()

    if (wiringPiSPISetup(SPI_CHAN, SPI_SPEED) < 0) {    // Verifica si se pudo inicializar el bus SPI
        std::cout << "No se pudo inicializar el SPI" << std::endl;
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);    // Crea el socket UDP (SOCK_DGRAM)
    if (sockfd < 0) error("ERROR opening socket");  // Verifica si hubo un error al crear el socket
    length = sizeof(server);    // Obtiene el tamaño de la estructura 'server'
    bzero(&server,length);  // Llena la estructura 'server' con ceros
    server.sin_family=AF_INET;   // Asigna el tipo de dirección (IPv4)
    server.sin_addr.s_addr=inet_addr(IP);   // Asigna la dirección IP del servidor
    server.sin_port=htons(atoi(argv[1]));   // Asigna el puerto del servidor
    if (bind(sockfd,(struct sockaddr *)&server,length)<0) error("ERROR on binding");   // Asocia el socket con la dirección y el puerto
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0) error("ERROR setting socket options");  // Habilita la opción de broadcast en el socket


    uint16_t ADCvalue;  

    while (1){
        time_on++;  // Incrementa el tiempo encendido

        update_timestamp(); // Actualiza el timestamp
        ADCvalue = get_ADC(ADC_CHANNEL);    // Obtiene el valor del ADC
    
    if ((time_on %20) == 0) // Verifica si se cumplió el tiempo
    {
        agregar_evento(0); // Agrega el evento 0
    }

    //Verifica condiciones de alarma
    alarm_low_voltage();   
    alarm_high_voltage();   

    if(voltajeadc > 2.5){   // Verifica si el voltaje es mayor a 2.5V
        alarm.turnOn();
    }
    if(voltajeadc < 0.5){   // Verifica si el voltaje es menor a 0.5V
        alarm.turnOn();
    }
    else if (voltajeadc > 0.5 && voltajeadc < 2.5)  // Verifica si el voltaje está entre 0.5V y 2.5V
    {
        alarm.turnOff();
    }
        
    std::cout << std::flush;    // Limpia el buffer de salida
    if ((time_on % 20) == 0)    
    {
        listar_eventos();   // Lista los eventos
        if (n_eventos_pendientes_envio > 0) // Verifica si hay eventos pendientes de envío
        {
            std::cout << "Enviando eventos pendientes" << std::endl;
            listar_eventos();
        }           
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));  // Espera 1ms
    }

    return 0;
}


void RTU1(void*ptr){    // Función para el control de la RTU1
    while (1)
    {
        memset(buffer, 0, MSG_SIZE);    // Limpia el buffer
        n = recvfrom(sockfd, buffer, MSG_SIZE, 0, (struct sockaddr*)&addr, &length);
        if (n < 0) error("ERROR in recvfrom");  // Verifica si hubo error
        std::cout << "Received a datagram. It says: " << buffer << std::endl;   // Imprime el mensaje recibido

        if (n < 0) error("ERROR in sendto");    // Verifica si hubo error
        
        if((std::strcmp(buffeer, "RTU1 LED1 ON") == 0)){    // Verifica si se recibió el comando para encender el LED1 de la RTU1
            char mensaje[64];
            std::cout << "LED1 ON" << std::endl;
            std::cout << std::flush;

            LUZ_1.turnOn();
            LD1 = 1;
            std::strcpy(buffer, "LED1 ON");
            std::cout << std::flush;
            std::sprintf(mensaje, "LED indicador 1 encendido");
            std::cout << std::flush;
            agregar_evento(11);
        }
        if ((std::strcmp(buffer, "RTU1 LED1 OFF") == 0))    // Verifica si se recibió el comando para apagar el LED1 de la RTU1
        {
            char mensaje[64];
            std::cout << "LED1 OFF" << std::endl;
            std::cout << std::flush;

            LUZ_1.turnOff();
            LD1 = 0;
            std::strcpy(buffer, "LED1 OFF");
            std::cout << std::flush;
            std::sprintf(mensaje, "LED indicador 1 apagado");
            std::cout << std::flush;
            agregar_evento(12);
        }
        if((std::strcmp(buffeer, "RTU1 LED2 ON") == 0)){    // Verifica si se recibió el comando para encender el LED2 de la RTU1
            char mensaje[64];
            std::cout << "LED2 ON" << std::endl;
            std::cout << std::flush;

            LUZ_2.turnOn();
            LD2 = 1;
            std::strcpy(buffer, "LED2 ON");
            std::cout << std::flush;
            std::sprintf(mensaje, "LED indicador 2 encendido");
            std::cout << std::flush;
            agregar_evento(13);
        }
        if((std::strcmp(buffeer, "RTU1 LED2 OFF") == 0)){   // Verifica si se recibió el comando para apagar el LED2 de la RTU1
            char mensaje[64];
            std::cout << "LED2 OFF" << std::endl;
            std::cout << std::flush;

            LUZ_2.turnOff();
            LD2 = 0;
            std::strcpy(buffer, "LED2 OFF");
            std::cout << std::flush;
            std::sprintf(mensaje, "LED indicador 2 apagado");
            std::cout << std::flush;
            agregar_evento(14);
        }
    }
}

uint16_t get_ADC(int channel){  // Función para obtener el valor del ADC
    uint8_t spiData[2];
    uint16_t resultado;

    if((channel > 1) || (channel < 0)){ // Verifica si el canal es válido
        channel = 0;
    }

    spiData[0] = 0b01101000 | (channel << 4);   // Configura el canal
    spiData[1] = 0; 

    wiringPiSPIDataRW(SPI_CHAN, spiData, 2);    // Lee el valor del ADC

    resultado = ((spiData[0]) << 8) | spiData[1];   // Almacena el valor del ADC

    voltajeadc = (resultado * 3.3) / 1023;  // Calcula el voltaje del ADC

    return resultado;
}


void Button1(void){ // Función para el control del botón 1
    char mensaje[64];
    int status_B1;
    get_ADC(ADC_CHANNEL);
    PUSH1 = !button1.isButtonPressed();
    status_B1 = 0;

    if(PUSH1 == 1){
        status_B1 = 1;
        agregar_evento(3);
    }
}

void Button2(void){ // Función para el control del botón 2
    char mensaje[64];
    int status_B2;
    get_ADC(ADC_CHANNEL);
    PUSH2 = !button2.isButtonPressed();
    status_B2 = 0;

    if(PUSH2 == 1){
        status_B2 = 1;
        agregar_evento(4);
    }
}

void SWT1(void){    // Función para el control del switch 1
    char mensaje[64];
    switch1 = !switch1.isSwitchOn();
    get_ADC(ADC_CHANNEL);
    
    if(switch1 == 1){
        agregar_evento(5);
    }
    if(switch1 == 0){
        agregar_evento(6);
    }
}

void SWT2(void){    // Función para el control del switch 2
    char mensaje[64];
    switch2 = !switch2.isSwitchOn();
    get_ADC(ADC_CHANNEL);
    
    if(switch2 == 1){
        std::sprintf(mensaje,"7\t%0.2f\n", voltajeadc);
        std::cout << std::flush;
        agregar_evento(7);
    }
    if(switch2 == 0){
        std::sprintf(mensaje,"8\t%0.2f\n", voltajeadc);
        std::cout << std::flush;
        agregar_evento(8);
    }
}

void iotarduino(void){  // Función para el control del Arduino
    char mensaje[64];
    status_led_1 = digitalRead(Arduino);
    get_ADC(ADC_CHANNEL);

    if(status_led_1 == 1){
        agregar_evento(9);
    }
    if(status_led_1 == 0){
        agregar_evento(10);
    }
}


void listar_eventos(){  // Función para listar los eventos
    int i; // Declara una variable entera llamada 'i'

    for (i = 0; i < n_eventos_pendientes_envio; i++)    
    {
        std::cout << eventos_pendientes[i] << std::endl; // Imprime el evento actual
        server.sin_addr.s_addr = inet_addr(IP); // Configura la dirección IP del servidor
        n = sendto(sockfd, eventos_pendientes[i], MAX_NEVENTOS, 0, (struct sockaddr*)&server, length); // Envía el evento al servidor
        if (n < 0) error("Sendto"); // Si hay un error en el envío, imprime un mensaje de error
    }
    n_eventos_pendientes_envio = 0; // Reinicia el contador de eventos pendientes para envío
}

void agregar_evento(char *evento_id){
    struct timeval current_time; // Estructura para almacenar el tiempo actual

    {
        gettimeofday(&current_time, NULL); // Obtiene el tiempo actual
        std::sprintf(evento_id, "%s %ld.%06ld", timestamp_str, current_time.tv_sec, current_time.tv_usec); // Formatea el evento con timestamp
        numero_eventos++; // Incrementa el contador de eventos
        update_timestep(); // Actualiza el paso de tiempo
    };

    std::sprintf(linea_evento, "RTU1 %02d %s.%03d %d%d %d%d %d %d%d %.2f\n", evento_id, timestamp_str, current_time_usec / 1000, switch1, switch2, PUSH1, PUSH2, status_led_1, LD1, LD2, voltajeadc);
    std::printf("%s", linea_evento); // Imprime el evento

    if (n_eventos_pendientes_envio < MAX_NEVENTOS) {
        std::strcpy(eventos_pendientes[n_eventos_pendientes_envio], evento); // Copia el evento a la lista de eventos pendientes
        n_eventos_pendientes_envio++; // Incrementa el contador de eventos pendientes para envío
    }
}


void update_timestamp(){ // Función para actualizar el timestamp
    struct timeval current_time; // Estructura para almacenar el tiempo actual
    gettimeofday(&current_time, NULL); // Obtiene el tiempo actual
    // Formatea el timestamp con milisegundos
    std::sprintf(timestamp_str, "%s.%03d", timestamp(), current_time.tv_usec / 1000); 
    current_time_usec = current_time.tv_usec; // Almacena los microsegundos actuales
}


void update_timestep(){
    struct timeval current_time; // Estructura para almacenar el tiempo actual
    gettimeofday(&current_time, NULL); // Obtiene el tiempo actual
    // Formatea el timestamp con milisegundos
    std::sprintf(timestamp_str, "%s.%03d", timestamp(), current_time.tv_usec / 1000); 
    current_time_usec = current_time.tv_usec; // Almacena los microsegundos actuales
}

void alarm_low_voltage(){ // Función para la alarma de bajo voltaje
    char mendaje[64];
    static uint8_t last_alarm_status = 0;
    if (voltajeadc < 0.5) {
        agregar_evento(1);
        last_alarm_status = 1;
    }
    else {
        last_alarm_status = 0;
        AlarmHandler alarm(Alarm);
        alarm.turnOff();
        FLGB = 0;
    }
}


void alarm_high_voltage(){ // Función para la alarma de alto voltaje
    char mendaje[64];
    static uint8_t last_alarm_status = 0;
    if (voltajeadc > 2.5) {
        agregar_evento(2);
        last_alarm_status = 1;
    }
    else {
        last_alarm_status = 0;
        AlarmHandler alarm(Alarm);
        alarm.turnOff();
        FLGB = 0;
    }
}



