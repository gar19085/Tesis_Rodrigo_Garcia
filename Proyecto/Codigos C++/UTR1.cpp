/*
 ============================================================================
 Nombre: UTR1.cpp
 Autor:  Rodrigo José García Ambrosy
 Proyecto
 ============================================================================
 */
#include <wiringPi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <wiringPiSPI.h>
#include <string>
#include<sys/socket.h>
#include<arpa/inet.h>


#define SPI_CHAN 0
#define SPI_SPEED 1500000

#define ADC_CHANNEL 0
#define MSG_SIZE 40
#define IP "10.0.0.255"

#define LUZ_1   20
#define LUZ_2   21
#define BTN1    5
#define BTN2    17
#define Switch1 19
#define Switch2 26
#define Alarm   13
#define Arduino 12


uint16_t get_ADC(int channel);	
float voltajeadc=0;
uint32_t time_on=0;
char timestamp_str [26];
uint32_t numero_eventos=0;
#define MAX_NEVENTOS 300
char eventos_pendientes[MAX_NEVENTOS+1][64];
uint16_t n_eventos_pendientes_envio =0;
uint8_t boton1 =0;
uint8_t boton2 =0;
uint8_t  switch1=0;
uint8_t  switch2=0;
 uint8_t   Myswitch1 =0;
uint16_t tiempo_buzzer_on = 0;
uint8_t status_led_1= 0;
char linea_status [128];
char linea_evento[128];

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


class LightController {
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

class ButtonHandler {
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

class SwitchHandler {
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

class AlarmHandler {
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

void error(const char *msg) {
    perror(msg);
    exit(1);
}

std::string timestamp() {
    std::time_t t = std::time(nullptr);
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%T", std::localtime(&t));
    return mbstr;
}

std::string update_timestep() {
    std::time_t t = std::time(nullptr);
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%T", std::localtime(&t));
    return mbstr;
}

int main() {
    wiringPiSetupGpio();

    LightController luz1Ctrl(LUZ_1);
    LightController luz2Ctrl(LUZ_2);
    ButtonHandler button1(BTN1);
    ButtonHandler button2(BTN2);
    SwitchHandler switch1(Switch1);
    SwitchHandler switch2(Switch2);
    AlarmHandler alarm(Alarm);
    A = (1/10900)/2;

    wiringPiISR(LUZ_1, INT_EDGE_BOTH, (void*)&PUSH1);
    wiringPiISR(LUZ_2, INT_EDGE_BOTH, (void*)&PUSH2);
    wiringPiISR(Switch1, INT_EDGE_BOTH, (void*)&SWT1);
    wiringPiISR(Switch2, INT_EDGE_BOTH, (void*)&SWT2);
    wiringPiISR(Arduino, INT_EDGE_BOTH, (void*)&iotarduino);

    std::srand(time(NULL));

    if (wiringPiSPISetup(SPI_CHAN, SPI_SPEED) < 0) {
        std::cout << "No se pudo inicializar el SPI" << std::endl;
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    length = sizeof(server);
    bzero(&server,length);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=inet_addr(IP);
    server.sin_port=htons(atoi(argv[1]));
    if (bind(sockfd,(struct sockaddr *)&server,length)<0) error("ERROR on binding");
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval) < 0) error("ERROR setting socket options");

    uint16_t ADCvalue;

    while (1){
        time_on++;

        update_timestamp();
        ADCvalue = get_ADC(ADC_CHANNEL);
    
    if ((time_on %20) == 0)
    {
        agregar_evento(0);
    }

    alarm_low_voltage();
    alarm_high_voltage();

    if(voltajeadc > 2.5){
        alarm.turnOn();
    }
    if(voltajeadc < 0.5){
        alarm.turnOn();
    }
    else if (voltajeadc > 0.5 && voltajeadc < 2.5)
    {
        alarm.turnOff();
    }
        
    std::cout << std::flush;
    if ((time_on % 20) == 0)
    {
        listar_eventos();
        if (n_eventos_pendientes_envio > 0)
        {
            std::cout << "Enviando eventos pendientes" << std::endl;
            listar_eventos();
        }           
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}


void RTU1(void*ptr){
    while (1)
    {
        memset(buffer, 0, MSG_SIZE);
        n = recvfrom(sockfd, buffer, MSG_SIZE, 0, (struct sockaddr*)&addr, &length);
        if (n < 0) error("ERROR in recvfrom");
        std::cout << "Received a datagram. It says: " << buffer << std::endl;

        if (n < 0) error("ERROR in sendto");
        
        if((std::strcmp(buffeer, "RTU1 LED1 ON") == 0)){
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
        if ((std::strcmp(buffer, "RTU1 LED1 OFF") == 0))
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
        if((std::strcmp(buffeer, "RTU1 LED2 ON") == 0)){
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
        if((std::strcmp(buffeer, "RTU1 LED2 OFF") == 0)){
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

uint16_t get_ADC(int channel){
    uint8_t spiData[2];
    uint16_t resultado;

    if((channel > 1) || (channel < 0)){
        channel = 0;
    }

    spiData[0] = 0b01101000 | (channel << 4);
    spiData[1] = 0;

    wiringPiSPIDataRW(SPI_CHAN, spiData, 2);

    resultado = ((spiData[0]) << 8) | spiData[1];

    voltajeadc = (resultado * 3.3) / 1023;

    return resultado;
}


void Button1(void){
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

void Button2(void){
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

void SWT1(void){
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

void SWT2(void){
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

void iotarduino(void){
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


void listar_eventos(){
    int i;

    for (i = 0; i < n_eventos_pendientes_envio; i++)
    {
        std::cout << eventos_pendientes[i] << std::endl;
        server.sin_addr.s_addr = inet_addr(IP);
        n = sendto(sockfd, eventos_pendientes[i], MAX_NEVENTOS, 0, (struct sockaddr*)&server, length);
        if (n < 0) error("Sendto");
    }
    n_eventos_pendientes_envio = 0;
}

void agregar_evento(char *evento_id){
    struct timeval current_time;
    {
        gettimeofday(&current_time, NULL);
        std::sprintf(evento_id, "%s %ld.%06ld", timestamp_str, current_time.tv_sec, current_time.tv_usec);
        numero_eventos++;
        update_timestep();
    };
    std::sprintf(linea_evento, "RTU1 %02d %s.%03d %d%d %d%d %d %d%d %.2f\n", evento_id, timestamp_str, current_time_usec / 1000, switch1, switch2, PUSH1, PUSH2, status_led_1, LD1, LD2, voltajeadc);
    std::printf("%s", linea_evento);

    if (n_eventos_pendientes_envio < MAX_NEVENTOS) {
        std::strcpy(eventos_pendientes[n_eventos_pendientes_envio], evento);
        n_eventos_pendientes_envio++;
    }
}


void update_timestamp(){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    std::sprintf(timestamp_str, "%s.%03d", timestamp(), current_time.tv_usec / 1000);
    current_time_usec = current_time.tv_usec;
}


void update_timestep(){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    std::sprintf(timestamp_str, "%s.%03d", timestamp(), current_time.tv_usec / 1000);
    current_time_usec = current_time.tv_usec;
}


void alarm_low_voltage(){
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


void alarm_high_voltage(){
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



