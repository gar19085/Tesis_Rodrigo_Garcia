/*
 * IE3059lab7.h
 * IE3059 - Electrónica Digital 3
 * Autor: Luis Alberto Rivera
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Para el lab 7, necesitará copiar 3 archivos a la tarjeta SD del Raspberry Pi:
 - El archivo de encabezado "IE3059lab7.h" (este archivo)
 - El ejecutable "IE3059lab7" (para la RPi3B+), "IE3059lab7_RPi3B" o "IE3059lab7_RPi4".
 - La librería estática "libIE3059lab7.a" (para la RPi3B+), "libIE3059lab7_RPi3B.a" o
   "libIE3059lab7_RPi4.a"
 
 Por conveniencia, copie los archivos en el directorio donde estarán sus programas
 (aunque pueden copiarse donde prefiera).
 
 Para poder usar las funciones check_button y clear_button en sus programas,
 necesita incluir este archivo: #include "IE3059lab7.h"
 
 Al compilar sus programas, necesitarán enlazar la libería estática. 
 Para ello, compile así (usando la librería adecuada según la RPi que use):
	gcc miprograma.c -o miprograma libIE3059lab7.a
		o
	gcc miprograma.c -o miprograma -L/ruta/al/directorio_donde_está_la_lib -lIE3059lab7
	
 Si usa funciones de wiringPi, incluya también -lwiringPi al compilar.
 Si usa pthreads, no olvide incluir también -lpthread.
 
 Si usa Eclipse, asegúrese de incluir las liberías en las configuraciones
 (C/C++ Build > Settings > GCC C Linker).
 La librería estática se incluye en "Other objects", dentro de la opción "Miscellaneous"
 del Linker.
 
 Antes de correr "miprograma", debe correr el ejecutable IE3059lab7 (IE3059lab7_RPi3B
 o IE3059lab7_RPi4, según la RPi que use). Al correrlo, debe ingresar el puerto en
 que se ha conectado el push button. Se asume el pinout GPIO (también llamado RPi),
 no el pinout wiringPi.
	./IE3059lab7 #puerto
	
 Sugerencia: abra dos terminales, y corra cada programa en una terminal distinta.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

int check_button(void);
void clear_button(void);
