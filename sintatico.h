#ifndef SINTATICO_H
#define SINTATICO_H
#include "lexico.h" // para a struct Token

#define MAX_TOKENS 256

typedef struct {
    Token token;
    int ehTerminal; // bool que será utilizado para a construção da árvore
} Derivassaum; // Estrutura para gerar a cadeia que forma uma sentença

void AnaliseSintatica(FILE * lexOut, FILE * saidaSintatico);
void extrairLexemas(FILE * lexOut, Token * destino);
void exibirTokens(Token * tokenVector);

#endif