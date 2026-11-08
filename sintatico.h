#ifndef SINTATICO_H
#define SINTATICO_H
#include <stdio.h>
#include "lexico.h" // para a struct Token

#define MAX_TOKENS 4096

//retorna 0 se valido, 1 se invalido, -1 se erro operacional
int AnaliseSintatica(FILE *tokens, FILE *arvore, FILE *erros);

int extrairLexemas(FILE *in, Token *dst);
void exibirTokens(Token *tokenVector, int total);

#endif