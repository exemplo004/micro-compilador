#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

void AnaliseSintatica(FILE * in, FILE * out) {
    char frase[256];
    char lexema[32];

    while(fgets(frase, sizeof(frase), in))
    {
        extrairLexema(frase, lexema);
        // TODO: talvez fazer um sistema de fila pra analisar a ordem dos tokens?
    }
}

void extrairLexema (char * f, char * dest) {
    sscanf(f, "<%[^,]", dest);
}