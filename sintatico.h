#ifndef SINTATICO_H
#define SINTATICO_H
#include "lexico.h" // para a struct Token

typedef struct {
    Token token;
    int ehTerminal; // bool que será utilizado para a construção da árvore
} Derivassaum; // Estrutura para gerar a cadeia que forma uma sentença

void AnaliseSintatica(FILE * entradaSintatico, FILE * saidaSintatico);
void extrairLexema(char * frase, Derivassaum * destino);

#endif