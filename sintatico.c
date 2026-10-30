#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

void AnaliseSintatica(FILE * in, FILE * out) {
    char frase[256];
    Derivassaum der[256];

    // Lê a saída do léxico e cria uma fila de token para preparação para a análise sintática
    int i = 0;
    while(fgets(frase, sizeof(frase), in))
    {
        extrairLexema(frase, &der[i]);
        i++;
    }

    // TODO: transformar o vetor em estrutura de árvore

}

void extrairLexema (char * f, Derivassaum * dest) {
    sscanf(f, "<%[^,], %[^>]> %d %d", dest->token.nome, dest->token.lexema, &dest->token.linha, &dest->token.coluna);
}