#include <wiringPi.h> // Librería para controlar pines GPIO
#include <iostream> // Librería estándar de entrada/salida
#include <thread>   // Librería para utilizar hilos
#include <chrono>   // Librería para utilizar la función sleep_for
#include <cstdio>   // Librería estándar de entrada/salida
#include <cstdlib>  // Librería estándar para funciones generales
#include <ctime>    // Librería para utilizar la función time
#include <cstring>  // Librería para la manipulación de cadenas
#include <cstdint>
#include <wiringPiSPI.h> // Librería para utilizar el bus SPI
#include <wiringPiI2C.h> // Librería para utilizar el bus I2C
#include <string>   // Librería para utilizar cadenas
#include <sys/socket.h>  // Librería para utilizar sockets
#include <arpa/inet.h>   // Librería para manipular direcciones IP
#include <sys/time.h>
#include <unistd.h>
#include <vector>

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
#define Alarm   14  // Pin para la alarma
#define Arduino 16  // Pin para el Arduino

// Global variables to store the state of the LEDs, buttons, and switches
char timestamp_str[20];

int switch1, switch2, PUSH1, PUSH2, status_led_1, LD1, LD2;
float voltajeadc = 0; 

const int MAX_NEVENTOS = 100; // Adjust the value based on your requirements
char eventos_pendientes[MAX_NEVENTOS][256]; // Assuming a fixed size for the array of C-style strings
int n_eventos_pendientes_envio = 0;
uint32_t numero_eventos=0;
uint32_t time_on=0;

int sockfd, n;
unsigned int length;
struct sockaddr_in addr, broadcast_addr;
char buffer[MSG_SIZE];
int boolval = 1;	

char message[128];

uint16_t get_ADC(int channel);	

void RTU1(int sockfd, sockaddr_in& addr, socklen_t& length);
void agregar_evento(uint8_t evento_id);

void error(const char *msg) // Función para imprimir errores
{
    perror(msg);
    exit(0);
}


class LED {
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

class Button {
    public:
        Button(int pin, int eventID) : pin(pin), eventID(eventID) {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_DOWN);
            wiringPiISR(pin, INT_EDGE_FALLING, &Button::isrWrapper);
            instance = this; // Set the instance to this object
        }

        bool isPressed() {
            return digitalRead(pin) == HIGH;
        }

        void waitForButtonRelease() {
            while (isPressed()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        void isr() {
            agregar_evento(eventID);
        }

    private:
        int pin;
        int eventID;
        static Button* instance; // Static member to store the instance

        // Static wrapper function to call the non-static isr function
        static void isrWrapper() {
            if (instance != nullptr) {
                instance->isr();
            }
        }
};

Button* Button::instance = nullptr; // Initialize the static member

class Button2 {
    public:
        Button2(int pin, int eventID) : pin(pin), eventID(eventID) {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_DOWN);
            wiringPiISR(pin, INT_EDGE_FALLING, &Button2::isrWrapper);
            instance = this; // Set the instance to this object
        }

        bool isPressed() {
            return digitalRead(pin) == HIGH;
        }

        void waitForButtonRelease() {
            while (isPressed()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        void isr() {
            agregar_evento(eventID);
        }

    private:
        int pin;
        int eventID;
        static Button2* instance; // Static member to store the instance

        // Static wrapper function to call the non-static isr function
        static void isrWrapper() {
            if (instance != nullptr) {
                instance->isr();
            }
        }
};

Button2* Button2::instance = nullptr; // Initialize the static member

class Switch {
    public:
        Switch(int pin, int eventID, int& globalSwitch) : pin(pin), eventID(eventID), globalSwitch(globalSwitch) {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_UP);
        }

        void checkState() {
            int currentState = digitalRead(pin);
            if (currentState != previousState) {
                // State change detected
                switch (currentState) {
                    case HIGH:
                        agregar_evento(eventID + 1);  // Switch turned OFF
                        globalSwitch = 0;
                        break;
                    case LOW:
                        agregar_evento(eventID);      // Switch turned ON
                        globalSwitch = 1;
                        break;
                }
                previousState = currentState;
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
        //printf("%d = %s\n ", i, eventos_pendientes[i]);

        std::cout << eventos_pendientes[i] << std::endl;

        n = sendto(sockfd, eventos_pendientes[i], MAX_NEVENTOS, 0,
        (struct sockaddr *)&addr, sizeof(struct sockaddr));
	    if(n < 0)
		    std::cerr << "Error en sendto" << std::endl;        
        }
    n_eventos_pendientes_envio = 0;
}


void agregar_evento(uint8_t evento_id) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm* timeinfo = localtime(&now);
    char timestamp[80];
    strftime(timestamp, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    numero_eventos++;

    // Format the event string using sprintf
    std::sprintf(message, "RTU1 %d %s %d%d %d%d %d %d%d %0.2f",
             evento_id, timestamp, 
             switch1, switch2, PUSH1, PUSH2, status_led_1, LD1, LD2, voltajeadc);

    // Print the event string
    std::cout << "Sending message: " << message << std::endl;

    // If there is space in the array, copy the event string
    if (n_eventos_pendientes_envio < MAX_NEVENTOS) {
        std::strcpy(eventos_pendientes[n_eventos_pendientes_envio], message);
        n_eventos_pendientes_envio++;
    }
    //memset(buffer, 0, MSG_SIZE);

    //if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0) {
    //    std::cerr << "Error sending message" << std::endl;
    //    return;
    //}

};

int main(int argc, char *argv[]) {
    wiringPiSetupGpio();
    pinMode(Alarm, OUTPUT);
    uint16_t ADCvalue;
    int last_alarm_statusH=0;
    int last_alarm_statusL=0;
        // Add SPI setup
    if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
        std::cerr << "Error setting up SPI" << std::endl;
        return 1;  // Exit with an error code
    }
    
    LED luz1(LUZ_1);
    LED luz2(LUZ_2);
    Button btn1(BTN1, 1);
    Button2 btn2(BTN2, 2);
    Switch swtch1(Swtch1,3,switch1);
    Switch swtch2(Swtch2,5, switch2);
    digitalWrite(Alarm, LOW);
    
   if(argc != 2)
	{
	    std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        exit(1);
	}


    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if(sockfd < 0)
		std::cerr << "Opening socket" << std::endl;

	addr.sin_family = AF_INET; // Configura la familia de direcciones
	addr.sin_port = htons(atoi(argv[1]));	 // Configura el puerto

    length = sizeof(addr);

	if(bind(sockfd, (struct sockaddr *)&addr, length) < 0)
		std::cerr << "Error binding socket." << std::endl;

	// change socket permissions to allow broadcast
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
  		std::cerr << "Error setting socket options\n" << std::endl;
    
    addr.sin_addr.s_addr = inet_addr(IP); // Configura la dirección IP

    std::thread rtuThread(RTU1, sockfd, std::ref(addr), std::ref(length));

    while (true) {
        time_on++;
        ADCvalue =get_ADC(ADC_CHANNEL);
        std::cout << ADCvalue << std::endl;
        PUSH1 = 0;
        PUSH2 = 0;
        if (btn1.isPressed()) {
            PUSH1 = 1;      
        }
        if (btn2.isPressed()) {
            PUSH2 = 1;
        }
        swtch1.checkState();
        swtch2.checkState();

        // Check voltage and control the speaker accordingly

        if (voltajeadc < 0.5) {
            if(last_alarm_statusL == 0){
            // Below 0.5V, turn on the speaker
                agregar_evento(15);
                last_alarm_statusL = 1;
                digitalWrite(Alarm, HIGH);
            }
        } else if (voltajeadc > 2.5) {
            // Above 2.5V, turn off the speaker
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

        if ((time_on %15) == 0){   
            agregar_evento (0);
        }
        fflush(stdout);
        if ((time_on % 15)==0){
           // printf("Eventos pendientes:\n");
           listar_eventos();
            if (n_eventos_pendientes_envio >0){
           
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    rtuThread.join();
    return 0;
}

void RTU1(int sock, sockaddr_in& addres, socklen_t& leng){
    LED luz1(LUZ_1);
    LED luz2(LUZ_2);
        // Replace with the I2C address of your Arduino Nano IoT
    int i2cAddress = 0x08;

    // Open I2C communication
    int i2cHandle = wiringPiI2CSetup(i2cAddress);
    if (i2cHandle == -1) {
        std::cerr << "Error opening I2C communication." << std::endl;
        return;
    }

    while (1){

        memset(buffer, 0, MSG_SIZE);
        n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr*)&addres, &leng);
        if (n < 0){
            perror("ERROR reading from socket (revfrom)");
        } 
        std::cout << "Message received (" << n << " bytes): " << buffer << std::endl;


        if((std::strcmp(buffer, "RTU1 LED1 1") == 0)){
            char mensaje[60];
            luz1.on();
            LD1 = 1;
            agregar_evento(9);
            std::sprintf(mensaje, "LED indicador 1 encendido");
            std::cout << std::flush;
        }
        if((std::strcmp(buffer, "RTU1 LED1 0") == 0)){
            char mensaje[60];
            luz1.off();
            LD1 = 0;
            agregar_evento(10);
            std::sprintf(mensaje, "LED indicador 1 apagado");
            std::cout << std::flush;
        }
        if((std::strcmp(buffer, "RTU1 LED2 1") == 0)){
            char mensaje[60];
            luz2.on();
            LD2 = 1;
            agregar_evento(11);
            std::sprintf(mensaje, "LED indicador 2 encendido");
            std::cout << std::flush;
        }
        if((std::strcmp(buffer, "RTU1 LED2 0") == 0)){
            char mensaje[60];
            luz2.off();
            LD2 = 0;
            agregar_evento(12);
            std::sprintf(mensaje, "LED indicador 2 apagado");
            std::cout << std::flush;
        }
        if((std::strcmp(buffer, "RTU1 LEDIoT 1") == 0)){
            char mensaje[60];
            char data1[] = "A";
            status_led_1 = 1;
            agregar_evento(13);
            write(i2cHandle, data1, sizeof(data1));
            std::sprintf(mensaje, "LED Iot encendido");
            std::cout << std::flush;
        }
        if((std::strcmp(buffer, "RTU1 LEDIoT 0") == 0)){
            char mensaje[60];
            char data0[] = "B";
            status_led_1 = 0;
            agregar_evento(14);
            write(i2cHandle, data0, sizeof(data0));
            std::sprintf(mensaje, "LED Iot encendido");
            std::cout << std::flush;
        }
    }
    close(i2cHandle);
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

