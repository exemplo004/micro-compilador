#ifndef SINTATICO_H
#define SINTATICO_H
#include <stdio.h>
#include "lexico.h" // para a struct Token

#define MAX_TOKENS 1024
#define MAX_NESTING 256

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

typedef struct {
    Token * tokens;
    int i;
} Scanner;

// Estrutura de Parser
// É meio que o cara que segura o scanner e decide o que fazer com ele
typedef struct {
    Scanner scanner;
    Token atual;
    int erro;
    unsigned nivel;
} Parser;

// Estrutura de árvore sintática
typedef struct AST {
    char * display;
    struct AST ** filhos;
    int qtFilhos;
    int capacidade;
} AST;

int AnaliseSintatica(FILE * lexOut, FILE * saidaSintatico, FILE * errSintatico);
int extrairLexemas(FILE * lexOut, Token * destino);
void exibirTokens(Token * tokenVector, int total);
void iniciar_ast(Token * vetorTokens);

// ---- EXPANSORES ----
AST * exp_programa(Parser * p);
AST * exp_secao(Parser * p);
AST * exp_secao_dados(Parser * p);
AST * exp_secao_codigo(Parser * p);
AST * exp_linha_dados(Parser * p);
AST * exp_linha_codigo(Parser * p);
AST * exp_rotulo(Parser * p);
AST * exp_diretiva_global(Parser * p);
AST * exp_diretiva_dados(Parser * p);
AST * exp_diretiva_inteiros(Parser * p);
AST * exp_lista_inteiros(Parser * p);
AST * exp_diretiva_string(Parser * p);
AST * exp_instrucao(Parser * p);
AST * exp_instrucao_tres_registradores(Parser * p);
AST * exp_op_tres_registradores(Parser * p);
AST * exp_instrucao_dois_registradores(Parser * p);
AST * exp_op_dois_registradores(Parser * p);
AST * exp_instrucao_um_registrador(Parser * p);
AST * exp_op_um_registrador(Parser * p);
AST * exp_instrucao_imediato(Parser * p);
AST * exp_instrucao_deslocamento(Parser * p);
AST * exp_op_deslocamento(Parser * p);
AST * exp_instrucao_li(Parser * p);
AST * exp_instrucao_la(Parser * p);
AST * exp_instrucao_memoria(Parser * p);
AST * exp_op_memoria(Parser * p);
AST * exp_endereco(Parser * p);
AST * exp_instrucao_desvio(Parser * p);
AST * exp_op_desvio(Parser * p);
AST * exp_instrucao_salto(Parser * p);
AST * exp_op_salto(Parser * p);
AST * exp_instrucao_sem_operandos(Parser * p);

#endif