#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

int dot_rec(AST *t, FILE *f, int *nivel) {
    int atual = (*nivel)++;

    fprintf(f, "  n%i [label=\"%s\"];\n", atual, t->display);

    for (int i = 0; i < t->qtFilhos; i++) {
        int filho = dot_rec(t->filhos[i], f, nivel);
        fprintf(f, "  n%d -> n%d;\n", atual, filho);;
    }

    return atual;
}

int ast_to_dot(AST * t, char * fn) {
    FILE *f = fopen(fn, "w");
    if (!f) { printf("Erro ao abrir arquivo .dot\n"); return 1; }
    if (t == NULL) { printf("Ahrvore sintahtica vazia ou invahlida\n"); return 1;}

    fputs("digraph AST {\n  node [fontname=\"Consolas\" shape=box];\n", f);

    int nivel = 0;
    dot_rec(t, f, &nivel);

    fputs("}\n", f);
    int ok = !ferror(f);
    if (fclose(f) != 0) ok = 0;
    if (!ok) fprintf(stderr, "Falha ao salvar a arvore.\n");
    return ok;
}

// Funcão principal do analisador
int AnaliseSintatica(FILE * tokens) {
    Token vetorTokens[MAX_TOKENS] = { 0 };
    int totalTokens = extrairLexemas(tokens, vetorTokens);

    FILE * fe = fopen(ARQ_ERR_SIN, "w");
    
    // se o arquivo estiver completamente vazio ou apenas o TK_EOF
    if (totalTokens <= 1) {
        fprintf(fe, "Erro: Arquivo vazio ou sem secoes validas.\n");
        return 1;
    }

    AST * raiz = iniciar_ast(&vetorTokens[0]);

    if (!raiz) {
        printf("PROGRAMA SINTATICAMENTE INVAHLIDO\n");
        return 1;
    }
    fprintf(fe, "Nenhum erro sintático encontrado.");
    printf("PROGRAMA SINTATICAMENTE VAHLIDO\n");

    ast_to_dot(raiz, ARQ_AST);

    fclose(fe);
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

// -------------------------------------------------------------
// ---- FUNÇÕES DE ÁRVORES--------------------------------------
// -------------------------------------------------------------

// Cria um nó vazio de capacidade 1
AST * novoNoh(char * display) {
    AST * r = (AST *) malloc(sizeof(AST));

    r->display = malloc(strlen(display) + 1);
    strcpy(r->display, display);

    r->qtFilhos = 0;
    r->capacidade = 1;
    r->filhos = (AST **) malloc(r->capacidade * sizeof(AST *));

    return r;
}

// Insere um novo nó no primeiro espaço disponível, da esquerda pra direita
// Aumenta a capacidade do nó pai em 1 caso não tenha espaços disponíveis, e então insere o nó filho
void inserirNoh(AST * pai, AST * filho) {
    if (!filho) {
        printf("Noh filho eh NULL. Abortando.\n");
        return;
    }

    if (pai->qtFilhos >= pai->capacidade) {
        pai->filhos = realloc(pai->filhos, ++pai->capacidade * sizeof(AST *));
    }

    pai->filhos[pai->qtFilhos++] = filho;
}

// Libera a memória de todos os nós de uma árvore
void freeAST(AST * raiz) {
    if (!raiz) return;

    for (int i = 0; i < raiz->qtFilhos; i++) {
        freeAST(raiz->filhos[i]);
    }
    
    free(raiz->display);
    free(raiz->filhos);
    free(raiz);
}

void displayArvore(AST * raiz) {
    if (!raiz) {
        printf("{ }");
        return;
    }

    if (raiz->qtFilhos == 0) {
        printf("[ %s ]", raiz->display);
        return;
    }

    for (int i = 0; i < raiz->qtFilhos; i++) {
        displayArvore(raiz->filhos[i]);
    }

    printf("< %s >", raiz->display);
}

// Converte o nome do token (string) em TipoToken (enum) para comparações
TipoToken tipo(Token t) {
    if (!strcmp(t.nome, "REG"))         return REG;
    if (!strcmp(t.nome, "ID"))          return ID;
    if (!strcmp(t.nome, "NUM_INT"))     return NUM_INT;
    if (!strcmp(t.nome, "STRING"))      return STRING;
    if (!strcmp(t.nome, "INS_LI"))      return INS_LI;
    if (!strcmp(t.nome, "INS_LA"))      return INS_LA;
    if (!strcmp(t.nome, "INS_MOVE"))    return INS_MOVE;
    if (!strcmp(t.nome, "INS_LW"))      return INS_LW;
    if (!strcmp(t.nome, "INS_SW"))      return INS_SW;
    if (!strcmp(t.nome, "INS_LB"))      return INS_LB;
    if (!strcmp(t.nome, "INS_SB"))      return INS_SB;
    if (!strcmp(t.nome, "INS_LH"))      return INS_LH;
    if (!strcmp(t.nome, "INS_SH"))      return INS_SH;
    if (!strcmp(t.nome, "INS_ADD"))     return INS_ADD;
    if (!strcmp(t.nome, "INS_ADDI"))    return INS_ADDI;
    if (!strcmp(t.nome, "INS_ADDU"))    return INS_ADDU;
    if (!strcmp(t.nome, "INS_SUB"))     return INS_SUB;
    if (!strcmp(t.nome, "INS_SUBU"))    return INS_SUBU;
    if (!strcmp(t.nome, "INS_MUL"))     return INS_MUL;
    if (!strcmp(t.nome, "INS_DIV"))     return INS_DIV;
    if (!strcmp(t.nome, "INS_MFLO"))    return INS_MFLO;
    if (!strcmp(t.nome, "INS_MFHI"))    return INS_MFHI;
    if (!strcmp(t.nome, "INS_AND"))     return INS_AND;
    if (!strcmp(t.nome, "INS_OR"))      return INS_OR;
    if (!strcmp(t.nome, "INS_XOR"))     return INS_XOR;
    if (!strcmp(t.nome, "INS_NOR"))     return INS_NOR;
    if (!strcmp(t.nome, "INS_SLL"))     return INS_SLL;
    if (!strcmp(t.nome, "INS_SRL"))     return INS_SRL;
    if (!strcmp(t.nome, "INS_SLT"))     return INS_SLT;
    if (!strcmp(t.nome, "INS_BEQ"))     return INS_BEQ;
    if (!strcmp(t.nome, "INS_BNE"))     return INS_BNE;
    if (!strcmp(t.nome, "INS_BLT"))     return INS_BLT;
    if (!strcmp(t.nome, "INS_BLE"))     return INS_BLE;
    if (!strcmp(t.nome, "INS_BGT"))     return INS_BGT;
    if (!strcmp(t.nome, "INS_BGE"))     return INS_BGE;
    if (!strcmp(t.nome, "INS_J"))       return INS_J;
    if (!strcmp(t.nome, "INS_JAL"))     return INS_JAL;
    if (!strcmp(t.nome, "INS_JR"))      return INS_JR;
    if (!strcmp(t.nome, "INS_SYSCALL")) return INS_SYSCALL;
    if (!strcmp(t.nome, "INS_NOP"))     return INS_NOP;
    if (!strcmp(t.nome, "DIR_DATA"))    return DIR_DATA;
    if (!strcmp(t.nome, "DIR_TEXT"))    return DIR_TEXT;
    if (!strcmp(t.nome, "DIR_WORD"))    return DIR_WORD;
    if (!strcmp(t.nome, "DIR_HALF"))    return DIR_HALF;
    if (!strcmp(t.nome, "DIR_BYTE"))    return DIR_BYTE;
    if (!strcmp(t.nome, "DIR_SPACE"))   return DIR_SPACE;
    if (!strcmp(t.nome, "DIR_ASCII"))   return DIR_ASCII;
    if (!strcmp(t.nome, "DIR_ASCIIZ"))  return DIR_ASCIIZ;
    if (!strcmp(t.nome, "DIR_GLOBL"))   return DIR_GLOBL;
    if (!strcmp(t.nome, "DIR_ALIGN"))   return DIR_ALIGN;
    if (!strcmp(t.nome, "SMB_COM"))     return SMB_COM;
    if (!strcmp(t.nome, "SMB_COL"))     return SMB_COL;
    if (!strcmp(t.nome, "SMB_OPA"))     return SMB_OPA;
    if (!strcmp(t.nome, "SMB_CPA"))     return SMB_CPA;
    if (!strcmp(t.nome, "TK_EOF"))      return TK_EOF;
    if (!strcmp(t.nome, "EOL"))         return EOL;
    
    printf("O nome de token \"%s\" naum eh um tipo de token reconhecido\n", t.nome);
    exit(1);
}

// Converte o TipoToken (enum) uma string
char * nome(TipoToken t) {
    if (t == REG)         return "REG";
    if (t == ID)          return "ID";
    if (t == NUM_INT)     return "NUM_INT";
    if (t == STRING)      return "STRING";
    if (t == INS_LI)      return "INS_LI";
    if (t == INS_LA)      return "INS_LA";
    if (t == INS_MOVE)    return "INS_MOVE";
    if (t == INS_LW)      return "INS_LW";
    if (t == INS_SW)      return "INS_SW";
    if (t == INS_LB)      return "INS_LB";
    if (t == INS_SB)      return "INS_SB";
    if (t == INS_LH)      return "INS_LH";
    if (t == INS_SH)      return "INS_SH";
    if (t == INS_ADD)     return "INS_ADD";
    if (t == INS_ADDI)    return "INS_ADDI";
    if (t == INS_ADDU)    return "INS_ADDU";
    if (t == INS_SUB)     return "INS_SUB";
    if (t == INS_SUBU)    return "INS_SUBU";
    if (t == INS_MUL)     return "INS_MUL";
    if (t == INS_DIV)     return "INS_DIV";
    if (t == INS_MFLO)    return "INS_MFLO";
    if (t == INS_MFHI)    return "INS_MFHI";
    if (t == INS_AND)     return "INS_AND";
    if (t == INS_OR)      return "INS_OR";
    if (t == INS_XOR)     return "INS_XOR";
    if (t == INS_NOR)     return "INS_NOR";
    if (t == INS_SLL)     return "INS_SLL";
    if (t == INS_SRL)     return "INS_SRL";
    if (t == INS_SLT)     return "INS_SLT";
    if (t == INS_BEQ)     return "INS_BEQ";
    if (t == INS_BNE)     return "INS_BNE";
    if (t == INS_BLT)     return "INS_BLT";
    if (t == INS_BLE)     return "INS_BLE";
    if (t == INS_BGT)     return "INS_BGT";
    if (t == INS_BGE)     return "INS_BGE";
    if (t == INS_J)       return "INS_J";
    if (t == INS_JAL)     return "INS_JAL";
    if (t == INS_JR)      return "INS_JR";
    if (t == INS_SYSCALL) return "INS_SYSCALL";
    if (t == INS_NOP)     return "INS_NOP";
    if (t == DIR_DATA)    return "DIR_DATA";
    if (t == DIR_TEXT)    return "DIR_TEXT";
    if (t == DIR_WORD)    return "DIR_WORD";
    if (t == DIR_HALF)    return "DIR_HALF";
    if (t == DIR_BYTE)    return "DIR_BYTE";
    if (t == DIR_SPACE)   return "DIR_SPACE";
    if (t == DIR_ASCII)   return "DIR_ASCII";
    if (t == DIR_ASCIIZ)  return "DIR_ASCIIZ";
    if (t == DIR_GLOBL)   return "DIR_GLOBL";
    if (t == DIR_ALIGN)   return "DIR_ALIGN";
    if (t == SMB_COM)     return "SMB_COM";
    if (t == SMB_COL)     return "SMB_COL";
    if (t == SMB_OPA)     return "SMB_OPA";
    if (t == SMB_CPA)     return "SMB_CPA";
    if (t == TK_EOF)      return "TK_EOF";
    if (t == EOL)         return "EOL";
    
    printf("O tipo de token inserido naum foi reconhecido\n");
    return NULL;
}

// Retorna a string com nome e o lexema de um token para o serem o display do nó
char * t_to_d(Token t) {
    int len = strlen(t.nome) + 2 * strlen(t.lexema) + 3; 
    char * dText = (char *) malloc(len);
    if (!dText) {
        perror("Erro ao alocar memohria para string de display");
        return "ERRO MEMÓRIA";
    }

    // f é o fim da string, dependendo do tamanho dela
    char * f = dText + sprintf(dText, "%s\\n", t.nome);

    // Evita aspas para não conflitar com a gramática de arquivos .dot
    for (char * c = t.lexema; *c; c++) {
        if (*c == '"' || *c == '\\') *f++ = '\\'; // Confia em mim
        *f++ = *c;
    }
    *f = '\0';

    return dText;
}

static void erro(Parser *p, const char *mensagem) {
    if (p->erro) return;
    FILE * f = fopen(ARQ_ERR_SIN, "w");

    fprintf(f, "Erro sintahtico na linha %d, coluna %d\n\t%s;\n\tencontrado %s ('%s').\n",
        p->atual.linha, p->atual.coluna, mensagem, p->atual.nome, p->atual.lexema);
            
    p->erro = 1;
    fclose(f);
}

// Avança o parser para o próximo token
void proximo_token(Parser * p) {    
    if (p->erro) return;
    
    if (p->scanner.i >= MAX_TOKENS || p->scanner.i < 0 ) {
        erro(p, "Quantidade mahxima de tokens excedida\n");
        exit(p->scanner.i);
    }
    
    // Aborta se o nome do token atual está vazio
    if (!strcmp(&p->scanner.tokens[p->scanner.i].nome[0], "")) {
        erro(p, "Token vazio encontrado\n");
        exit(p->scanner.i);
    }
    
    // Incrementa cursor e entrega o novo token ao parser
    p->scanner.i++;
    p->atual = p->scanner.tokens[p->scanner.i];
}

// Consome um token terminal, passando para o próximo ao fim da chamada
// Retorna 0 em sucesso, 1 com erro de tipagem, 2  no caso de erro em alocação memória
// Nos casos de erro, dá freeAST no nó pai
int consumir_terminal(Parser * p, AST * nohPai, TipoToken tipoEsperado, const char * callerFunction) {
    AST * nohFilho;

    // Preparando mensagem de erro de tipo
    char * msgErro = (char *) malloc( (strlen("esperado ") + strlen(nome(tipoEsperado)) + 1) * sizeof(char) );
    sprintf(msgErro, "esperado %s", nome(tipoEsperado));
    
    if (tipo(p->atual) != tipoEsperado) {
        erro(p, msgErro);
        freeAST(nohPai);
        return 1;
    }
    
    // Preparando mensagem de erro de alocação de memória
    char * msgMemoriaInsuficiente = (char *) malloc( (strlen("memohria insuficiente em ") + strlen(callerFunction) + 1) * sizeof(char) );
    sprintf(msgMemoriaInsuficiente, "memohria insuficiente em %s", callerFunction);

    nohFilho = novoNoh(t_to_d(p->atual));
    if(!nohFilho) {
        erro(p, msgMemoriaInsuficiente);
        freeAST(nohPai); freeAST(nohFilho);
        return 2;
    }

    inserirNoh(nohPai, nohFilho);
    proximo_token(p);
    return 0;
}

AST * iniciar_ast(Token * tokens) {
    Scanner s = { tokens, 0 };

    // Inicializa o parser com 0 em todos os atributos e insere o seu primeiro token
    Parser p = { 0 };
    p.scanner = s;
    p.atual = p.scanner.tokens[p.scanner.i];
    
    // Parte da derivação <programa> como nó raíz, gerando uma árvore n-ária a ser percorrida pelo gerador do arquivo .dot
    AST * raiz = exp_programa(&p);
    if (p.erro) return NULL;

    return raiz;
}

// Verifica se o token é capaz de iniciar uma derivação <linha_dados>
int inicia_linha_dados(TipoToken t) {
    return t == ID || t == DIR_ASCII || t == DIR_ASCIIZ ||
    t == DIR_WORD  || t == DIR_HALF  || t == DIR_BYTE ||
    t == DIR_SPACE || t == DIR_ALIGN || t == DIR_GLOBL;
}

int inicia_diretiva_dados(TipoToken t) { return inicia_linha_dados(t) && t != ID && t != DIR_GLOBL; }

// Verifica o token é capaz de iniciar derivações de operações
// Serão chamadas abaixo, em inicia_instrucao()
int inicia_op_um_registrador(TipoToken t)     { return t == INS_MFLO || t == INS_MFHI || t == INS_JR; }
int inicia_op_dois_registradores(TipoToken t) { return t == INS_MOVE || t == INS_DIV; }
int inicia_op_tres_registradores(TipoToken t) { return t == INS_ADD || t == INS_ADDU || t == INS_SUB || t ==  INS_SUBU || t == INS_MUL || t == INS_SLT || t == INS_AND || t == INS_OR || t == INS_XOR || t == INS_NOR; }
int inicia_op_deslocamento(TipoToken t)       { return t == INS_SLL || t == INS_SRL; }
int inicia_op_memoria(TipoToken t)            { return t == INS_LW || t == INS_SW || t == INS_LB || t == INS_SH || t == INS_SB || t == INS_LH; }
int inicia_op_desvio(TipoToken t)             { return t == INS_BEQ || t == INS_BNE || t == INS_BLT || t == INS_BLE || t == INS_BGT || t == INS_BGE; }
int inicia_op_salto(TipoToken t)              { return t == INS_J || t == INS_JAL; }

// Verifica o token é capaz de iniciar uma derivação <instrucao>, utilizando as funções acima
int inicia_instrucao(TipoToken t) {
    return inicia_op_um_registrador(t) ||
    inicia_op_dois_registradores(t) ||
    inicia_op_tres_registradores(t) ||
    inicia_op_deslocamento(t) ||
    inicia_op_memoria(t) ||
    inicia_op_desvio(t) ||
    inicia_op_salto(t) ||
    t == INS_ADDI || t == INS_LI || t == INS_LA || t == INS_SYSCALL || t == INS_NOP;
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
    AST * a = novoNoh("<programa>");
    if (!a) { erro(p, "memohria insuficiente em exp_programa()\n"); return NULL; }
    
    do {
        AST * b = exp_secao(p);
        if (!b) {
            erro(p, "<secao> esperado");
            freeAST(a);
            freeAST(b);
            return NULL;
        }
        inserirNoh(a, b);
    } while (tipo(p->atual) != TK_EOF);

    AST * c = novoNoh("TK_EOF");
    if (!c) { erro(p, "memohria insuficiente em exp_programa()\n"); return NULL; }

    inserirNoh(a, c);
    return a;
}

// <secao_dados> | <secao_codigo>
AST * exp_secao(Parser * p) {
    AST * a = novoNoh("<secao>");
    if (!a) { erro(p, "memohria insuficiente em exp_secao()"); return NULL; }
    
    AST * b;
    if (tipo(p->atual) == DIR_DATA)              b = exp_secao_dados(p);
    else if (tipo(p->atual) == DIR_TEXT)         b = exp_secao_codigo(p);
    else { erro(p, "esperado DIR_DATA ou DIR_TEXT"); freeAST(a); return NULL; }

    if (b != NULL && !p->erro) {
        inserirNoh(a, b);
        return a;
    }

    erro(p, "esperado <secao_dados> ou <secao_codigo>");
    freeAST(a);
    return NULL;
}

// .data" EOL { <linha_dados> }
AST * exp_secao_dados(Parser * p) {
    AST * a = novoNoh("<secao_dados>");
    if (!a) { erro(p, "memohria insuficiente em exp_secao_dados()"); return NULL; }

    AST * b;
    
    if (tipo(p->atual) != DIR_DATA) {
        erro(p, "DIR_DATA esperado");
        freeAST(a);
        return NULL;
    }
    b = novoNoh(t_to_d(p->atual));
    if (!b) { erro(p, "memohria insuficiente em exp_secao_dados()"); freeAST(a); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    if(tipo(p->atual) != EOL) {
        erro(p, "EOL esperado");
        freeAST(a);
        return NULL;
    }
    b = novoNoh("EOL");
    if (!b) { erro(p, "memohria insuficiente em exp_secao_dados()"); freeAST(a); freeAST(b); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    while (inicia_linha_dados(tipo(p->atual))) {
        b = exp_linha_dados(p);
        if (!b || p->erro) { freeAST(b); return NULL; }
        inserirNoh(a, b);
    }

    return a;
}

// ".text" EOL { <linha_codigo> }
AST * exp_secao_codigo(Parser * p) {
    AST * a = novoNoh("<secao_codigo>");
    if (!a) { erro(p, "memohria insuficiente em exp_secao_codigo()"); return NULL; }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) { erro(p, "memohria insuficiente em exp_secao_codigo()"); freeAST(a); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    if (tipo(p->atual) != EOL) {
        erro(p, "esperado EOL");
        freeAST(a);
        return NULL;
    }
    b = novoNoh("EOL");
    if (!b) { erro(p, "memohria insuficiente em exp_secao_codigo()"); freeAST(a); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    while (!p->erro && (tipo(p->atual) == ID || inicia_instrucao(tipo(p->atual)) || tipo(p->atual) == DIR_GLOBL)) {
        b = exp_linha_codigo(p);
        if (!b || p->erro) { freeAST(b); return NULL; }
        inserirNoh(a, b);
    }

    return a;
}

// <rotulo> [ <diretiva_dados> ] EOL | <diretiva_dados> EOL | <diretiva_global> EOL
AST * exp_linha_dados(Parser * p) {
    AST * a = novoNoh("<linha_dados>");
    if (!a) { erro(p, "memohria insuficiente em exp_linha_dados()"); return NULL; }

    AST * b;

    // Verificando qual o novo nó baseado no token atual
    if (tipo(p->atual) == ID) {                           // Verificando <rotulo>
        b = exp_rotulo(p);
        if (b != NULL && !p->erro) inserirNoh(a, b);
        else return NULL;

        if (inicia_diretiva_dados(tipo(p->atual))) {
            b = exp_diretiva_dados(p);
            if (b != NULL && !p->erro) inserirNoh(a, b);
            else return NULL;
        }
    }
    else if (inicia_diretiva_dados(tipo(p->atual))) {     // Verificando <diretiva_dados>
        b = exp_diretiva_dados(p);
        if (b != NULL && !p->erro) inserirNoh(a, b);
        else return NULL;
    }
    else if (tipo(p->atual) == DIR_GLOBL) {               // Verificando <diretiva_global>
        b = exp_diretiva_global(p);
        if (b != NULL && !p->erro) inserirNoh(a, b);
        else return NULL;
    } else {
        erro(p, "esperado ID, ou <diretiva_dados>, ou DIR_GLOBL");
        freeAST(a);
        return NULL;
    }

    // Verificando EOL
    if (tipo(p->atual) != EOL) {
        erro(p, "EOL esperado");
        freeAST(a);
        return NULL;
    }
    b = novoNoh("EOL");
    if (!b) { erro(p, "memohria insuficiente em exp_secao_dados()"); freeAST(a); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// <rotulo> [ <instrucao> ] EOL | <instrucao> EOL | <diretiva_global> EOL
AST * exp_linha_codigo(Parser * p) {
    AST * a = novoNoh("<linha_codigo>");
    if (!a) { erro(p, "memohria insuficiente em exp_linha_codigo()"); return NULL; }

    AST * b;

    // Verificando qual o novo nó baseado no token atual
    if (tipo(p->atual) == ID) {                            // Verificando <rotulo>
        b = exp_rotulo(p);

        if (!b || p->erro) { freeAST(a); freeAST(b); return NULL; }
        inserirNoh(a, b);
        
        if (inicia_instrucao(tipo(p->atual))) {            // Verificando <rotulo> [ <instrucao> ]
            b = exp_instrucao(p);
            
            if (!b || p->erro) { freeAST(a); return NULL; }
            inserirNoh(a, b);
        }
    } else if (inicia_instrucao(tipo(p->atual))) {         // Verificando <instrucao>
        b = exp_instrucao(p);

        if (!b || p->erro) { freeAST(a); return NULL; }
        inserirNoh(a, b);
    } else if (tipo(p->atual) == DIR_GLOBL) {              // Verificando <diretiva_global>
        b = exp_diretiva_global(p);

        if (!b || p->erro) { freeAST(a); return NULL; }
        inserirNoh(a, b);
    } else {
        erro(p, "esperado ID, ou DIR_GLOBL ou <instrucao>");
        freeAST(a); return NULL;
    }

    // Verificando EOL
    if (tipo(p->atual) != EOL) {
        erro(p, "EOL esperado");
        freeAST(a); return NULL;
    }
    b = novoNoh("EOL");
    if (!b) { erro(p, "memohria insuficiente em exp_linha_codigo()"); return NULL; }
    inserirNoh(a, b); proximo_token(p);

    return a;
}

// ID ":"
AST * exp_rotulo(Parser * p) {
    AST * a = novoNoh("<rotulo>");
    if (!a) { erro(p, "memohria insuficiente em exp_rotulo()"); return NULL; }

    // exp_rotulo() só é chamada a partir do token ID, então ele já é esperado
    AST * b = novoNoh(t_to_d(p->atual));
    if(!b) { erro(p, "memohria insuficiente em exp_rotulo()"); freeAST(a); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    // Verificando ":"
    if (tipo(p->atual) != SMB_COL) {
        erro(p, "esperado \":\"");
        freeAST(a); return NULL;
    }
    b = novoNoh(t_to_d(p->atual));
    if(!b) { erro(p, "memohria insuficiente em exp_rotulo()"); freeAST(a); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// ".globl" ID
AST * exp_diretiva_global(Parser * p) {
    AST * a = novoNoh("<diretiva_global>");
    if (!a) { erro(p, "memohria insuficiente em exp_diretiva_global()"); return NULL; }

    if(consumir_terminal(p, a, DIR_GLOBL, __func__)) return NULL;
    if(consumir_terminal(p, a, ID, __func__)) return NULL;
    
    return a;
}

// ---- 2.3. Declarações de dados
// <diretiva_inteiros> <lista_inteiros> | <diretiva_string> STRING | ".space" NUM_INT | ".align" NUM_INT
AST * exp_diretiva_dados(Parser * p) {
    AST * a = novoNoh("<diretiva_dados>");
    if (!a) { erro(p, "memohria insuficiente em exp_diretiva_dados()"); return NULL; }

    AST * b;
    if (tipo(p->atual) == DIR_WORD || tipo(p->atual) == DIR_HALF || tipo(p->atual) == DIR_BYTE) { // Verificando <diretiva_inteiros> <lista_inteiros>
        b = exp_diretiva_inteiros(p);
        if (!p->erro && b != NULL) { inserirNoh(a, b); }
        else return NULL;

        b = exp_lista_inteiros(p);
        if (!p->erro && b != NULL) { inserirNoh(a, b); }
        else { freeAST(a); return NULL; }
    } else if (tipo(p->atual) == DIR_ASCII || tipo(p->atual) == DIR_ASCIIZ) { // Verificando <diretiva_string> STRING
        b = exp_diretiva_string(p);
        if (!p->erro && b != NULL) { inserirNoh(a, b); }
        else return NULL;

        if (tipo(p->atual) != STRING) {
            erro(p, "esperado STRING");
            freeAST(a); return NULL;
        } else {
            b = novoNoh(t_to_d(p->atual));
            if (b != NULL) { inserirNoh(a, b); proximo_token(p); }
            else { freeAST(a); return NULL; }
        }
    } else if (tipo(p->atual) == DIR_SPACE || tipo(p->atual) == DIR_ALIGN) { // Verificando ".space" NUM_INT ou ".align" NUM_INT
        b = novoNoh(t_to_d(p->atual));
        if (b != NULL) { inserirNoh(a, b); proximo_token(p); }
        else { freeAST(a); return NULL; }
        
        if (tipo(p->atual) != NUM_INT) {
            erro(p, "esperado NUM_INT");
            freeAST(a); return NULL;
        } else {
            b = novoNoh(t_to_d(p->atual));
            if (b != NULL) { inserirNoh(a, b); proximo_token(p); }
            else { freeAST(a); return NULL; }
        }
    } else {
        erro(p, "esperado <diretiva_inteiros>, ou <diretiva_string>, ou \".space\", ou \".align\"");
        freeAST(a);
        return NULL;
    }

    return a;
}

// ".word" | ".half" | ".byte"
AST * exp_diretiva_inteiros(Parser * p) {
    AST * a = novoNoh("<diretiva_inteiros>");
    if (!a) { erro(p, "memohria insuficiente em exp_diretiva_inteiros()"); return NULL; }
    
    AST * b;
    if (tipo(p->atual) != DIR_WORD && tipo(p->atual) != DIR_HALF && tipo(p->atual) != DIR_BYTE) {
        erro(p, "\".word\" ou \".half\" ou \".byte\" esperado");
        freeAST(a); return NULL;
    }
    b = novoNoh(t_to_d(p->atual));
    
    if (!b) {
        erro(p, "memohria insuficiente em exp_diretiva_inteiros()");
        freeAST(a); freeAST(b); return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// NUM_INT { "," NUM_INT }
AST * exp_lista_inteiros(Parser * p) {
    AST * a = novoNoh("<lista_inteiros>");
    AST * b = tipo(p->atual) == NUM_INT ? novoNoh(t_to_d(p->atual)) : NULL;

    if (!a || !b) {
        erro(p, "memohria insuficiente em exp_lista_inteiros() ou NUM_INT esperado");
        freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    // Verificando inteiros adicionais
    while (tipo(p->atual) == SMB_COM) {
        b = novoNoh(t_to_d(p->atual));
        if (b != NULL) { inserirNoh(a, b); proximo_token(p); }
        else { freeAST(a); freeAST(b); return NULL; }

        b = tipo(p->atual) == NUM_INT ? novoNoh(t_to_d(p->atual)) : NULL;
        if(!b) {
            erro(p, "NUM_INT esperado");
            freeAST(a); return NULL;
        } else { inserirNoh(a, b); proximo_token(p); }
    }

    return a;
}

// ".ascii" | ".asciiz"
AST * exp_diretiva_string(Parser * p) {
    AST * a = novoNoh("<diretiva_string>");
    if (!a) { erro(p, "memohria insuficiente em exp_diretiva_string()"); return NULL; }

    if (tipo(p->atual) != DIR_ASCII && tipo(p->atual) != DIR_ASCIIZ) {
        erro(p, "\".ascii\" ou \".asciiz\" esperado");
        freeAST(a); return NULL;
    }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_diretiva_string()");
        freeAST(a); freeAST(b); return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// -------------------------
// ---- 2.4. Instruções ----
// -------------------------

// <instrucao_tres_registradores> | <instrucao_dois_registradores> | <instrucao_um_registrador> | <instrucao_imediato> | <instrucao_deslocamento> | <instrucao_li> | <instrucao_la> | <instrucao_memoria> | <instrucao_desvio> | <instrucao_salto> | <instrucao_sem_operandos>
AST * exp_instrucao(Parser * p) {
    AST * a = novoNoh("<instrucao>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao()"); return NULL; }

    AST * b;

    if      (inicia_op_tres_registradores(tipo(p->atual)))               b = exp_instrucao_tres_registradores(p);
    else if (inicia_op_dois_registradores(tipo(p->atual)))               b = exp_instrucao_dois_registradores(p);
    else if (inicia_op_um_registrador(tipo(p->atual)))                   b = exp_instrucao_um_registrador(p);
    else if (tipo(p->atual) == INS_ADDI)                                 b = exp_instrucao_imediato(p);
    else if (inicia_op_deslocamento(tipo(p->atual)))                     b = exp_instrucao_deslocamento(p);
    else if (tipo(p->atual) == INS_LI)                                   b = exp_instrucao_li(p);
    else if (tipo(p->atual) == INS_LA)                                   b = exp_instrucao_la(p);
    else if (inicia_op_memoria(tipo(p->atual)))                          b = exp_instrucao_memoria(p);
    else if (inicia_op_desvio(tipo(p->atual)))                           b = exp_instrucao_desvio(p);
    else if (inicia_op_salto(tipo(p->atual)))                            b = exp_instrucao_salto(p);
    else if (tipo(p->atual) == INS_SYSCALL || tipo(p->atual) == INS_NOP) b = exp_instrucao_sem_operandos(p);
    else {
        erro(p, "esperado algum token de instrussaum");
        freeAST(a); return NULL;
    }

    if(p->erro || !b) { freeAST(a); freeAST(b); return NULL; }
    inserirNoh(a, b);

    return a;
}

// <op_tres_registradores> REG "," REG "," REG
AST * exp_instrucao_tres_registradores(Parser * p) {
    AST * a = novoNoh("<instrucao_tres_registradores>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_tres_registradores()"); return NULL; }

    AST * b = exp_op_tres_registradores(p);

    if (p->erro || !b) { freeAST(a); freeAST(b); return NULL; }
    inserirNoh(a, b);

    // Verificando REG
    if (consumir_terminal(p, a, REG, __func__)) return NULL;

    // Verificando "," REG "," REG
    for (int i = 0; i < 2; i++) {
        if (consumir_terminal(p, a, SMB_COM, __func__)) return NULL;
        if (consumir_terminal(p, a, REG,     __func__)) return NULL;
    }

    return a;
}

// "add" | "addu" | "sub" | "subu" | "mul" | "and" | "or" | "xor" | "nor" | "slt"
AST * exp_op_tres_registradores(Parser * p) {
    AST * a = novoNoh("<op_tres_registradores>");
    if (!a) { erro(p, "memohria insuficiente em exp_op_tres_registradores()"); return NULL; }

    if (!inicia_op_tres_registradores(tipo(p->atual))) {
        erro(p, "esperado <op_tres_registradores>");
        freeAST(a); return NULL;
    }
    
    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_op_tres_registradores()");
        freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// <op_dois_registradores> REG "," REG
AST * exp_instrucao_dois_registradores(Parser * p) {
    AST * a = novoNoh("<instrucao_dois_registradores>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_dois_registradores()"); return NULL; }

    AST * b = exp_op_dois_registradores(p);

    if (p->erro || !b) { freeAST(a); freeAST(b); return NULL; }
    inserirNoh(a, b);

    // Verificando REG "," REG
    if (consumir_terminal(p, a, REG,     __func__)
    ||  consumir_terminal(p, a, SMB_COM, __func__)
    ||  consumir_terminal(p, a, REG,     __func__)
    ) return NULL;

    return a;
}

// "move" | "div"
AST * exp_op_dois_registradores(Parser * p) {
    AST * a = novoNoh("<op_dois_registradores>");
    if (!a) { erro(p, "memohria insuficiente em exp_op_dois_registradores()"); return NULL; }
    
    if (!inicia_op_dois_registradores(tipo(p->atual))) {
        erro(p, "esperado <op_dois_registradores>");
        freeAST(a); return NULL;
    }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_op_dois_registradores()");
        freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// <op_um_registrador> REG
AST * exp_instrucao_um_registrador(Parser * p) {
    AST * a = novoNoh("<instrucao_um_registrador>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_um_registrador()"); return NULL; }

    AST * b = exp_op_um_registrador(p);

    if (p->erro || !b) { freeAST(a); freeAST(b); return NULL; }
    inserirNoh(a, b);

    // Verificando REG
    if (consumir_terminal(p, a, REG,     __func__)) return NULL;

    return a;
}

// "mflo" | "mfhi" | "jr"
AST * exp_op_um_registrador(Parser * p) {
    AST * a = novoNoh("<op_um_registrador>");
    if (!a) { erro(p, "memohria insuficiente em exp_op_um_registrador()"); return NULL; }
    
    if (!inicia_op_um_registrador(tipo(p->atual))) {
        erro(p, "esperado <op_um_registrador>");
        freeAST(a); return NULL;
    }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_op_um_registrador()");
        freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// "addi" REG "," REG "," NUM_INT
AST * exp_instrucao_imediato(Parser * p) {
    AST * a = novoNoh("<instrucao_imediato>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_imediato()"); return NULL; }

    if(consumir_terminal(p, a, INS_ADDI, __func__)
    || consumir_terminal(p, a, REG,      __func__)
    || consumir_terminal(p, a, SMB_COM,  __func__)
    || consumir_terminal(p, a, REG,      __func__)
    || consumir_terminal(p, a, SMB_COM,  __func__)
    || consumir_terminal(p, a, NUM_INT,  __func__)
    ) return NULL;

    return a;
}

// <op_deslocamento> REG "," REG "," NUM_INT
AST * exp_instrucao_deslocamento(Parser * p) {
    AST * a = novoNoh("<instrucao_deslocamento>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_deslocamento()"); return NULL; }

    AST * b = exp_op_deslocamento(p);

    if (p->erro || !b) { freeAST(a); freeAST(b); return NULL; }
    inserirNoh(a, b);

    // Verificando REG "," REG "," NUM_INT
    if (consumir_terminal(p, a, REG,     __func__)
    ||  consumir_terminal(p, a, SMB_COM, __func__)
    ||  consumir_terminal(p, a, REG,     __func__)
    ||  consumir_terminal(p, a, SMB_COM, __func__)
    ||  consumir_terminal(p, a, NUM_INT, __func__)
    ) return NULL;

    return a;
}

// "sll" | "srl"
AST * exp_op_deslocamento(Parser * p) {
    AST * a = novoNoh("<op_deslocamento>");
    if (!a) { erro(p, "memohria insuficiente em exp_op_deslocamento()"); return NULL; }
    
    if (!inicia_op_deslocamento(tipo(p->atual))) {
        erro(p, "esperado INS_SLL ou INS_SRL");
        freeAST(a); return NULL;
    }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_op_deslocamento()");
        freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// "li" REG "," NUM_INT
AST * exp_instrucao_li(Parser * p) { // sei que não é a melhor função, mas tô testando umas coisas
    AST * a = novoNoh("<instrucao_li>");
    AST * b = novoNoh(t_to_d(p->atual));
    if (!a || !b) { erro(p, "memohria insuficiente em exp_instrucao_li()"); return NULL; }
    inserirNoh(a, b); proximo_token(p);

    AST *c, *d, *e;
    c = tipo(p->atual) == REG     ? novoNoh(t_to_d(p->atual)) : NULL; proximo_token(p); // Yeah I know this bit looks
    d = tipo(p->atual) == SMB_COM ? novoNoh(t_to_d(p->atual)) : NULL; proximo_token(p); // dumb after I created t_to_d()
    e = tipo(p->atual) == NUM_INT ? novoNoh(t_to_d(p->atual)) : NULL; proximo_token(p); // but time is short and YOLO!

    if (!c || !d || !e) {
        erro(p, "erro ao criar derivassaum <instrucao_li>");
        freeAST(c); freeAST(d); freeAST(e);
        return NULL;
    }
    inserirNoh(a, c); inserirNoh(a, d); inserirNoh(a, e);

    return a;
}

// "la" REG "," ID
AST * exp_instrucao_la(Parser * p) {
    AST * a = novoNoh("<instrucao_la>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_la()"); return NULL; }

    if (consumir_terminal(p, a, INS_LA,  __func__)
    ||  consumir_terminal(p, a, REG,     __func__)
    ||  consumir_terminal(p, a, SMB_COM, __func__)
    ||  consumir_terminal(p, a, ID,      __func__)
    ) return NULL;

    return a;
}

// <op_memoria> REG "," <endereco>
AST * exp_instrucao_memoria(Parser * p) {
    AST * a = novoNoh("<instrucao_memoria>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_memoria()"); return NULL; }

    if (!inicia_op_memoria(tipo(p->atual))) {
        erro(p, "<op_memoria> esperado");
        freeAST(a); return NULL;
    }

    AST * b = exp_op_memoria(p);
    if (!p->erro && b != NULL) inserirNoh(a, b);
    else {
        freeAST(a); freeAST(b);
        return NULL;
    }

    // Verificando REG ","
    if (consumir_terminal(p, a, REG, __func__)
    ||  consumir_terminal(p, a, SMB_COM, __func__)
    ) return NULL;

    // Verificando <endereco>
    if (tipo(p->atual) != ID && tipo(p->atual) != NUM_INT) {
        erro(p, "<endereco> esperado");
        freeAST(a); return NULL;
    }
    b = exp_endereco(p);
    if (!p->erro && b != NULL) inserirNoh(a, b);
    else { freeAST(a); return NULL; }

    return a;
}

// "lw" | "sw" | "lb" | "sb" | "lh" | "sh"
AST * exp_op_memoria(Parser * p) {
    AST * a = novoNoh("<op_memoria>");
    if (!a) { erro(p, "memohria insuficiente em exp_op_memoria()"); return NULL; }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_op_memoria()");
       freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b); proximo_token(p);

    return a;
}

// ID | NUM_INT "(" REG ")"
AST * exp_endereco(Parser * p) {
    AST * a = novoNoh("<endereco>");
    if (!a) { erro(p, "memohria insuficiente em exp_endereco()"); return NULL; }

    
    if (tipo(p->atual) == ID) { // Verificando ID
        if (consumir_terminal(p, a, ID, __func__)) return NULL;
    } else { // Função só é chamada caso seja ID ou NUM_INT, então, se não é ID...
        if (consumir_terminal(p, a, NUM_INT, __func__)
        ||  consumir_terminal(p, a, SMB_OPA, __func__)
        ||  consumir_terminal(p, a, REG,     __func__)
        ||  consumir_terminal(p, a, SMB_CPA, __func__)
        ) return NULL;
    }

    return a;
}

// <op_desvio> REG "," REG "," ID
AST * exp_instrucao_desvio(Parser * p) {
    AST * a = novoNoh("<instrucao_desvio>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_desvio()"); return NULL; }

    AST * b = exp_op_desvio(p);

    if (!p->erro && b != NULL) inserirNoh(a, b);
    else { freeAST(a); freeAST(b); return NULL; }

    // Verificando REG "," REG "," ID
    if (consumir_terminal(p, a, REG,     __func__)
    ||  consumir_terminal(p, a, SMB_COM, __func__)
    ||  consumir_terminal(p, a, REG,     __func__)
    ||  consumir_terminal(p, a, SMB_COM, __func__)
    ||  consumir_terminal(p, a, ID,      __func__)
    ) return NULL;

    return a;
}

// "beq" | "bne" | "blt" | "ble" | "bgt" | "bge"
AST * exp_op_desvio(Parser * p) {
    AST * a = novoNoh("<op_desvio>");
    if (!a) { erro(p, "memohria insuficiente em exp_op_desvio()"); return NULL; }
    
    if (!inicia_op_desvio(tipo(p->atual))) {
        erro(p, "esperado INS_SLL ou INS_SRL");
        freeAST(a); return NULL;
    }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_op_desvio()");
        freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b);
    proximo_token(p);

    return a;
}

// <op_salto> ID
AST * exp_instrucao_salto(Parser * p) {
    AST * a = novoNoh("<instrucao_salto>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_salto()"); return NULL; }

    AST * b = exp_op_salto(p);

    if (!p->erro && b != NULL) inserirNoh(a, b);
    else { freeAST(a); freeAST(b); return NULL; }

    // Verificando ID
    if (consumir_terminal(p, a, ID, __func__)) return NULL;

    return a;
}

// "j" | "jal"
AST * exp_op_salto(Parser * p) {
    AST * a = novoNoh("<op_salto>");
    if (!a) { erro(p, "memohria insuficiente em exp_op_salto()"); return NULL; }

    if (!inicia_op_salto(tipo(p->atual))) {
        erro(p, "esperado INS_J ou INS_JAL");
        freeAST(a); return NULL;
    }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_op_salto()");
       freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b); proximo_token(p);

    return a;
}

// “syscall” | “nop”
AST * exp_instrucao_sem_operandos(Parser * p) {
    AST * a = novoNoh("<instrucao_sem_operandos>");
    if (!a) { erro(p, "memohria insuficiente em exp_instrucao_sem_operandos()"); return NULL; }

    if (tipo(p->atual) != INS_SYSCALL && tipo(p->atual) != INS_NOP) {
        erro(p, "INS_SYSCALL ou INS_NOP esperado");
        freeAST(a); return NULL;
    }

    AST * b = novoNoh(t_to_d(p->atual));
    if (!b) {
        erro(p, "memohria insuficiente em exp_instrucao_sem_operandos()");
        freeAST(a); freeAST(b);
        return NULL;
    }
    inserirNoh(a, b); proximo_token(p);
    
    return a;
}

void exibirTokens(Token * t, int total) {
    int i = 0;
    while (i < total) {
        printf("%i. %s\t%s\t%i\t%i\n", i, t[i].nome, t[i].lexema, t[i].linha, t[i].coluna);
        i++;
    }
}

