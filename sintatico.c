#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sintatico.h"

// TODO: fazer uma função que compare a string nome de um token com o enum TipoToken
// !! Ver função nome_token() em ./materiais/lexico-sintatico-prof/lexico.c

// Funcão principal do analisador
int AnaliseSintatica(FILE * tokens, FILE * arvore, FILE * erros) {
    Token vetorTokens[MAX_TOKENS] = { 0 };
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

    printf("inserindo noh em %s\n", pai->display); // DONKEY debug
    if (pai->qtFilhos >= pai->capacidade) {
        printf("Limite de filhos para o noh %s excedido. Ampliando capacidade...\n", pai->display);
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

static void erro(Parser *p, const char *mensagem) {
    if (p->erro) return;
    fprintf(stderr, "Erro sintahtico na linha %d, coluna %d: %s; encontrado %s ('%s').\n",
            p->atual.linha, p->atual.coluna, mensagem,
            p->atual.nome, p->atual.lexema);
    p->erro = 1;
}

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
    printf("NOVO TOKEN (%i)\t%s\n", p->scanner.i, p->atual.nome); // DONKEY
}

void iniciar_ast(Token * tokens) {
    Scanner s = { tokens, 0 };
    printf("token[%i]: %s\n", s.i, tokens[s.i].nome); // DONKEY

    // Inicializa o parser com 0 em todos os atributos e insere o seu primeiro token
    Parser p = { 0 };
    p.scanner = s;
    p.atual = p.scanner.tokens[p.scanner.i];
    
    AST * raiz = novoNoh("<start>");
    inserirNoh(raiz, exp_programa(&p));

    // AST * raiz = exp_programa(&p);

    // if (!p.erro && p.atual.tipo != TOKEN_FIM)
    //     erro(&p, "esperado operador ou fim da expressao");
    // free(p.atual.lexema);
    // if (p.erro) { free_ast(raiz); return NULL; }
    // return raiz;
}

// Verifica se o token é capaz de iniciar uma derivação <linha_dados>
int inicia_linha_dados(TipoToken t) {
    return t == ID || t == DIR_ASCII || t == DIR_ASCII ||
    t == DIR_WORD  || t == DIR_HALF  || t == DIR_BYTE ||
    t == DIR_SPACE || t == DIR_ALIGN || t == DIR_GLOBL;
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
    printf("exp_programa()!\n"); // DONKEY function-call
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
        scanf("%d", &a->qtFilhos); // DONKEY WAIT FOR INPUT
    } while (tipo(p->atual) != TK_EOF);

    AST * c = novoNoh("TK_EOF");
    if (!c) { erro(p, "memohria insuficiente em exp_programa()\n"); return NULL; }

    inserirNoh(a, c);
    return a;
}

// <secao_dados> | <secao_codigo>
AST * exp_secao(Parser * p) {
    printf("exp_secao()!\n"); // DONKEY function-call
    AST * a = novoNoh("<secao>");
    if (!a) { erro(p, "memohria insuficiente em exp_secao()"); return NULL; }
    
    AST * b;
    if (tipo(p->atual) == DIR_DATA) { b = exp_secao_dados(p);}
    else if (tipo(p->atual) == DIR_TEXT) { b = exp_secao_codigo(p);}
    else { erro(p, "esperado DIR_DATA ou DIR_TEXT"); freeAST(a); return NULL; }

    if (b != NULL && !p->erro) {
        inserirNoh(a, b);
        return a;
    }

    erro(p, "esperado <secao_dados> ou <secao_codigo>");
    freeAST(a); freeAST(b);
    return NULL;
}

// .data" EOL { <linha_dados> }
AST * exp_secao_dados(Parser * p) {
    printf("exp_secao_dados()!\n"); // DONKEY function-call
    AST * a = novoNoh("<secao_dados>");
    if (!a) { erro(p, "memohria insuficiente em exp_secao_dados()"); return NULL; }

    if (tipo(p->atual) != DIR_DATA) {
        erro(p, "DIR_DATA esperado");
        freeAST(a);
        return NULL;
    }
    AST * b = novoNoh("DIR_DATA");
    if (!b) { erro(p, "memohria insuficiente em exp_secao_dados()"); freeAST(a); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    if(tipo(p->atual) != EOL) {
        erro(p, "EOL esperado");
        freeAST(a); freeAST(b);
        return NULL;
    }
    b = novoNoh("EOL");
    if (!b) { erro(p, "memohria insuficiente em exp_secao_dados()"); freeAST(a); freeAST(b); return NULL; }
    inserirNoh(a, b);
    proximo_token(p);

    while (inicia_linha_dados(tipo(p->atual))) {
        b = exp_linha_dados(p);
        if (!b) { freeAST(b); return NULL; }
        inserirNoh(a, b);
    }

    return a;
}

// ".text" EOL { <linha_codigo> }
AST * exp_secao_codigo(Parser * p) {
    printf("implementar função exp_secao_codigo\n"); // TODO
    exit(1);
}

// <rotulo> [ <diretiva_dados> ] EOL | <diretiva_dados> EOL | <diretiva_global> EOL
AST * exp_linha_dados(Parser * p) { // CONTINUE
    printf("implementar função exp_linha_dados\n"); // TODO
    exit(1);
}

// <rotulo> [ <instrucao> ] EOL | <instrucao> EOL | <diretiva_global> EOL
AST * exp_linha_codigo(Parser * p) {
    printf("implementar função exp_linha_codigo\n"); // TODO
    exit(1);
}

// ID ":"
AST * exp_rotulo(Parser * p) {
    printf("implementar função exp_rotulo\n"); // TODO
    exit(1);
}

// ".globl" ID
AST * exp_diretiva_global(Parser * p) {
    printf("implementar função exp_diretiva_global\n"); // TODO
    exit(1);
}

// ---- 2.3. Declarações de dados
// <diretiva_inteiros> <lista_inteiros> | <diretiva_string> STRING | ".space" NUM_INT | ".align" NUM_INT
AST * exp_diretiva_dados(Parser * p) {
    printf("implementar função exp_diretiva_dados\n"); // TODO
    exit(1);
}

// ".word" | ".half" | ".byte"
AST * exp_diretiva_inteiros(Parser * p) {
    printf("implementar função exp_diretiva_inteiros\n"); // TODO
    exit(1);
}

// NUM_INT { "," NUM_INT }
AST * exp_lista_inteiros(Parser * p) {
    printf("implementar função exp_lista_inteiros\n"); // TODO
    exit(1);
}

// ".ascii" | ".asciiz"
AST * exp_diretiva_string(Parser * p) {
    printf("implementar função exp_diretiva_string\n"); // TODO
    exit(1);
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
    exit(1);
}

// <op_tres_registradores> REG "," REG "," REG
AST * exp_instrucao_tres_registradores(Parser * p) {
    printf("implementar função exp_instrucao_tres_registradores\n"); // TODO
    exit(1);
}

// "add" | "addu" | "sub" | "subu" | "mul" | "and" | "or" | "xor" | "nor" | "slt"
AST * exp_op_tres_registradores(Parser * p) {
    printf("implementar função exp_op_tres_registradores\n"); // TODO
    exit(1);
}

// <op_dois_registradores> REG "," REG
AST * exp_instrucao_dois_registradores(Parser * p) {
    printf("implementar função exp_instrucao_dois_registradores\n"); // TODO
    exit(1);
}

// "move" | "div"
AST * exp_op_dois_registradores(Parser * p) {
    printf("implementar função exp_op_dois_registradores\n"); // TODO
    exit(1);
}

// <op_um_registrador> REG
AST * exp_instrucao_um_registrador(Parser * p) {
    printf("implementar função exp_instrucao_um_registrador\n"); // TODO
    exit(1);
}

// "mflo" | "mfhi" | "jr"
AST * exp_op_um_registrador(Parser * p) {
    printf("implementar função exp_op_um_registrador\n"); // TODO
    exit(1);
}

// "addi" REG "," REG "," NUM_INT
AST * exp_instrucao_imediato(Parser * p) {
    printf("implementar função exp_instrucao_imediato\n"); // TODO
    exit(1);
}

// <op_deslocamento> REG "," REG "," NUM_INT
AST * exp_instrucao_deslocamento(Parser * p) {
    printf("implementar função exp_instrucao_deslocamento\n"); // TODO
    exit(1);
}

// "sll" | "srl"
AST * exp_op_deslocamento(Parser * p) {
    printf("implementar função exp_op_deslocamento\n"); // TODO
    exit(1);
}

// "li" REG "," NUM_INT
AST * exp_instrucao_li(Parser * p) {
    printf("implementar função exp_instrucao_li\n"); // TODO
    exit(1);
}

// "la" REG "," ID
AST * exp_instrucao_la(Parser * p) {
    printf("implementar função exp_instrucao_la\n"); // TODO
    exit(1);
}

// <op_memoria> REG "," <endereco>
AST * exp_instrucao_memoria(Parser * p) {
    printf("implementar função exp_instrucao_memoria"); // TODO
    exit(1);
}

// "lw" | "sw" | "lb" | "sb" | "lh" | "sh"
AST * exp_op_memoria(Parser * p) {
    printf("implementar função exp_op_memoria"); // TODO
    exit(1);
}

// ID | NUM_INT "(" REG ")"
AST * exp_endereco(Parser * p) {
    printf("implementar função exp_endereco"); // TODO
    exit(1);
}

// <op_desvio> REG "," REG "," ID
AST * exp_instrucao_desvio(Parser * p) {
    printf("implementar função exp_instrucao_desvio"); // TODO
    exit(1);
}

// "beq" | "bne" | "blt" | "ble" | "bgt" | "bge"
AST * exp_op_desvio(Parser * p) {
    printf("implementar função exp_op_desvio"); // TODO
    exit(1);
}

// <op_salto> ID
AST * exp_instrucao_salto(Parser * p) {
    printf("implementar função exp_instrucao_salto"); // TODO
    exit(1);
}

// "j" | "jal"
AST * exp_op_salto(Parser * p) {
    printf("implementar função exp_op_salto"); // TODO
    exit(1);
}

// <instrucao_sem_operandos> ::= “syscall” | “nop”
AST * exp_instrucao_sem_operandos(Parser * p) {
    printf("implementar função exp_instrucao_sem_operandos"); // TODO
    exit(1);
}

void exibirTokens(Token * t, int total) {
    int i = 0;
    while (i < total) {
        printf("%i. %s\t%s\t%i\t%i\n", i, t[i].nome, t[i].lexema, t[i].linha, t[i].coluna);
        i++;
    }
}

