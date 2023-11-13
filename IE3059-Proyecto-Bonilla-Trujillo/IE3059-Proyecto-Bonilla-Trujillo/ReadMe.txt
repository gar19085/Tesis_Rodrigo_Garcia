-----------------------------------------------------------
PASOS PARA PODER EJECUTAR DE MANER ÓPTIMA LOS PROGRAMAS:	
-----------------------------------------------------------

--------------
HISTORIADOR
--------------
1. Abrir la consola
2. Ir a ubicacion donde se encuentra el programa .c
3. Correr el comando "ipconfig" o "ifconfig" (Windows o Linux, respectivamente)
4. Anotar la dirección IPv4 del apartado "Adaptador de LAN inalámbrica WIFI"
5. Compilar el programa de la forma "gcc Historiador.c -o historiador -lpthread"
6. Correr el ejecutable .exe o .a (Windows o Linux, respectivamente) seguido por el numero de puerto, de la forma "./historiador 2000"
7. Ejecutar el programa correspondiente el UTR

--------------
UTR
--------------
1. Abrir la consola
2. Ir a ubicacion donde se encuentra el programa .c
3. Compilar el programa de la forma "gcc UTR.c -o utr-lpthread -lwiringPi"
4. Correr el ejecutable .exe o .a (Windows o Linux, respectivamente) seguido por la dirección IP del historiador y el numero de puerto, de la forma "./muestreo 192.168.1.12 2000"