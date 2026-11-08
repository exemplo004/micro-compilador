#include <stdio.h>
#include "preprocessador.h"
#include "lexico.h"
#include "sintatico.h"

int main (int argc, char ** argv)
{
    // Checando se o usuário inseriu os parâmetros adequadamente
    if (argc != 5)
    {
        printf("Numero incorreto de argumentos\n"
               "Uso: main.exe <arquivo_entrada.asm> <arquivo_saida.pre> <arquivo_saida.lex> <arquivo_saida.sin>\n");
        return 1;
    }

    // Arquivos de entrada e saída
    char * inPre = argv[1];
    char * outPre = argv[2];
    char * outLex = argv[3];
    char * outSin = argv[4];

    // Checando se o arquivo de entrada existe
    FILE * arquivo = fopen(inPre, "r");
    if (arquivo == NULL)
    {
        printf("O arquivo %s nao foi encontrado.\n", inPre);
        return 1;
    }
    fclose(arquivo);

    PreProcessamento(inPre, outPre);

    FILE * entradaLexico = fopen(outPre, "r");
    if (!entradaLexico)
    {
        printf("Naum foi possihvel abrir o arquivo preh-processado %s\n", outPre);
        return 1;
    }

    FILE * saidaLexico = fopen(outLex, "w");
    if (!saidaLexico)
    {
        printf("Nao foi possivel criar o arquivo de saida %s\n", outLex);
        fclose(entradaLexico);
        return 1;
    }

    ConfigurarArquivosAuxiliares(outLex);
    AnaliseLexica(entradaLexico, saidaLexico);
    fclose(entradaLexico);
    fclose(saidaLexico);


    FILE * entradaSintatico = fopen(outLex, "r");
    if (!entradaSintatico) {
        printf("Naum foi possihvel abrir o arquivo lehxico %s\n", outLex);
        return 1;
    }

    FILE * saidaSintatico = fopen(outSin, "w");
    if (!saidaLexico)
    {
        printf("Nao foi possivel criar o arquivo de saida %s\n", outSin);
        fclose(entradaSintatico);
        return 1;
    }

    FILE * errosSintatico = fopen("erros_temporarios.txt", "w");
    AnaliseSintatica(entradaSintatico, saidaSintatico, errosSintatico);
    fclose(entradaSintatico);
    fclose(saidaSintatico);

    return 0;
}