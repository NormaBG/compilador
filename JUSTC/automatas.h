#ifndef AUTOMATAS_H
#define AUTOMATAS_H
#define MAX_TABLA_SIMBOLOS 100

// Enumeración para los tipos de tokens
typedef enum{
    TOKEN_IDENTIFICADOR = 1,
    TOKEN_SIMBOLO = 2, 
    TOKEN_NUM = 3,
    TOKEN_PALABRA_RESERVADA = 4
} tiposdetoken;

//tabla de simbolos
typedef struct {
    char lexema[100]; // El lexema del identificador
    char tipo[20];    // "int", "float", "función", etc.
    int valor;        // El valor
} EntradaTablaSimbolos;

EntradaTablaSimbolos tablaSimbolos[MAX_TABLA_SIMBOLOS]; // La tabla de símbolos como una matriz

// Arreglo para palabras reservadas
char* palabrasReservadas[] = {
    "inicio",
    "INICIO",
    "Inicio",
    "fin",
    "FIN",
    "Fin",
    "arit",
    "si",
    "entonces",
    "sino",
    "SI",
    "ENTONCES",
    "SINO",
    "mientras",
    "MIENTRAS",
    "para",
    "PARA",
    "yy",
    "YY",
    "oo",
    "OO",
    "noo",
    "NOO",
    "entero",
    "ENTERO",
    "cadena",
    "CADENA",
    "decimal",
    "DECIMAL",
    "CARACTER",
    "caracter",
    "bool",
    "leer",
    "escribir"
};

#endif