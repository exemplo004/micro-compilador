#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

void AnaliseSintatica(FILE * in, FILE * out) {
    Token tokens[MAX_TOKENS] = { 0 };

    // Lê a saída do léxico e cria uma fila de token para preparação para a análise sintática
    extrairLexemas(in, &tokens[0]);
    exibirTokens(&tokens[0]);
    
    // TODO: transformar o vetor em estrutura de árvore
}

void extrairLexemas (FILE * in, Token * dst) {
    char frase[256];
    int i = 0;
    int curLinha = 1;
    
    while(fgets(frase, sizeof(frase), in))
    {
        Token t = { 0 };
        sscanf(frase, "<%[^,], %[^>]> %d %d", t.nome, t.lexema, &t.linha, &t.coluna);


        if (curLinha != t.linha) {
            Token eol = { "EOL", "eol", curLinha, dst[i - 1].coluna + (strlen(dst[i - 1].lexema)) };
            dst[i++] = eol;
            
            curLinha = t.linha;
        }

        dst[i++] = t;
    }
}

void exibirTokens(Token * t) {
    int i = 0;

    while (t[i].nome[0] != '\0' && i < MAX_TOKENS) {
        printf("%i. %s\t%s\t%i\t%i\n", i, t[i].nome, t[i].lexema, t[i].linha, t[i].coluna);
        i++;
    }
}