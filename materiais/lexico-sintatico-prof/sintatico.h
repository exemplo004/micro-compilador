#ifndef SINTATICO_H
#define SINTATICO_H

#include "lexico.h"

// Estrutura de árvore sintática
// É como se fosse um nó de uma árvore binária
typedef struct AST {
    TipoToken tipo;
    char * lexema;
    struct AST * esq;
    struct AST * dir;
} AST;

/* Retorna NULL e emite diagnostico se houver erro.
 * A AST possui copias dos lexemas, independentes do Scanner.
 * O chamador deve libera-la com ast_free(). */
AST *parse_ast(Scanner *scanner);
void ast_print(const AST *arvore, int profundidade);
int ast_to_dot(const AST *arvore, const char *caminho);
void ast_free(AST *arvore);

#endif
