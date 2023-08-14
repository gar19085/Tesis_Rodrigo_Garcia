/* 	Nombre     : 	funciones_cadenas.c
	Autor      : 	Luis A. Rivera
	Descripción: 	Ilustración del uso de diversas funciones para manipulación
                    de cadenas y otras cosas (rand, switch-case, atoi).		 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAX_NUM  100
#define MSG_SIZE  60
#define CADENA_CMP "HoLa\n"   // Puede cambiarla

// Prototipos
void ej_strtok(char *);
int menu(void);

// Se puede ingresar como argumento el valor máximo para números
// aleatorios que se generarán en una de las opciones del menú.
int main(int argc, char *argv[])
{
	int max_num = MAX_NUM, opcion, compara, n;
	char cad_ingresos1[MSG_SIZE], cad_ingresos2[MSG_SIZE];

	if(argc > 2)
	{
		printf("Uso: %s máx_num_aleatorio\n", argv[0]);
		exit(1);
	}
	
	if(argc == 2)
	{
		max_num = atoi(argv[1]);
		if(max_num < 2)
			max_num = MAX_NUM;	// Por si se ingresa algo inválido.
	}

	srand(time(NULL));	// La semilla de números aleatorios cambia cada vez.

	do
	{
		opcion = menu();

		switch(opcion)
		{
			case 0:
				puts("\nAdiós...\n\n");
				break;

			case 1:  // strcmp
				puts("\nIngrese la cadena que quiere comparar con "CADENA_CMP"");
				fflush(stdout);
				//gets(cad_ingresos1);
				fgets(cad_ingresos1, MSG_SIZE-1, stdin);  // se prefiere fgets sobre gets

				compara = strcmp(cad_ingresos1, CADENA_CMP);

				if(compara < 0)
					puts("\nSu cadena es 'menor'.\n");
				else if(compara > 0)
					puts("\nSu cadena es 'mayor'.\n");
				else
					puts("\nSu cadena es exactamente igual.\n");	
				
				break;

			case 2:  // strncmp
				puts("\nIngrese la primera cadena:");
				fflush(stdout);
				//gets(cad_ingresos1);
				fgets(cad_ingresos1, MSG_SIZE-1, stdin);  // se prefiere fgets sobre gets
				puts("\nIngrese la segunda cadena:");
				fflush(stdout);
				//gets(cad_ingresos2);
				fgets(cad_ingresos2, MSG_SIZE-1, stdin);  // se prefiere fgets sobre gets
				puts("\nIngrese el número de caracteres a comparar (> 0):");
				fflush(stdout);
				scanf("%d%*c", &n);  // El %*c es para ignorar el "enter".
				
				if(n < 1)
					n = 1;
								
				if(strncmp(cad_ingresos1, cad_ingresos2, n) == 0)
					printf("\nLos primeros %d caracteres de sus cadenas son iguales.\n", n);
				else
					printf("\nLos primeros %d caracteres de sus cadenas no son iguales.\n", n);

				break;

			case 3:  // strtok, strcpy
				puts("\nIngrese una cadena con al menos tres '.' o espacios en blanco:\n");
				fflush(stdout);
				//gets(cad_ingresos1);
				fgets(cad_ingresos1, MSG_SIZE-1, stdin);  // se prefiere fgets sobre gets
				ej_strtok(cad_ingresos1);

				break;

			case 4:  // sprintf, rand
				n = rand() % (max_num+1);

				memset(cad_ingresos1, 0, MSG_SIZE);	// "Limpia" la cadena.

				// Se pueden usar los mismos especificadores que printf (%s, %f, etc.).
				sprintf(cad_ingresos1, "El número aleatorio entre 0 y %d fue: %d",
                        max_num, n);
				puts(cad_ingresos1);

				break;

			default:
				puts("\nOpción no válida\n");
				break;
		}
	} while(opcion != 0);

	return 0;
}

// Función para desplegar un menú de opciones.
// Devuelve un entero con la opción seleccionada por el usuario.
int menu(void)
{
	char opcion[10]; // No se necesita una cadena. Es para ilustrar el uso de atoi.
	printf("\n\nIngrese la opción deseada:\n"
           "0 - Salir del programa.\n"
		   "1 - Ejemplo de strcmp.\n"
		   "2 - Ejemplo de strncmp.\n"
		   "3 - Ejemplo de strtok y strcpy.\n"
		   "4 - Ejemplo de sprintf.\n\n");
	
	fflush(stdout);
	//gets(opcion);
	fgets(opcion, 9, stdin);  // se prefiere fgets sobre gets

	return(atoi(opcion));
}


// Ejemplo que ilustra el uso de la función strtok (divide la cadena
// en piezas o "tokens").
// El delimitador o delimitadores pueden ser cualquier caracter. Aquí se
// usa ". " (punto y espacio), pero se pueden usar otros. Por ejemplo, si se
// usa "+ $*", cualquiera de los cuatro caracteres (incluyendo el espacio
// en blanco) sirve para separar los tokens. Puede usarse sólo uno (ej. "-").
// En esta función se asume que la cadena de entrada tiene por lo menos tres
// de los delimitadores (y otros caracteres antes, después y entre ellos).
// Si eso no se cumple, se imprimen (null). Pruebe distintas combinaciones.
// En general, se debe tener cuidado de no descopmoner "de más". Cuando
// se llegue a un puntero NULL, ya no se debe seguir.
void ej_strtok(char *cadena_original)
{
	char *token, copia[MSG_SIZE];

	strcpy(copia, cadena_original);	// por si se quiere mantener la cadena
								// original, ya que con strtok, se "perderá".
	
	token = strtok(cadena_original, ". "); // Pruebe cambiar delimitador(es).
	printf("\nOriginal (perdida): %s, Copia: %s\n"
           "token 1: %s\n", cadena_original, copia, token);

	for(int i = 2; i <= 4; i++)
	{
		token = strtok(NULL, ". "); // Podría(n) ser distinto(s) cada vez.
		printf("token %d: %s\n", i, token);
	}

	return;
}
