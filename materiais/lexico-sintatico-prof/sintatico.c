#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

/* E -> T { ('+' | '-') T }
 * T -> U { ('*' | '/') U }
 * U -> ('+' | '-') U | P
 * P -> F [ '^' U ]
 * F -> INTEIRO | '(' E ')'
 * Potencia associa a direita e precede sinais unarios.
 */
typedef struct {
    Scanner *scanner;
    Token atual;
    int erro;
    unsigned nivel;
} Parser;

static AST *expressao(Parser *p);
static AST *unario(Parser *p);

void ast_free(AST *t) {
    if (!t) return;
    ast_free(t->esq);
    ast_free(t->dir);
    free(t->lexema);
    free(t);
}

static void erro(Parser *p, const char *mensagem) {
    if (p->erro) return;
    fprintf(stderr, "Erro sintatico na linha %d, coluna %d: %s; encontrado %s ('%s').\n",
            p->atual.linha, p->atual.coluna, mensagem,
            nome_token(p->atual.tipo), p->atual.lexema);
    p->erro = 1;
}

// Avança o cursor do scanner para o próximo token e o transforma em seu token atual
static void consumir(Parser *p) {
    // Aborta caso o parser tenha encontrado um erro
    if (p->erro) return;

    // Libera o espaço de lexema do token atual (malloc() está em AST * novo())
    free(p->atual.lexema);

    // Pede para que o scanner busque o próximo token e faz desse token o token atual do parser
    p->atual = proximo_token(p->scanner);

    // Declara erro no parser caso ocorreram falhas na identificação do token (ainda é erro léxico)
    if (p->atual.tipo == TOKEN_ERRO) {
        fprintf(stderr, "Erro lexico na linha %d, coluna %d: %s.\n",
                p->atual.linha, p->atual.coluna, p->atual.lexema);
        p->erro = 1;
    }
}

/* Assume propriedade dos filhos, inclusive em caso de falha. */
static AST *novo(Parser *p, TipoToken tipo, const char *lexema, AST *l, AST *r) {
    AST *t = calloc(1, sizeof(*t));
    char *copia = t ? malloc(strlen(lexema) + 1) : NULL;
    if (!t || !copia) {
        free(t);
        ast_free(l);
        ast_free(r);
        fprintf(stderr, "Memoria insuficiente para a arvore.\n");
        p->erro = 1;
        return NULL;
    }
    strcpy(copia, lexema);
    t->tipo = tipo;
    t->lexema = copia;
    t->esq = l;
    t->dir = r;
    return t;
}

// F -> INTEIRO | '(' E ')'
static AST *primario(Parser *p) {
    if (p->erro) return NULL;
    if (p->atual.tipo == TOKEN_INTEIRO) {
        AST *t = novo(p, TOKEN_INTEIRO, p->atual.lexema, NULL, NULL);
        consumir(p);
        return t;
    }
    if (p->atual.tipo == TOKEN_ABRE_PAR) {
        consumir(p);
        AST *t = expressao(p);
        if (!p->erro && p->atual.tipo != TOKEN_FECHA_PAR)
            erro(p, "esperado ')'");
        if (p->erro) { ast_free(t); return NULL; }
        consumir(p);
        return t;
    }
    erro(p, "esperado inteiro ou '('");
    return NULL;
}

// P -> F [ '^' U ]
static AST *potencia(Parser *p) {
    AST *l = primario(p);
    if (!p->erro && p->atual.tipo == TOKEN_POT) {
        consumir(p);
        AST *r = unario(p);
        if (p->erro) { ast_free(l); ast_free(r); return NULL; }
        l = novo(p, TOKEN_POT, "^", l, r);
    }
    return l;
}

// U -> ('+' | '-') U | P
static AST *unario(Parser *p) {
    // Aborta se o parser indicar erro
    if (p->erro) return NULL;

    // Aborta se o nível da árvore for igual ou maior que 256 (aninhamento é chique)
    if (p->nivel >= 256) {
        erro(p, "limite de aninhamento excedido");
        return NULL;
    }

    // Incrementa o nível de ⊹₊˚‧︵‿₊୨ aninhamento ୧₊‿︵‧˚₊⊹ 
    p->nivel++;
    AST *t;

    // Se o token atual for '+' ou '-' consome-o e chama-o novamente... sério?
    if (p->atual.tipo == TOKEN_MAIS || p->atual.tipo == TOKEN_MENOS) {
        TipoToken tipo = p->atual.tipo;
        consumir(p);

        // Recursividade??
        AST * r = unario(p); // ... 🤙🏻
        if (p->erro) { ast_free(r); t = NULL; }
        else t = novo(p, tipo, tipo == TOKEN_MAIS ? "+" : "-", NULL, r);
    } else t = potencia(p);
    p->nivel--;
    return t;
}

// T -> U { ('*' | '/') U }
static AST *termo(Parser *p) {
    AST *l = unario(p);
    while (!p->erro && (p->atual.tipo == TOKEN_MULT || p->atual.tipo == TOKEN_DIV)) {
        TipoToken tipo = p->atual.tipo;
        consumir(p);
        AST *r = unario(p);
        if (p->erro) { ast_free(l); ast_free(r); return NULL; }
        l = novo(p, tipo, tipo == TOKEN_MULT ? "*" : "/", l, r);
    }
    return l;
}

// E -> T { ('+' | '-') T }
static AST *expressao(Parser *p) {
    AST * l = termo(p);
    while (!p->erro && (p->atual.tipo == TOKEN_MAIS || p->atual.tipo == TOKEN_MENOS)) {
        TipoToken tipo = p->atual.tipo;
        consumir(p);
        AST *r = termo(p);
        if (p->erro) { ast_free(l); ast_free(r); return NULL; }
        l = novo(p, tipo, tipo == TOKEN_MAIS ? "+" : "-", l, r);
    }
    return l;
}

// 
AST * parse_ast(Scanner * scanner) {
    // Inicializa o parser com 0 em todos os atributos, exceto o p.scanner, que a ele é atribuído o scanner principal do programa
    Parser p = { 0 };
    p.scanner = scanner;

    // Atualiza o token atual do parser utilizando seu scanner
    consumir(&p);

    AST * raiz = expressao(&p);
    if (!p.erro && p.atual.tipo != TOKEN_FIM)
        erro(&p, "esperado operador ou fim da expressao");
    free(p.atual.lexema);
    if (p.erro) { ast_free(raiz); return NULL; }
    return raiz;
}

void ast_print(const AST *t, int profundidade) {
    if (!t) return;
    for (int i = 0; i < profundidade; i++) fputs("  ", stdout);
    if (t->tipo == TOKEN_INTEIRO) printf("INTEIRO(%s)\n", t->lexema);
    else printf("%s%s\n", t->lexema, t->esq ? "" : " (unario)");
    ast_print(t->esq, profundidade + 1);
    ast_print(t->dir, profundidade + 1);
}

static int dot_rec(const AST *t, FILE *f, int *id) {
    int atual = (*id)++;
    /* Lexemas sao apenas digitos ou operadores, sem caracteres de escape DOT. */
    fprintf(f, "  n%d [label=\"%s%s\"];\n", atual, t->lexema,
            t->tipo != TOKEN_INTEIRO && !t->esq ? " (unario)" : "");
    if (t->esq) {
        int filho = dot_rec(t->esq, f, id);
        fprintf(f, "  n%d -> n%d;\n", atual, filho);
    }
    if (t->dir) {
        int filho = dot_rec(t->dir, f, id);
        fprintf(f, "  n%d -> n%d;\n", atual, filho);
    }
    return atual;
}

int ast_to_dot(const AST *t, const char *caminho) {
    FILE *f = fopen(caminho, "w");
    if (!f) { perror(caminho); return 0; }
    fputs("digraph AST {\n  node [fontname=\"Consolas\"];\n", f);
    int id = 0;
    if (t) dot_rec(t, f, &id);
    fputs("}\n", f);
    int ok = !ferror(f);
    if (fclose(f) != 0) ok = 0;
    if (!ok) fprintf(stderr, "Falha ao salvar a arvore.\n");
    return ok;
}
