#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "automatas.h"
#include <stdbool.h>

typedef struct Token {
    char lexema[100];
    char* tipo;
    char nombre[20];
    int id;
    int valor;
    int linea;
    int columna;
} Token;

// Estructura para un nodo de la lista enlazada de tokens
typedef struct tokenNodo {
    Token token;
    struct tokenNodo* Ant;  // Nuevo campo para el nodo anterior
    struct tokenNodo* Sig;  // Campo para el siguiente nodo
} tokenNodo;

struct Nodo{
    struct Nodo *Ant;
    struct Token Info;
    struct Nodo *Sig;
};

FILE* fuente;
char car;
int linea = 0;
int columna = 0;
int contadorID = 1;
int entradas = 0;
int lineaError = 0;
int columnaError = 0;
tokenNodo* listaTokens = NULL; // lista enlazada
tokenNodo* nodoRaiz = NULL;

int esPalabraReservada(char* palabra) {
    for (int i = 0; i < sizeof(palabrasReservadas) / sizeof(palabrasReservadas[0]); i++) {
        if (strcmp(palabra, palabrasReservadas[i]) == 0) {
            return 1; // Es una palabra reservada
        }
    }
    return 0; // No es una palabra reservada
}

/*void imprimirCabecera() {
    printf("Id    Nombre          Tipo                            Lexema    Valor    Ren    Col\n");
    printf("-------------------------------------------------------------------------------------------\n");
}*/

// imprimir acomodado
void imprimirValor(int valor, int longitud) {
    char formato[10];
    sprintf(formato, "%%%dd", longitud);
    printf(formato, valor);
}

void imprimirToken(Token token) {
    printf("%-5d%-15s%-32s%-10s", token.id, token.nombre, token.tipo, token.lexema);
    imprimirValor(token.valor, 8);
    imprimirValor(token.linea, 6);
    imprimirValor(token.columna, 4);
    printf("\n");
}

void agregarTokenALista(Token token) {
    // Crear un nuevo nodo para el token
    tokenNodo* nuevoNodo = (tokenNodo*)malloc(sizeof(tokenNodo));
    nuevoNodo->token = token;
    nuevoNodo->Ant = NULL;  // Inicializar el puntero al nodo anterior
    nuevoNodo->Sig = NULL;  // Inicializar el puntero al siguiente nodo

    // Agregar el nuevo nodo a la lista enlazada
    if (listaTokens == NULL) {
        listaTokens = nuevoNodo;
        nodoRaiz = nuevoNodo;
    } else {
        tokenNodo* actual = listaTokens;
        while (actual->Sig != NULL) {
            actual = actual->Sig;
        }
        actual->Sig = nuevoNodo;
        nuevoNodo->Ant = actual;  // Ajustar el puntero al nodo anterior del nuevo nodo
        nodoRaiz->Ant = nuevoNodo;  // Ajustar el puntero al nodo anterior del primer nodo
    }
    if (nuevoNodo == NULL) {
        // Manejar el error de asignación de memoria
        fprintf(stderr, "Error: No se pudo asignar memoria para un nuevo nodo.\n");
        exit(EXIT_FAILURE);
    }
}


void imprimirListaTokens() {
    // Imprimir la lista de tokens
    printf("\nLista de Tokens:\n");
    printf("Id    Nombre          Tipo                            Lexema    Valor    Ren    Col\n");
    printf("-------------------------------------------------------------------------------------------\n");

    tokenNodo* actual = listaTokens;
    while (actual != NULL) {
        imprimirToken(actual->token);
        actual = actual->Sig;
    }

    if (nodoRaiz != NULL) {
        tokenNodo* ultimoNodo = nodoRaiz;

        while (ultimoNodo->Sig != NULL) {
            ultimoNodo = ultimoNodo->Sig;
        }

        //printf("Ultimo Nodo",ultimoNodo);
        //imprimirToken(ultimoNodo->token);
    } else {
        printf("La lista de tokens está vacía.\n");
    }
}

Token automataSimbolo() {
    char lexema[100];
    lexema[0] = car;
    lexema[1] = '\0';
    Token token;

    car = fgetc(fuente);
    while ((car != EOF) && (isdigit(car) || isalpha(car) || car == '_')) {
        lexema[strlen(lexema)] = car;
        lexema[strlen(lexema) + 1] = '\0';

        car = fgetc(fuente);
    }

    strcpy(token.lexema, lexema);
    token.tipo = (isdigit(lexema[0])) ? "TOKEN_NUM" : "TOKEN_SIMBOLO";
    strcpy(token.nombre, (isdigit(lexema[0])) ? "NUM" : "SIMB");
    token.id = contadorID;
    contadorID++;
    token.valor = (isdigit(lexema[0])) ? atoi(lexema) : 0;
    token.linea = linea;
    token.columna = columna - strlen(token.lexema) + 1;

    // Agregar el token a la lista enlazada
    agregarTokenALista(token);

    return token;
}

Token automataIdentificador() {
    char lexema[100];
    int i = 0;

    while ((car != EOF) && (isalnum(car) || car == '_')) {
        lexema[i++] = car;
        car = fgetc(fuente);
    }

    lexema[i] = '\0';

    Token token;
    strcpy(token.lexema, lexema);
    token.tipo = esPalabraReservada(lexema) ? "TOKEN_PALABRA_RESERVADA" : "TOKEN_IDENTIFICADOR";
    strcpy(token.nombre, esPalabraReservada(lexema) ? "PalRes" : "ID");
    token.id = contadorID;
    contadorID++;
    token.valor = 0;
    token.linea = linea;
    token.columna = columna - strlen(token.lexema) + 1;

    // Agregar el token a la lista enlazada
    agregarTokenALista(token);

    return token;
}

Token automataNumero() {
    char lexema[100];
    int i = 0;

    while ((car != EOF) && isdigit(car)) {
        lexema[i++] = car;
        car = fgetc(fuente);
    }

    lexema[i] = '\0';

    Token token;
    strcpy(token.lexema, lexema);
    token.tipo = "TOKEN_NUM";
    strcpy(token.nombre, "NUM");
    token.id = contadorID;
    contadorID++;
    token.valor = atoi(lexema);
    token.linea = linea;
    token.columna = columna - strlen(token.lexema) + 1;

    // Agregar el token a la lista enlazada
    agregarTokenALista(token);

    return token;
}

Token automataPalabraReservada() {
    char lexema[100];
    int i = 0;

    while ((car != EOF) && (isalpha(car) || car == '_')) {
        lexema[i++] = car;
        car = fgetc(fuente);
    }

    lexema[i] = '\0';

    Token token;
    strcpy(token.lexema, lexema);
    token.tipo = "TOKEN_PALABRA_RESERVADA";
    strcpy(token.nombre, "PalRes");
    token.id = contadorID;
    contadorID++;
    token.valor = 0;
    token.linea = linea;
    token.columna = columna - strlen(token.lexema) + 1;

    // Agregar el token a la lista enlazada
    agregarTokenALista(token);

    return token;
}

int isspace_custom(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

//asignacion

void asignacion(Token tokenActual) {
    if (strcmp(tokenActual.tipo, "TOKEN_IDENTIFICADOR") == 0) {
        Mover(); // Mover uma vez para verificar o sinal de igual
        if (listaTokens != NULL) {
            Token tokenIgual = listaTokens->token;
            if (strcmp(tokenIgual.lexema, "=") == 0) {
                Mover(); // Mover novamente para obter o próximo token (o número)
                if (listaTokens != NULL) {
                    Token tokenNumero = listaTokens->token;
                    if (strcmp(tokenNumero.tipo, "TOKEN_NUM") == 0) {
                        //printf("Asignacion Correcta\n");
                        return;
                    } else {
                        printf("Error: Se esperaba un numero despues del signo igual.\n");
                        return;
                    }
                } else {
                    printf("Error: Fin de archivo inesperado después del signo igual.\n");
                    return;
                }
            } else {
                printf("Error: Se esperaba el signo igual (=) despues del identificador.\n");
                return;
            }
        } else {
            printf("Error: Fin de archivo inesperado después del identificador.\n");
            return;
        }
    } else {
        printf("Error: Se esperaba un identificador a la izquierda de la asignacion.\n");
        return;
    }
}

//ANALIZADOR SINTACTICO

struct Token tokensin;

bool exprela() {
    if (tokensin.tipo == TOKEN_IDENTIFICADOR || tokensin.tipo == TOKEN_NUM) {
        Mover();
        if (strcmp(tokensin.lexema, ">") == 0 || strcmp(tokensin.lexema, "<") == 0) {
            Mover();
            if (tokensin.tipo == TOKEN_IDENTIFICADOR || tokensin.tipo == TOKEN_NUM) {
                Mover();
                return true;
            }
        }
    }
    return false;
}


void si(Token tokenActual) {
    if (strcmp(tokenActual.lexema, "si") == 0) {
        Mover(); // No es necesario mover aquí, ya estás en el siguiente 
        if (strcmp(listaTokens->token.lexema, "(") == 0) {
        //if (strcmp(tokenActual.lexema, "(") == 0) {
            Mover(); // Mover al siguiente token
            /*exprela();
            /*Mover();*/
            // Mover(); // No es necesario mover aquí, ya lo hiciste en exprela
            if (strcmp(listaTokens->token.lexema, ")") == 0) {
                Mover(); // Mover al siguiente token
                if (strcmp(listaTokens->token.lexema, "{") == 0) {
                    Mover(); // Mover al siguiente token
                    sent();
                    if (strcmp(listaTokens->token.lexema, "}") == 0) {
                        printf("If correcto\n");
                        return;
                    } else {
                        printf("Error: Se esperaba '}' despues de la sentencia si.\n");
                    }
                } else {
                    printf("Error: Se esperaba '{' despues de la condicion si.\n");
                }
            } else {
                printf("Error: Se esperaba ')' despues de la condicion si.\n");
            }
        } else {
            printf("Error: Se esperaba '(' despues de 'si'.\n");
        }
    }
}


void Mover() {
    if (listaTokens != NULL && listaTokens->Sig != NULL) {
        listaTokens = listaTokens->Sig;  // Avanzar al siguiente nodo
    } else {
        printf("Error: No hay mas tokens para avanzar.\n");
        exit(1);
    }
    return;
}


void sent() {
    Mover(); // Avanzar al siguiente token
    if (listaTokens != NULL) {
        Token tokenActual = listaTokens->token;

        //otro inicio
        if (strcmp(tokenActual.lexema, "Inicio") == 0)
        {
            printf("No puede a ver dos inicios");
            sent();
            //asignacion
        } else if(strcmp(tokenActual.tipo, "TOKEN_IDENTIFICADOR") == 0){
            //printf(" es ID: %s\n", tokenActual.lexema);
            asignacion(tokenActual);
            //SI
        } else if (strcmp(tokenActual.lexema, "si") == 0) {
            si(tokenActual);
        } else {
            printf("Error: Lista de tokens vacía\n");
            exit(1);
        }
    }
}

void inicio() {
    if (listaTokens != NULL) {
        Token primerToken = listaTokens->token;
        if (strcmp(primerToken.lexema, "Inicio") == 0) {
            sent();
        } else {
            printf("Error: El programa debe comenzar con la etiqueta 'Inicio'\n");
            exit(1);
        }
    } else {
        printf("Error: Lista de tokens vacía\n");
        exit(1);
    }
}

int main() {
    lineaError = 0;
    columnaError = 0;

    fuente = fopen("prueba.txt", "r");
    if (fuente != NULL) {
        //imprimirCabecera();

        while ((car = fgetc(fuente)) != EOF) {
            if (car == '+' || car == '-' || car == '*' || car == '/' || car == '=' || car == ';' || car == '#' || car == '&' || car == '(' || car == ')' || car == '>' || car == '<' || car == '{'|| car == '}') {
                Token token = automataSimbolo();
               // imprimirToken(token);
            } else if (isalpha(car) || car == '_') {
                Token token = automataIdentificador();
                //imprimirToken(token);
            } else if (isdigit(car)) {
                Token token = automataNumero();
                //imprimirToken(token);
            }
        }

        fclose(fuente);

        // Imprimir la lista de tokens
        imprimirListaTokens();
    } else {
        printf("No se encontró el archivo\n");
    }

    inicio();

    return 0;
}