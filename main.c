#include <stdio.h>
#include "preprocessador.h"
#include "lexico.h"
#include "sintatico.h"

int main (int argc, char ** argv)
{
    // Checando se o usuário inseriu os parâmetros adequadamente
    if (argc != 3)
    {
        printf("Numero incorreto de argumentos\n"
               "Uso: main.exe <arquivo_entrada.asm> <prefixo_saida>\n");
        return 1;
    }

    // Arquivos de entrada e saída
    char * in = argv[1];
    char * out = argv[2];

    // Checando se o arquivo de entrada existe
    FILE * arquivo = fopen(in, "r");
    if (arquivo == NULL)
    {
        printf("O arquivo %s nao foi encontrado.\n", in);
        return 1;
    }
    fclose(arquivo);

    PreProcessamento(in, ARQ_OUT_PRE);

    FILE * entradaLexico = fopen(ARQ_OUT_PRE, "r");
    if (!entradaLexico)
    {
        printf("Naum foi possihvel abrir o arquivo preh-processado %s\n", ARQ_OUT_PRE);
        return 1;
    }

    FILE * saidaLexico = fopen(ARQ_OUT_LEX, "w");
    if (!saidaLexico)
    {
        printf("Nao foi possivel criar o arquivo de saida %s\n", ARQ_OUT_LEX);
        fclose(entradaLexico);
        return 1;
    }

    ConfigurarArquivosAuxiliares(ARQ_OUT_LEX);
    AnaliseLexica(entradaLexico, saidaLexico);
    fclose(entradaLexico);
    fclose(saidaLexico);


    FILE * entradaSintatico = fopen(ARQ_OUT_LEX, "r");
    if (!entradaSintatico) {
        printf("Naum foi possihvel abrir o arquivo lehxico %s\n", ARQ_OUT_LEX);
        return 1;
    }

    AnaliseSintatica(entradaSintatico, out);
    fclose(entradaSintatico);

    return 0;
}