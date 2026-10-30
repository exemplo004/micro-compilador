#ifndef PREPROCESSADOR_H
#define PREPROCESSADOR_H

#define ARQUIVO_REFERENCIA "ref"
#define ARQUIVO_TEMPORARIO "buffer"

#if defined(_WIN32)
#define CPCOMMAND "copy"
#else
#define CPCOMMAND "cp"
#endif

void PreProcessamento(char * entrada, char * saida);

#endif