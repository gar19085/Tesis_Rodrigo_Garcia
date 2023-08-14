#include <iostream>
#include <fstream>
#include <string>
#include <vector>

constexpr int MAX_LETRAS = 100;
constexpr int MAX_CADENAS = 60;
constexpr char ORIGINAL[] = "Prueba.txt";
constexpr char PRIMERO[] = "primero.txt";
constexpr char SEGUNDO[] = "segundo.txt";

int main()
{
    std::ifstream fp_original(ORIGINAL); 
    if (!fp_original.is_open())
    {
        std::cerr << "Error al abrir el archivo." << std::endl;
        exit(1);
    }

    std::vector<std::string> StringArray;
    std::string line;
    while (std::getline(fp_original, line))
    {
        StringArray.push_back(line);
        std::cout << line << std::endl;
    }
    fp_original.close(); 

    std::cout << "\nNúmero de líneas leídas: " << StringArray.size() << std::endl;

    std::ofstream fp_primero(PRIMERO);
    std::ofstream fp_segundo(SEGUNDO);

    for (size_t i = 0; i < StringArray.size(); i++)
    {
        if (i % 2 == 0)
            fp_primero << StringArray[i] << std::endl; 
        else 
            fp_segundo << StringArray[i] << std::endl; 
    }

    fp_primero.close();
    fp_segundo.close();

    std::cout << "\nListo..." << std::endl;

    return 0;
}