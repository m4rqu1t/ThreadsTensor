#ifndef TENSOR_H
#define TENSOR_H

#include <stdio.h>

typedef struct Node {
    int **matriz;
    int colunas;
    int linhas;
} Node;

typedef struct argsThread1D{
    Node a;
    Node b;
    Node *resultado;
    int initLinha;
    int fimLinha;
}argsThread1D;

typedef struct argsThread2D {
    Node a;
    Node b;
    Node *resultado;
    int initLinha;
    int fimLinha;
    int initColuna;
    int fimColuna;
} argsThread2D;

Node lerArquivo(FILE *arquivo);
Node produtoTensorial(Node A, Node B);
Node produtoTensorial_Pthreads1D(Node A, Node B, int num_threads);
Node produtoTensorial_Pthreads2D(Node A, Node B, int num_threads);
void liberarMatriz(Node matriz);

#endif
