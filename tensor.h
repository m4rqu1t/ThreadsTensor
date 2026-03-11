#ifndef TENSOR_H
#define TENSOR_H

#include <stdio.h>

typedef struct Node {
    int **matriz;
    int colunas;
    int linhas;
} Node;

Node lerArquivo(FILE *arquivo);
Node produtoTensorial(Node A, Node B);
void liberarMatriz(Node matriz);

#endif
