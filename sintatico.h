#ifndef SINTATICO_H
#define SINTATICO_H
#include <stdio.h>
#include "lexico.h" // para a struct Token

#define MAX_TOKENS 256

typedef enum {
    // Registradores identificadores e literais
    REG, ID, NUM_INT, STRING,

    // Instruções
    INS_LI, INS_LA, INS_MOVE,
    INS_LW, INS_SW, INS_LB,
    INS_SB, INS_LH, INS_SH,
    INS_ADD, INS_ADDI, INS_ADDU,
    INS_SUB, INS_SUBU, INS_MUL,
    INS_DIV, INS_MFLO, INS_MFHI,
    INS_AND, INS_OR, INS_XOR,
    INS_NOR, INS_SLL, INS_SRL,
    INS_SLT, INS_BEQ, INS_BNE,
    INS_BLT, INS_BLE, INS_BGT,
    INS_BGE, INS_J, INS_JAL,
    INS_JR, INS_SYSCALL, INS_NOP,

    // Diretivas
    DIR_DATA, DIR_TEXT, DIR_WORD, DIR_HALF, DIR_BYTE,
    DIR_SPACE, DIR_ASCII, DIR_ASCIIZ, DIR_GLOBL, DIR_ALIGN,

    // Pontuação
    SMB_COM, SMB_COL, SMB_OPA, SMB_CPA,

    // Finalizadores
    TK_EOF, EOL
} TipoToken;


// Estrutura de Parser
// É meio que o cara que segura o scanner e decide o que fazer com ele
typedef struct {
    Token atual;
    int erro;
    unsigned nivel;
} Parser;

// Estrutura de árvore sintática
// É como se fosse um nó de uma árvore binária
typedef struct AST {
    TipoToken tipo;
    char * lexema;
    struct AST * esq;
    struct AST * dir;
} AST;

int AnaliseSintatica(FILE * lexOut, FILE * saidaSintatico, FILE * errSintatico);
void extrairLexemas(FILE * lexOut, Token * destino);
void exibirTokens(Token * tokenVector);

#endif