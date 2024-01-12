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

void Mover(); void sent();

//asignacion
void asignacion(Token tokenActual) {
    if (strcmp(tokenActual.tipo, "TOKEN_IDENTIFICADOR") == 0) {
        agregarNodoArbol(tokenActual.tipo);
        Mover(); // mover una vez mas
        if (listaTokens != NULL) {
            Token tokenIgual = listaTokens->token;
            if (strcmp(tokenIgual.lexema, "=") == 0) {
                agregarNodoArbol(tokenIgual.tipo);
                Mover();
                if (listaTokens != NULL) {
                    Token tokenNumero = listaTokens->token;
                    if (strcmp(tokenNumero.tipo, "TOKEN_NUM") == 0) {
                        //printf("Asignacion Correcta\n");
                        agregarNodoArbol(tokenNumero.tipo);
                        sent();
                        return;
                    } else {
                        printf("Error en la linea %d: Se esperaba un numero despues del signo igual.\n", lineaError);
                        exit(0);
                        //return;
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

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
// Estructura para un nodo del árbol sintáctico
typedef struct NodoArbol {
    char* valor;
    struct NodoArbol* padre;
    struct NodoArbol* hijos[50];  // Puedes ajustar el tamaño según tus necesidades
    int numHijos;
} NodoArbol;

NodoArbol* nodoActual = NULL;  // Puntero al nodo actual en construccióng

// Función para agregar un nodo al ARbol
void agregarNodoArbol(char* valor) {
    
    NodoArbol* nuevoNodo = (NodoArbol*)malloc(sizeof(NodoArbol));
    nuevoNodo->valor = strdup(valor);  // Duplicar la cadena para evitar problemas de memoria
    nuevoNodo->numHijos = 0;
    nuevoNodo->padre = nodoActual;
    
    if (nodoActual != NULL) {
        nodoActual->hijos[nodoActual->numHijos++] = nuevoNodo;
    } else {
        // Este es el primer nodo raiz del arbol
        nodoActual = nuevoNodo;
    }
}

void imprimirArbolEstetico(NodoArbol* nodo, int nivel) {
    if (nodo != NULL)  {
        for (int i = 0; i < nivel; i++) {
            printf("   ");
        }
        printf("|-- %s\n", nodo->valor);

        for (int i = 0; i < nodo->numHijos; i++) {
            imprimirArbolEstetico(nodo->hijos[i], nivel + 1);  // Incrementar nivel antes de imprimir los hijos
        }
    }else{
        printf("Fin del arbol");
    }
}

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

struct Token tokensin;
bool exprela(); //expresion relacion
void si(Token tokenActual); //si
void sino(Token tokenActual); //sino
void sent(); 
void leer(tokenActual); //leer
void escribir(tokenActual); //escribir
void mientras(tokenActual); //mientras
void variable(tokenActual); //decimal para declaracion de variables


bool exprela() {
    Mover();
    if (tokensin.tipo == "TOKEN_IDENTIFICADOR" || tokensin.tipo == "TOKEN_NUM") {
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if (strcmp(tokensin.lexema, ">") == 0 || strcmp(tokensin.lexema, "<") == 0) {
            agregarNodoArbol(tokensin.tipo);
            Mover();
            if (tokensin.tipo == "TOKEN_IDENTIFICADOR" || tokensin.tipo == "TOKEN_NUM") {
                agregarNodoArbol(tokensin.tipo);
                Mover();
                return true;
            }else{
                printf("Error en la linea %d, de esperaba un Identificador o Numero", lineaError);
                exit(1);
            }
        }else{
            printf("Error en la linea %d, de esperaba un simbolo "">"" o ""<""", lineaError);
            exit(1);
        }
    }else{
        printf("Error en la linea %d, de esperaba un Identificador o Numero", lineaError);
        exit(1);
    }
    return false;
}

void si(Token tokenActual) {
    if (strcmp(tokenActual.lexema, "si") == 0) {
            agregarNodoArbol(tokenActual.tipo);
        Mover(); // No es necesario mover aquí, ya estás en el siguiente 
        if (strcmp(listaTokens->token.lexema, "(") == 0) {
            agregarNodoArbol(tokensin.tipo);
            exprela();
            if (strcmp(listaTokens->token.lexema, ")") == 0) {
                agregarNodoArbol(tokensin.tipo);
                Mover(); // Mover al siguiente token
                if (strcmp(listaTokens->token.lexema, "{") == 0) {
                    agregarNodoArbol(tokensin.tipo);
                    //Mover(); // Mover al siguiente token
                    sent();
                    if (strcmp(listaTokens->token.lexema, "}") == 0) {
                        agregarNodoArbol(tokensin.tipo);
                        sent();  // Mover al siguiente token después del '}'
                        //return;
                    } else {
                        printf("Error en la linea %d: Se esperaba '}' despues de la sentencia si.\n", lineaError);
                        exit(1);
                    }
                } else {
                    printf("Error en la linea %d: Se esperaba '{' despues de la condicion si.\n", lineaError);
                    exit(1);
                }
            } else {
                printf("Error en la linea %d: Se esperaba ')' despues de la condicion si.\n", lineaError);
                exit(1);
            }
        } else {
            printf("Error en la linea %d: Se esperaba '(' despues de 'si'.\n"), lineaError;
            exit(1);
        }
    }
}

void sino(Token tokenActual) {

    Retroceder();

    if (strcmp(tokensin.lexema, "}")== 0)
    {
        Mover();
     if (strcmp(tokenActual.lexema, "sino") == 0) {
        agregarNodoArbol(tokenActual.tipo);
        Mover(); // Mover al siguiente token
        if (strcmp(listaTokens->token.lexema, "{") == 0) {
            //Mover();
            agregarNodoArbol(tokensin.tipo);
            sent();
            if (strcmp(listaTokens->token.lexema, "}") == 0) {
                agregarNodoArbol(tokensin.tipo);
                Mover();  // Mover al siguiente token después del '}'
                //return;
            } else {
                printf("Error: Se esperaba '}' despues del bloque de codigo 'sino'.\n",lineaError);
                exit(1);
            }
        } else {
            printf("Error: Se esperaba '{' despues de 'sino'.\n",lineaError);
            exit(1);
        }
    }else{
        printf("Error: se esperaba 'sino' en la linea: ", lineaError);
        exit(1);
    }
    }else{
        printf("error: se esperaba  '}' en la linea: ", lineaError);
        exit(1);
    }
    
}

void leer(tokenActual){
    if (strcmp(listaTokens->token.lexema, "leer") == 0)
    {
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if(strcmp(listaTokens->token.lexema, "(") == 0){
            agregarNodoArbol(tokensin.tipo);
            Mover();
            if (strcmp(listaTokens->token.tipo, "TOKEN_IDENTIFICADOR") == 0 || (strcmp(listaTokens->token.tipo, "TOKEN_NUM")))
            {
                agregarNodoArbol(tokensin.tipo);
                Mover();
                if(strcmp(listaTokens->token.lexema, ")") == 0){
                    agregarNodoArbol(tokensin.tipo);
                    sent();
                }
            }else{
                printf("Error: se esperaba un texto", lineaError);
                exit(1);
            }
        }else{
            printf("Error: se esperaba comillas dobles en la linea", lineaError);
            exit(1);
        }
    }else{
        printf("Error: se esperaba 'leer' en la linea: ", lineaError);
        exit(1);
    }
    
}

void escribir(tokenActual){
    if(strcmp(listaTokens->token.lexema,"escribir") == 0){
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if (strcmp(listaTokens->token.lexema, "(") == 0)
        {
            agregarNodoArbol(tokensin.tipo);
            Mover();
            if (strcmp(listaTokens->token.tipo, "TOKEN_IDENTIFICADOR") == 0 || strcmp(listaTokens->token.tipo, "TOKEN_NUM"))
            {
                agregarNodoArbol(tokensin.tipo);
                Mover();
                if(strcmp(listaTokens->token.lexema,")") == 0){
                    agregarNodoArbol(tokensin.tipo);
                    sent();
                }else{
                    printf("Error: se esperaba ')' en la linea: ", lineaError);
                    exit(1);
                }
            }else{
                printf("Error: se esperaba un identificador o numero en la linea: ", lineaError);
                exit(1);
            }
        }else{
            printf("Error: se esperaba '(' en la linea: ", lineaError);
            exit(1);
        }
    }else{
        printf("Error: se esperaba 'escribir' en la linea: ", lineaError = tokensin.linea);
    }
}

void mientras(tokenActual){
    if (strcmp(listaTokens->token.lexema, "mientras") == 0)
    {
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if((strcmp(listaTokens->token.lexema, "(") == 0)){
            //Mover();
            agregarNodoArbol(tokensin.tipo);
            if(exprela()){
                if((strcmp(listaTokens->token.lexema, ")") == 0)){
                    //printf("Mientras correcto'\n'");
                    agregarNodoArbol(tokensin.tipo);
                    sent();
                }else{
                    printf("Error: se esperaba ')'", lineaError= tokensin.linea);
                    exit(1);
                }
            }
        }else{
            printf("Error: Se esperaba '(' en la linea", lineaError= tokensin.linea);
            exit(1);
        }
    }else{
        printf("se esperaba la palabra reservada mientras", lineaError= tokensin.linea);
        exit(1);
    }
}

void variable(tokenActual){
    if(strcmp(listaTokens->token.lexema, "decimal") == 0){
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if (strcmp(listaTokens->token.tipo, "TOKEN_IDENTIFICADOR") == 0)
        {
            agregarNodoArbol(tokensin.tipo);
            sent();
        }else{
            printf("Error: Se esperaba un identificador en la linea: ", lineaError = tokensin.linea);
            exit(1);
        }
    }else if(strcmp(listaTokens->token.lexema, "caracter") == 0){
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if (strcmp(listaTokens->token.tipo, "TOKEN_IDENTIFICADOR") == 0)
        {
            agregarNodoArbol(tokensin.tipo);
            sent();
        }else{
            printf("Error: Se esperaba un identificador en la linea: ", lineaError = tokensin.linea);
            exit(1);
        }
    }else if(strcmp(listaTokens->token.lexema, "cadena") == 0){
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if (strcmp(listaTokens->token.tipo, "TOKEN_IDENTIFICADOR") == 0)
        {
            agregarNodoArbol(tokensin.tipo);
            sent();
        }else{
            printf("Error: Se esperaba un identificador en la linea: ", lineaError = tokensin.linea);
            exit(1);
        }
    }else{
        printf("Se esperaba la declaracion de una variable");
        exit(1);
    }
}

void arit(tokenActual);

void arit(tokenActual){
    if(strcmp(listaTokens->token.lexema, "arit") == 0){
        agregarNodoArbol(tokensin.tipo);
        Mover();
        if ((strcmp(listaTokens->token.lexema, "=") == 0))
        {
            agregarNodoArbol(tokensin.tipo);
            Mover();
            if((strcmp(listaTokens->token.tipo, "TOKEN_NUM") == 0)){
                agregarNodoArbol(tokensin.tipo);
                Mover();
                if ((strcmp(listaTokens->token.lexema, "+") == 0) || (strcmp(listaTokens->token.lexema, "-") == 0) || (strcmp(listaTokens->token.lexema, "/") == 0) || (strcmp(listaTokens->token.lexema, "*") == 0))
                {
                    agregarNodoArbol(tokensin.tipo);
                    Mover();
                    if ((strcmp(listaTokens->token.tipo, "TOKEN_NUM") == 0))
                    {
                        agregarNodoArbol(tokensin.tipo);
                        sent();
                    }else{
                        printf("Se esperaba un numero en: ", lineaError);
                        exit(1);
                    }
                    
                }else{
                    printf("Se esperaba un simbolo aritmetico en: ", lineaError);
                    exit(1);
                }
                
            }else{
                printf("Se esperaba un numero en: ", lineaError);
                exit(1);
            }
        }else{
            printf("Se esperaba el simbolo '=' en: ", lineaError);
            exit(1);
        }
        
    }else{
        printf("Se esperaba la palabra reservada arit en: ",lineaError);
        exit(1);
    }
}

void Mover() {
    if (listaTokens != NULL && listaTokens->Sig != NULL) {
        listaTokens = listaTokens->Sig;  // Avanzar al siguiente nodo
        tokensin = listaTokens->token;   // Actualizar tokensin con la información del nuevo token
        columna += strlen(tokensin.lexema); // Actualizar la columna
        lineaError = tokensin.linea;
    } else {
        printf("Se acabo la lista enlazada.\n");
        imprimirArbolEstetico(nodoActual,0);
        exit(1);
    }
    return;
}

void Retroceder() {
    if (listaTokens != NULL && listaTokens->Ant != NULL) {
        listaTokens = listaTokens->Ant;  // Retroceder al nodo anterior
        tokensin = listaTokens->token;   // Actualizar tokensin con la información del nuevo token
        columna -= strlen(tokensin.lexema); // Actualizar la columna
        lineaError = tokensin.linea;
    } else {
        printf("Estás en el inicio de la lista enlazada.\n");
        exit(1);
    }
}


void finbloque(tokenActual);

void finbloque(tokenActual){
    //Mover(); // Avanzar al siguiente token
    if (listaTokens != NULL) {
        Token tokenActual = listaTokens->token;
        if (strcmp(tokenActual.lexema, "}") == 0) {
            agregarNodoArbol(tokensin.tipo);
            sent();
            //Mover(); // Avanzar al siguiente token después de '}'
            //return;
        } else {
            printf("Error: Se esperaba '}' al final del bloque.\n en la linea", lineaError);
            exit(1);
        }
    } else {
        printf("Error: Fin de archivo inesperado al final del bloque.\n en la linea", lineaError);
        exit(1);
    }
}

void final(tokenActual);

void final(tokenActual){
    if (strcmp(listaTokens->token.lexema, "Fin") == 0)
    {
        agregarNodoArbol(tokensin.tipo);
        sent();
        printf("El programa finalizo con exito");
    if(strcmp(listaTokens->token.tipo, "TOKEN_NUM") == 0 || strcmp(listaTokens->token.tipo, "TOKEN_SIMBOLO") == 0 || strcmp(listaTokens->token.tipo, "TOKEN_PALABRA_RESERVADA") == 0 || strcmp(listaTokens->token.tipo, "TOKEN_IDENTIFICADOR") == 0){
            printf("No puede haber nada despues del final");
            exit(1);
        }else{
            printf("No puede haber nada despues del final");
            exit(1);    
        }
    }else{
        printf("No puede haber nada despues del final");
        exit(1);
    }
    
}

void sent() {
    Mover(); // Avanzar al siguiente token
    if (listaTokens != NULL) {
        Token tokenActual = listaTokens->token;

        //otro inicio
        if (strcmp(tokenActual.lexema, "Inicio") == 0)
        {
            printf("No puede a ver dos inicios");
            //asignacion
        } else if(strcmp(tokenActual.tipo, "TOKEN_IDENTIFICADOR") == 0){
            //printf(" es ID: %s\n", tokenActual.lexema);
            asignacion(tokenActual);
            //SI
        } else if (strcmp(tokenActual.lexema, "si") == 0) {
            si(tokenActual);
        }else if(strcmp(tokenActual.lexema, "sino") == 0){
            sino(tokenActual);
        }else if(strcmp(tokenActual.lexema,"leer") == 0){
            leer(tokenActual);
        }else if(strcmp(tokenActual.lexema,"escribir") == 0){
            escribir(tokenActual);
        }else if(strcmp(tokenActual.lexema,"mientras") == 0){
            mientras(tokenActual);
        }else if(strcmp(tokenActual.lexema,"decimal") == 0 || strcmp(listaTokens->token.lexema, "caracter") == 0 || strcmp(listaTokens->token.lexema, "cadena") == 0){
            variable(tokenActual);
        }else if(strcmp(tokenActual.lexema,"arit") == 0){
            arit(tokenActual);
        }else if(strcmp(tokenActual.lexema,"Fin") == 0){
            final(tokenActual);
        }else if(strcmp(tokenActual.tipo,"TOKEN_SIMBOLO") == 0){
            finbloque(tokenActual);
        } else {
            printf("Error: No se encontro otro token\n");
            return;
            exit(1);
        }
    }
}

void inicio() {
    if (listaTokens != NULL) {
        Token primerToken = listaTokens->token;
        if (strcmp(primerToken.lexema, "Inicio") == 0) {
            
            agregarNodoArbol(primerToken.tipo);
            
            
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
            if (car == '"' ||car == '+' || car == '-' || car == '*' || car == '/' || car == '=' || car == ';' || car == '#' || car == '&' || car == '(' || car == ')' || car == '>' || car == '<' || car == '{'|| car == '}') {
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
    // Imprimir el árbol
    imprimirArbolEstetico(nodoActual, 0);

    return 0;
}