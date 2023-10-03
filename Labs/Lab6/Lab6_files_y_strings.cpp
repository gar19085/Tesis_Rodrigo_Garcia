/*
 ============================================================================
 Nombre: Lab6_files_y_strings.cpp
 Autor:  Rodrigo José García Ambrosy
 ============================================================================
 */

#include <iostream> //Librería estándar de entrada/salida
#include <fstream> //Librería para manejo de archivos
#include <string>  //Librería para la manipulación de cadenas
#include <vector>  //Librería para el uso de vectores

constexpr int MAX_LETRAS = 100; //Máximo de letras
constexpr int MAX_CADENAS = 60; //Máximo de cadenas
constexpr char ORIGINAL[] = "Prueba.txt"; //Archivo original
constexpr char PRIMERO[] = "primero.txt"; //Primer archivo
constexpr char SEGUNDO[] = "segundo.txt"; //Segundo archivo

int main() //Función principal
{
    std::ifstream fp_original(ORIGINAL); //Se abre el archivo original
    if (!fp_original.is_open()) //Condicional para verificar si se abrio el archivo
    {
        std::cerr << "Error al abrir el archivo." << std::endl;
        exit(1);
    }
    //Se crea un vector para almacenar las lineas leidas
    std::vector<std::string> StringArray; 
    std::string line; //Se crea una variable para almacenar las lineas leidas
    while (std::getline(fp_original, line)) //Se lee una linea del archivo
    {
        StringArray.push_back(line); //Se agrega una linea en el vector
        std::cout << line << std::endl; //Se imprime la salida
    }
    fp_original.close(); //Se cierra el archivo original

    std::cout << "\nNúmero de líneas leídas: " << StringArray.size() << std::endl;

    std::ofstream fp_primero(PRIMERO); //Se abre el primer archivo
    std::ofstream fp_segundo(SEGUNDO); //Se abre el segundo archivo

    //Funcion para separar las lineas en dos archivos diferentes
    for (size_t i = 0; i < StringArray.size(); i++)
    {
        if (i % 2 == 0) //Se verifica si el indice es par
            fp_primero << StringArray[i] << std::endl; //Se escribe en el primer archivo
        else 
            fp_segundo << StringArray[i] << std::endl; //Se escribe en el segundo archivo
    }

    fp_primero.close();//Se cierran los archivos de salida
    fp_segundo.close();

    std::cout << "\nListo..." << std::endl;

    return 0; 
}