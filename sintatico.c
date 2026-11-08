#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

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

void exibirTokens(Token * t, int total) {
    int i = 0;
    while (i < total) {
        printf("%i. %s\t%s\t%i\t%i\n", i, t[i].nome, t[i].lexema, t[i].linha, t[i].coluna);
        i++;
    }
}

int AnaliseSintatica(FILE *tokens, FILE *arvore, FILE *erros) {
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
    //temporario
    return 0;
}