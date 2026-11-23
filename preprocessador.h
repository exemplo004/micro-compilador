#ifndef PREPROCESSADOR_H
#define PREPROCESSADOR_H

#define ARQUIVO_REFERENCIA "ref"
#define ARQUIVO_TEMPORARIO "buffer"

#define ARQ_OUT_PRE "out.pre"

#if defined(_WIN32)
#define CPCOMMAND "copy"
#else
#define CPCOMMAND "cp"
#endif

void PreProcessamento(char * entrada, char * saida);

#endif