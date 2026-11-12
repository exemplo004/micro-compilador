#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct AST {
    int value;
    struct AST ** children;
    int cCount;
    int cap;
} AST;

AST * createTree(int data) {
    AST * t =  (AST *) malloc(sizeof(AST));

    t->value = data;
    t->cap = 2; t->cCount = 0;
    t->children = (AST **) malloc(t->cap * sizeof(AST *));

    return t;
}

void addNode(AST * parent, AST * child) {
    if (parent->cCount >= parent->cap) {
        printf("child limit reached\n\texpanding tree...\n");
        parent->children = realloc(parent->children, ++parent->cap * sizeof(AST *));
    }

    parent->children[parent->cCount++] = child;
}

void freeAST(AST * root) {
    if (!root) return;
    for (int i = 0; i < root->cCount; i++) {
        freeAST(root->children[i]);
    }

    free(root->children);
    root->children = NULL;
    free(root);
    root = NULL;
}