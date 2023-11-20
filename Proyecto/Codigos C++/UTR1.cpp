#include <wiringPi.h> // Librería para controlar pines GPIO
#include <iostream> // Librería estándar de entrada/salida
#include <thread>   // Librería para utilizar hilos
#include <chrono>   // Librería para utilizar la función sleep_for
#include <cstdio>   // Librería estándar de entrada/salida
#include <cstdlib>  // Librería estándar para funciones generales
#include <ctime>    // Librería para utilizar la función time
#include <cstring>  // Librería para la manipulación de cadenas
#include <cstdint>  // Librería para utilizar tipos de datos enteros
#include <wiringPiSPI.h> // Librería para utilizar el bus SPI
#include <wiringPiI2C.h> // Librería para utilizar el bus I2C
#include <string>   // Librería para utilizar cadenas
#include <sys/socket.h>  // Librería para utilizar sockets
#include <arpa/inet.h>   // Librería para manipular direcciones IP
#include <sys/time.h>   // Librería para utilizar la función gettimeofday
#include <unistd.h>    // Librería que proporciona funciones y constantes específicas de sistemas Unix.
#include <vector>   // Librería para utilizar vectores

#define SPI_CHANNEL	      0	// Canal SPI de la Raspberry Pi, 0 ó 1
#define SPI_SPEED 	1500000	// Velocidad de la comunicación SPI (reloj, en HZ)
                            // Máxima de 3.6 MHz con VDD = 5V, 1.2 MHz con VDD = 2.7V
#define ADC_CHANNEL       0	// Canal A/D del MCP3002 a usar, 0 ó 1
#define MSG_SIZE 301     // Tamaño del mensaje
#define IP "192.168.1.255" // Dirección IP

#define LUZ_1   24  // Pin para la luz 1
#define LUZ_2   25  // Pin para la luz 2
#define BTN1    17   // Pin para el botón 1
#define BTN2    26  // Pin para el botón 2
#define Swtch1  22  // Pin para el switch 1
#define Swtch2  23  // Pin para el switch 2
#define Alarm   16  // Pin para la alarma/buzzer

char timestamp_str[20];

// Variables globales
int switch1, switch2, PUSH1, PUSH2, status_led_1, LD1, LD2;
float voltajeadc = 0; 

const int MAX_NEVENTOS = 100; // Número máximo de eventos
char eventos_pendientes[MAX_NEVENTOS][256]; // Número de eventos pendientes
int n_eventos_pendientes_envio = 0; // Número de eventos pendientes de envío
uint32_t numero_eventos=0; // Número de eventos
uint32_t time_on=0; // Tiempo encendido

int sockfd, n; // Variables para el socket
unsigned int length; 
struct sockaddr_in addr, broadcast_addr; // Estructuras para almacenar las direcciones de los sockets
char buffer[MSG_SIZE]; // Buffer para almacenar el mensaje
int boolval = 1;	

char message[128]; 

uint16_t get_ADC(int channel); //Función para obtener el valor del ADC	

void RTU1(int sockfd, sockaddr_in& addr, socklen_t& length); // Declaración de la función RTU1
void agregar_evento(uint8_t evento_id); // Declaración de la función agregar_evento

void error(const char *msg) // Función para imprimir errores
{
    perror(msg);
    exit(0);
}


class LED { // Clase para controlar LEDs
    public:
        LED(int pin) {
            this->pin = pin;
            pinMode(pin, OUTPUT);
        }

        void on() {
            digitalWrite(pin, HIGH);
        }

        void off() {
            digitalWrite(pin, LOW);
        }

    private:
        int pin;
    };

class Button { // Clase para controlar botones
    public:
        Button(int pin, int eventID) : pin(pin), eventID(eventID) {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_DOWN);
            wiringPiISR(pin, INT_EDGE_FALLING, &Button::isrWrapper); // Configura la interrupción
            instance = this; 
        }

        bool isPressed() {
            return digitalRead(pin) == HIGH;
        }

        void waitForButtonRelease() {
            while (isPressed()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        void isr() { // Función para la interrupción
            agregar_evento(eventID);
        }

    private:
        int pin;
        int eventID;
        static Button* instance; 

        // Wrapper para llamar a la función de interrupción de la clase
        static void isrWrapper() {
            if (instance != nullptr) {
                instance->isr();
            }
        }
};

Button* Button::instance = nullptr; // Inicio de la variable estática

class Button2 { // Clase para controlar botones
    public:
        Button2(int pin, int eventID) : pin(pin), eventID(eventID) {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_DOWN);
            wiringPiISR(pin, INT_EDGE_FALLING, &Button2::isrWrapper); // Configura la interrupción
            instance = this; 
        }

        bool isPressed() {
            return digitalRead(pin) == HIGH;
        }

        void waitForButtonRelease() {
            while (isPressed()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        void isr() { // Función para la interrupción
            agregar_evento(eventID);
        }

    private:
        int pin;
        int eventID;
        static Button2* instance; 

        // Wrapper para llamar a la función de interrupción de la clase
        static void isrWrapper() {
            if (instance != nullptr) {
                instance->isr();
            }
        }
};

Button2* Button2::instance = nullptr; // Inicio de la variable estática

class Switch { // Clase para controlar switches
    public:
        Switch(int pin, int eventID, int& globalSwitch) : pin(pin), eventID(eventID), globalSwitch(globalSwitch) { // Constructor
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_UP);
        }

        void checkState() { // Función para verificar el estado del switch
            int currentState = digitalRead(pin);
            if (currentState != previousState) {

                switch (currentState) { // Verifica si el switch está encendido o apagado
                    case HIGH:
                        agregar_evento(eventID + 1); 
                        globalSwitch = 0; 
                        break;
                    case LOW:
                        agregar_evento(eventID);      
                        globalSwitch = 1;
                        break;
                }
                previousState = currentState; // Actualiza el estado anterior
            }
        }

    private:
        int pin;
        int eventID;
        int& globalSwitch;
        int previousState = HIGH;  // Assuming the initial state is HIGH
};

//Funciones para orden de eventos
void listar_eventos(){
    int i;
    printf("listado eventos\n");
    for (i=0; i< n_eventos_pendientes_envio; i++){ 

        std::cout << eventos_pendientes[i] << std::endl; // Imprime los eventos pendientes

        //Manda los eventos pendientes
        n = sendto(sockfd, eventos_pendientes[i], MAX_NEVENTOS, 0, 
        (struct sockaddr *)&addr, sizeof(struct sockaddr));
	    if(n < 0)
		    std::cerr << "Error en sendto" << std::endl; //Verifica si hubo error     
        }
    n_eventos_pendientes_envio = 0; // Reinicia el número de eventos pendientes de envío
}


void agregar_evento(uint8_t evento_id) { // Función para agregar eventos
    struct timeval current_time; // Estructura para almacenar el tiempo actual
    gettimeofday(&current_time, NULL); // Obtiene el tiempo actual

    struct timeval tv; 
    gettimeofday(&tv, NULL); 
    time_t now = tv.tv_sec;
    struct tm* timeinfo = localtime(&now); // Obtiene la fecha y hora actual
    char timestamp[80];
    strftime(timestamp, 80, "%Y-%m-%d %H:%M:%S", timeinfo); // Formato de la fecha y hora

    numero_eventos++;

    // Formato del mensaje
    std::sprintf(message, "RTU1 %d %s %d %d%d %d%d %d %d%d %0.2f",
             evento_id, timestamp, static_cast<int>(current_time.tv_usec / 1000),
             switch1, switch2, PUSH1, PUSH2, status_led_1, LD1, LD2, voltajeadc);

    std::cout << "Mandando Mensaje: " << message << std::endl; // Imprime el mensaje

    // Se copia el mensaje en el arreglo de eventos pendientes
    if (n_eventos_pendientes_envio < MAX_NEVENTOS) {
        std::strcpy(eventos_pendientes[n_eventos_pendientes_envio], message);
        n_eventos_pendientes_envio++;
    }

};

int main(int argc, char *argv[]) { // Función principal
    wiringPiSetupGpio(); // Inicializa la librería wiringPi
    pinMode(Alarm, OUTPUT); // Configura el pin de la alarma como salida
    uint16_t ADCvalue; // Variable para almacenar el valor del ADC
    int last_alarm_statusH=0; // Variable para almacenar el estado de la alarma
    int last_alarm_statusL=0; 

    // Configuración del bus SPI
    if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
        std::cerr << "Error setting up SPI" << std::endl;
        return 1;  // Verifica si hubo error
    }
    //Se crean los objetos de las clases
    LED luz1(LUZ_1); 
    LED luz2(LUZ_2);
    Button btn1(BTN1, 1);
    Button2 btn2(BTN2, 2);
    Switch swtch1(Swtch1,3,switch1);
    Switch swtch2(Swtch2,5, switch2);
    digitalWrite(Alarm, LOW); 
    
   if(argc != 2) // Verifica si se ingresó el puerto
	{
	    std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        exit(1);
	}


    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Crea socket UDP sin conexión
    if(sockfd < 0)
		std::cerr << "Opening socket" << std::endl; // Verifica si hubo error al crear el socket

	addr.sin_family = AF_INET; // Configura la familia de direcciones
	addr.sin_port = htons(atoi(argv[1]));	 // Configura el puerto

    length = sizeof(addr); // Tamaño de la dirección

	if(bind(sockfd, (struct sockaddr *)&addr, length) < 0) // Asigna el socket
		std::cerr << "Error binding socket." << std::endl;

	// Cambia los permisos del socket para permitir el broadcast
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
  		std::cerr << "Error setting socket options\n" << std::endl;
    
    addr.sin_addr.s_addr = inet_addr(IP); // Configura la dirección IP

    std::thread rtuThread(RTU1, sockfd, std::ref(addr), std::ref(length)); // Crea el hilo de la función RTU1

    while (true) { // Ciclo infinito
        time_on++; // Incrementa el tiempo encendido
        ADCvalue =get_ADC(ADC_CHANNEL); // Obtiene el valor del ADC
        PUSH1 = 0; 
        PUSH2 = 0; 
        if (btn1.isPressed()) { // Verifica si el botón 1 está presionado
            PUSH1 = 1;      
        }
        if (btn2.isPressed()) { // Verifica si el botón 2 está presionado
            PUSH2 = 1;
        }
        swtch1.checkState(); // Verifica el estado del switch 1
        swtch2.checkState(); // Verifica el estado del switch 2

        //Condicionales para la alarma con el ADC
        if (voltajeadc < 0.5) {
            if(last_alarm_statusL == 0){
                agregar_evento(15);
                last_alarm_statusL = 1;
                digitalWrite(Alarm, HIGH);
            }
        } else if (voltajeadc > 2.5) {
            if(last_alarm_statusH == 0){
                agregar_evento(16);
                last_alarm_statusH = 1;
                digitalWrite(Alarm, HIGH);
            }
        }else if ((voltajeadc < 2.5)&&(voltajeadc > 0.5)){
            digitalWrite(Alarm, LOW);
            last_alarm_statusL = 0;
            last_alarm_statusH = 0;
        }

        if ((time_on %15) == 0){ //Cada 15 segundos se agrega un evento
            agregar_evento (0);
        }
        fflush(stdout);
        if ((time_on % 15)==0){ //Cada 15 segundos se imprime el número de eventos
           listar_eventos(); // Llama a la función para listar los eventos
            if (n_eventos_pendientes_envio >0){ // Verifica si hay eventos pendientes de envío
                std::cout << "Hay eventos pendientes de envío" << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Espera 1 segundo
    }

    rtuThread.join(); // Espera a que termine el hilo
    return 0;
}

void RTU1(int sock, sockaddr_in& addres, socklen_t& leng){ // Función para la RTU1
    LED luz1(LUZ_1);
    LED luz2(LUZ_2);
    int i2cAddress = 0x08; // Dirección del dispositivo I2C

    // Comunicación I2C abierta
    int i2cHandle = wiringPiI2CSetup(i2cAddress);
    if (i2cHandle == -1) {
        std::cerr << "Error opening I2C communication." << std::endl; // Verifica si hubo error
        return;
    }

    while (1){ 

        memset(buffer, 0, MSG_SIZE); // Limpia el buffer
        n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr*)&addres, &leng); // Recibe el mensaje
        if (n < 0){
            perror("ERROR reading from socket (revfrom)"); // Verifica si hubo error
        } 
        std::cout << "Message recibido (" << n << " bytes): " << buffer << std::endl; // Imprime el mensaje recibido


        if((std::strcmp(buffer, "RTU1 LED1 1") == 0)){ // Verifica si el mensaje es para encender el LED 1
            char mensaje[60];
            luz1.on();
            LD1 = 1;
            agregar_evento(9); // Agrega el evento 9
        }
        if((std::strcmp(buffer, "RTU1 LED1 0") == 0)){ // Verifica si el mensaje es para apagar el LED 1
            char mensaje[60];
            luz1.off();
            LD1 = 0;
            agregar_evento(10);
        }
        if((std::strcmp(buffer, "RTU1 LED2 1") == 0)){ // Verifica si el mensaje es para encender el LED 2
            char mensaje[60];
            luz2.on();
            LD2 = 1;
            agregar_evento(11);
        }
        if((std::strcmp(buffer, "RTU1 LED2 0") == 0)){ // Verifica si el mensaje es para apagar el LED 2
            char mensaje[60];
            luz2.off();
            LD2 = 0;
            agregar_evento(12);
        }
        if((std::strcmp(buffer, "RTU1 LEDIoT 1") == 0)){ // Verifica si el mensaje es para encender el LED IoT
            char mensaje[60];
            char data1[] = "A"; 
            status_led_1 = 1;
            agregar_evento(13);
            write(i2cHandle, data1, sizeof(data1)); // Escribe en el bus I2C para mandar el mensaje al Arduino
        }
        if((std::strcmp(buffer, "RTU1 LEDIoT 0") == 0)){ // Verifica si el mensaje es para apagar el LED IoT
            char mensaje[60]; 
            char data0[] = "B";
            status_led_1 = 0;
            agregar_evento(14);
            write(i2cHandle, data0, sizeof(data0)); // Escribe en el bus I2C para mandar el mensaje al Arduino
        }
    }
    close(i2cHandle); // Cierra la comunicación I2C
}

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
	voltajeadc=(resultado*3.3)/1023.0;
	return(resultado);
}

