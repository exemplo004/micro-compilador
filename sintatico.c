#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

//le o arquivo .lex e insere os tokens EOL e TK_EOF
int extrairLexemas(FILE *in, Token *dst) {
    char frase[256];
    int total = 0;
    int linhaAnterior = 0;
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
            if (total < MAX_TOKENS) {
                strcpy(dst[total].nome, "EOL");
                strcpy(dst[total].lexema, "EOL");
                dst[total].linha = linhaAnterior;
                dst[total].coluna = dst[total - 1].coluna + (int)strlen(dst[total - 1].lexema); 
                //pega a coluna de onde o ultimo token começou e soma com a quantidade de letras que o token tem
                total++;
            }
            temTokenNaLinha = 0;
        }
        //coloca o token lido no vetor
        if (total < MAX_TOKENS) {
            dst[total] = t;
            total++;
            linhaAnterior = t.linha;
            temTokenNaLinha = 1;
        }
    }
    //se a ultima linha do programa tinha token, ela tambem precisa de um EOL antes do final
    if (temTokenNaLinha && total < MAX_TOKENS) {
        strcpy(dst[total].nome, "EOL");
        strcpy(dst[total].lexema, "EOL");
        dst[total].linha = linhaAnterior;
        dst[total].coluna = dst[total - 1].coluna + (int)strlen(dst[total - 1].lexema);
        total++;
    }
    //adiciona TK_EOF no final da sequencia
    if (total < MAX_TOKENS) {
        strcpy(dst[total].nome, "TK_EOF");
        strcpy(dst[total].lexema, "EOF");
        dst[total].linha = linhaAnterior;
        dst[total].coluna = (total > 0) ? (dst[total - 1].coluna + 1) : 1;
        total++;
    }
    return total;
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