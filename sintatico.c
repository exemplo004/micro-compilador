#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

// TODO: fazer uma função que compare a string nome de um token com o enum TipoToken
// !! Ver função nome_token() em ./materiais/lexico-sintatico-prof/lexico.c

// Funcão principal do analisador
int AnaliseSintatica(FILE * tokens, FILE * arvore, FILE * erros) {
    Token vetorTokens[MAX_TOKENS];
    int totalTokens = extrairLexemas(tokens, vetorTokens);
    //verificação
    exibirTokens(vetorTokens, totalTokens);
    
    //se o arquivo estiver completamente vazio ou apenas o TK_EOF
    if (totalTokens <= 1) {
        fprintf(erros, "Erro: Arquivo vazio ou sem secoes validas.\n");
        printf("PROGRAMA SINTATICAMENTE INVALIDO\n");
        
        return 1;
    }

    iniciar_ast(&vetorTokens[0]);

    //temporario
    return 0;
}

//le o arquivo .lex e insere os tokens EOL e TK_EOF
int extrairLexemas(FILE *in, Token *dst) {
    char frase[256];
    int i = 0;
    int linhaAnterior = 0; // Registra linha ao adicionar um token, para o caso do próximo token ser um EOL
    int temTokenNaLinha = 0;
    
    while (fgets(frase,sizeof(frase), in) != NULL) {
        Token t = {0};
        int lidos = sscanf(frase, "<%[^,], %[^>]> %d %d", t.nome, t.lexema, &t.linha, &t.coluna);
        
        //se nao tiver o formato de token, pula
        if (lidos < 4) {
            continue;
        }
        
        //se for o token de fim, para de ler as linhas
        if (strcmp(t.nome, "TK_EOF") == 0) {
            break;
        }
        
        //se mudou de linha e a linha anterior tinha token, coloca o EOL da linha anterior
        if (linhaAnterior != 0 && t.linha != linhaAnterior && temTokenNaLinha) {
            if (i < MAX_TOKENS) {
                strcpy(dst[i].nome, "EOL");
                strcpy(dst[i].lexema, "EOL");
                dst[i].linha = linhaAnterior;
                dst[i].coluna = dst[i - 1].coluna + (int)strlen(dst[i - 1].lexema); 
                //pega a coluna de onde o ultimo token começou e soma com a quantidade de letras que o token tem
                i++;
            }
            temTokenNaLinha = 0;
        }
        
        //coloca o token lido no vetor
        if (i < MAX_TOKENS) {
            dst[i] = t;
            i++;
            linhaAnterior = t.linha;
            temTokenNaLinha = 1;
        }
    }
    //se a ultima linha do programa tinha token, ela tambem precisa de um EOL antes do final
    if (temTokenNaLinha && i < MAX_TOKENS) {
        strcpy(dst[i].nome, "EOL");
        strcpy(dst[i].lexema, "EOL");
        dst[i].linha = linhaAnterior;
        dst[i].coluna = dst[i - 1].coluna + (int)strlen(dst[i - 1].lexema);
        i++;
    }
    //adiciona TK_EOF no final da sequencia
    if (i < MAX_TOKENS) {
        strcpy(dst[i].nome, "TK_EOF");
        strcpy(dst[i].lexema, "EOF");
        dst[i].linha = linhaAnterior;
        dst[i].coluna = (i > 0) ? (dst[i - 1].coluna + 1) : 1;
        i++;
    }
    return i;
}

// Libera a memória de todos os nós de uma árvore
void ast_free(AST * raiz) {
    if (!raiz) return;
    ast_free(raiz->esq);
    ast_free(raiz->dir);
    free(raiz->lexema);
    free(raiz);
}

static void erro(Parser *p, const char *mensagem) {
    if (p->erro) return;
    fprintf(stderr, "Erro sintatico na linha %d, coluna %d: %s; encontrado %s ('%s').\n",
            p->atual.linha, p->atual.coluna, mensagem,
            nome_token(p->atual.tipo), p->atual.lexema);
    p->erro = 1;
}

AST * iniciar_ast(Token * tokens) {
    // Inicializa o parser com 0 em todos os atributos
    Parser p = { 0 };

    p.atual = tokens[0];

    AST * raiz = exp_programa(&p);

    // if (!p.erro && p.atual.tipo != TOKEN_FIM)
    //     erro(&p, "esperado operador ou fim da expressao");
    // free(p.atual.lexema);
    // if (p.erro) { ast_free(raiz); return NULL; }
    // return raiz;
}

// Abaixo estão todas as funcões que teremos que implementar, organizadas de acordo com o pdf 4
// As regras de derivação estão comentadas acima de cada função
// -------------------------------------------------------------------
// --------------------- E - P - N - O - E ---------------------------
// ----------------------- X - A - S - R - S -------------------------
// -------------------------------------------------------------------

// ---- 2.2. Organização do programa

// <secao> { <secao> } TK_EOF
AST * exp_programa(Parser * p) {
    AST * l = exp_secao(p);

    while (!p->erro) {
        // CONTINUE
        f (p->erro) { ast_free(l); ast_free(r); return NULL; }
    }
}

// <secao_dados> | <secao_codigo>
AST * exp_secao(Parser * p) {
    printf("implementar função exp_secao\n"); // TODO
}

// .data" EOL { <linha_dados> }
AST * exp_secao_dados(Parser * p) {
    printf("implementar função exp_secao_dados\n"); // TODO
}

// ".text" EOL { <linha_codigo> }
AST * exp_secao_codigo(Parser * p) {
    printf("implementar função exp_secao_codigo\n"); // TODO
}

// <rotulo> [ <diretiva_dados> ] EOL | <diretiva_dados> EOL | <diretiva_global> EOL
AST * exp_linha_dados(Parser * p) {
    printf("implementar função exp_linha_dados\n"); // TODO
}

// <rotulo> [ <instrucao> ] EOL | <instrucao> EOL | <diretiva_global> EOL
AST * exp_linha_codigo(Parser * p) {
    printf("implementar função exp_linha_codigo\n"); // TODO
}

// ID ":"
AST * exp_rotulo(Parser * p) {
    printf("implementar função exp_rotulo\n"); // TODO
}

// ".globl" ID
AST * exp_diretiva_global(Parser * p) {
    printf("implementar função exp_diretiva_global\n"); // TODO
}

// ---- 2.3. Declarações de dados
// <diretiva_inteiros> <lista_inteiros> | <diretiva_string> STRING | ".space" NUM_INT | ".align" NUM_INT
AST * exp_diretiva_dados(Parser * p) {
    printf("implementar função exp_diretiva_dados\n"); // TODO
}

// ".word" | ".half" | ".byte"
AST * exp_diretiva_inteiros(Parser * p) {
    printf("implementar função exp_diretiva_inteiros\n"); // TODO
}

// NUM_INT { "," NUM_INT }
AST * exp_lista_inteiros(Parser * p) {
    printf("implementar função exp_lista_inteiros\n"); // TODO
}

// ".ascii" | ".asciiz"
AST * exp_diretiva_string(Parser * p) {
    printf("implementar função exp_diretiva_string\n"); // TODO
}

// 

// ---- 2.4. Instruções
// <instrucao_tres_registradores>
// | <instrucao_dois_registradores>
// | <instrucao_um_registrador>
// | <instrucao_imediato>
// | <instrucao_deslocamento>
// | <instrucao_li>
// | <instrucao_la>
// | <instrucao_memoria>
// | <instrucao_desvio>
// | <instrucao_salto>
// | <instrucao_sem_operandos>
// sim, é tudo isso mesmo
AST * exp_instrucao(Parser * p) {
    printf("implementar função exp_instrucao\n"); // TODO
}

// <op_tres_registradores> REG "," REG "," REG
AST * exp_instrucao_tres_registradores(Parser * p) {
    printf("implementar função exp_instrucao_tres_registradores\n"); // TODO
}

// "add" | "addu" | "sub" | "subu" | "mul" | "and" | "or" | "xor" | "nor" | "slt"
AST * exp_op_tres_registradores(Parser * p) {
    printf("implementar função exp_op_tres_registradores\n"); // TODO
}

// <op_dois_registradores> REG "," REG
AST * exp_instrucao_dois_registradores(Parser * p) {
    printf("implementar função exp_instrucao_dois_registradores\n"); // TODO
}

// "move" | "div"
AST * exp_op_dois_registradores(Parser * p) {
    printf("implementar função exp_op_dois_registradores\n"); // TODO
}

// <op_um_registrador> REG
AST * exp_instrucao_um_registrador(Parser * p) {
    printf("implementar função exp_instrucao_um_registrador\n"); // TODO
}

// "mflo" | "mfhi" | "jr"
AST * exp_op_um_registrador(Parser * p) {
    printf("implementar função exp_op_um_registrador\n"); // TODO
}

// "addi" REG "," REG "," NUM_INT
AST * exp_instrucao_imediato(Parser * p) {
    printf("implementar função exp_instrucao_imediato\n"); // TODO
}

// <op_deslocamento> REG "," REG "," NUM_INT
AST * exp_instrucao_deslocamento(Parser * p) {
    printf("implementar função exp_instrucao_deslocamento\n"); // TODO
}

// "sll" | "srl"
AST * exp_op_deslocamento(Parser * p) {
    printf("implementar função exp_op_deslocamento\n"); // TODO
}

// "li" REG "," NUM_INT
AST * exp_instrucao_li(Parser * p) {
    printf("implementar função exp_instrucao_li\n"); // TODO
}

// "la" REG "," ID
AST * exp_instrucao_la(Parser * p) {
    printf("implementar função exp_instrucao_la\n"); // TODO
}

// <op_memoria> REG "," <endereco>
AST * exp_instrucao_memoria(Parser * p) {
    printf("implementar função exp_instrucao_memoria"); // TODO
}

// "lw" | "sw" | "lb" | "sb" | "lh" | "sh"
AST * exp_op_memoria(Parser * p) {
    printf("implementar função exp_op_memoria"); // TODO
}

// ID | NUM_INT "(" REG ")"
AST * exp_endereco(Parser * p) {
    printf("implementar função exp_endereco"); // TODO
}

// <op_desvio> REG "," REG "," ID
AST * exp_instrucao_desvio(Parser * p) {
    printf("implementar função exp_instrucao_desvio"); // TODO
}

// "beq" | "bne" | "blt" | "ble" | "bgt" | "bge"
AST * exp_op_desvio(Parser * p) {
    printf("implementar função exp_op_desvio"); // TODO
}

// <op_salto> ID
AST * exp_instrucao_salto(Parser * p) {
    printf("implementar função exp_instrucao_salto"); // TODO
}

// "j" | "jal"
AST * exp_op_salto(Parser * p) {
    printf("implementar função exp_op_salto"); // TODO
}

// <instrucao_sem_operandos> ::= “syscall” | “nop”
AST * exp_instrucao_sem_operandos(Parser * p) {
    printf("implementar função exp_instrucao_sem_operandos"); // TODO
}

void exibirTokens(Token * t, int total) {
    int i = 0;
    while (i < total) {
        printf("%i. %s\t%s\t%i\t%i\n", i, t[i].nome, t[i].lexema, t[i].linha, t[i].coluna);
        i++;
    }
}

