#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#include "tensor.h"



void* worker_Pthreads1D(void* arg) {

    argsThread1D* args = (argsThread1D*)arg;

    Node A = args->a;
    Node B = args->b;
    Node *res = args->resultado;

    for (int i = args->initLinha; i < args->fimLinha; i++) {
        for (int j = 0; j < res->colunas; j++) {

            int linhaA = i / B.linhas;
            int colunaA = j / B.colunas;
            int linhaB = i % B.linhas;
            int colunaB = j % B.colunas;

            res->matriz[i][j] = A.matriz[linhaA][colunaA] * B.matriz[linhaB][colunaB];
        }
    }

    return NULL;
}

Node produtoTensorial_Pthreads1D(Node A, Node B, int num_threads) {
    Node matrizResult;
    matrizResult.linhas = A.linhas * B.linhas;
    matrizResult.colunas = A.colunas * B.colunas;

    matrizResult.matriz = (int**)malloc(matrizResult.linhas * sizeof(int *));
    for(int i = 0; i < matrizResult.linhas; i++){
        matrizResult.matriz[i] = (int *)malloc(matrizResult.colunas * sizeof(int));
    }

    if (num_threads > matrizResult.linhas) {
        num_threads = matrizResult.linhas;
    }

    pthread_t threads[num_threads];
    argsThread1D args[num_threads];

    int linhas_por_thread = matrizResult.linhas / num_threads;
    int linhas_resto = matrizResult.linhas % num_threads;
    int linha_atual = 0;

    for (int t = 0; t < num_threads; t++) {
        args[t].a = A;
        args[t].b = B;
        args[t].resultado = &matrizResult;

        args[t].initLinha = linha_atual;

        int linhas_desta_thread = linhas_por_thread + (t < linhas_resto ? 1 : 0);
        args[t].fimLinha = linha_atual + linhas_desta_thread;
        linha_atual = args[t].fimLinha;

        pthread_create(&threads[t], NULL, worker_Pthreads1D, &args[t]);
    }


    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    return matrizResult;
}

void *worker_Pthreads2D(void *arg){

    argsThread2D* args = (argsThread2D*)arg;

        Node A = args->a;
        Node B = args->b;
        Node *res = args->resultado;

        for (int i = args->initLinha; i < args->fimLinha; i++) {
            for (int j = args->initColuna; j < args->fimColuna; j++) {

                int linhaA = i / B.linhas;
                int colunaA = j / B.colunas;
                int linhaB = i % B.linhas;
                int colunaB = j % B.colunas;

                res->matriz[i][j] = A.matriz[linhaA][colunaA] * B.matriz[linhaB][colunaB];
            }
        }

     return NULL;
}

Node produtoTensorial_Pthreads2D(Node A, Node B, int num_threads) {
    Node matrizResult;
    matrizResult.linhas = A.linhas * B.linhas;
    matrizResult.colunas = A.colunas * B.colunas;

    matrizResult.matriz = (int**)malloc(matrizResult.linhas * sizeof(int *));
    for(int i = 0; i < matrizResult.linhas; i++){
        matrizResult.matriz[i] = (int *)malloc(matrizResult.colunas * sizeof(int));
    }

    int total_celulas = matrizResult.linhas * matrizResult.colunas;
    if (num_threads > total_celulas) {
        num_threads = total_celulas;
    }

    pthread_t threads[num_threads];
    argsThread2D args[num_threads];

    int div_linhas = 1;
    int div_colunas = num_threads;
    for (int i = 2; i * i <= num_threads; i++) {
        if (num_threads % i == 0) {
            div_linhas = i;
            div_colunas = num_threads / i;
        }
    }

    int linhas_por_bloco = matrizResult.linhas / div_linhas;
    int resto_linhas = matrizResult.linhas % div_linhas;

    int colunas_por_bloco = matrizResult.colunas / div_colunas;
    int resto_colunas = matrizResult.colunas % div_colunas;

    int thread_id = 0;
    int linha_atual = 0;

    for (int i = 0; i < div_linhas; i++) {
        int linhas_desta_thread = linhas_por_bloco + (i < resto_linhas ? 1 : 0);
        int coluna_atual = 0;

        for (int j = 0; j < div_colunas; j++) {
            int colunas_desta_thread = colunas_por_bloco + (j < resto_colunas ? 1 : 0);

            if (thread_id < num_threads) {
                args[thread_id].a = A;
                args[thread_id].b = B;
                args[thread_id].resultado = &matrizResult;

                args[thread_id].initLinha = linha_atual;
                args[thread_id].fimLinha = linha_atual + linhas_desta_thread;

                args[thread_id].initColuna = coluna_atual;
                args[thread_id].fimColuna = coluna_atual + colunas_desta_thread;

                pthread_create(&threads[thread_id], NULL, worker_Pthreads2D, &args[thread_id]);
                thread_id++;
            }
            coluna_atual += colunas_desta_thread;
        }
        linha_atual += linhas_desta_thread;
    }

    for (int t = 0; t < thread_id; t++) {
        pthread_join(threads[t], NULL);
    }

    return matrizResult;
}

Node lerArquivo(FILE *arquivo) {
    Node matriz;
    char ch;
    int space = 0;
    matriz.colunas = 0;
    matriz.linhas = 0;

    while((ch = fgetc(arquivo)) != '\n' && ch != EOF){
        if(ch != ' '){
            if(!space){
                matriz.colunas++;
                space = 1;
            }
        } else {
            space = 0;
        }
    }

    if (matriz.colunas > 0){
        matriz.linhas = 1;
    }

    while ((ch = fgetc(arquivo)) != EOF) {
        if (ch == '\n') {
            matriz.linhas++;
        }
    }

    if(matriz.linhas == 0 || matriz.colunas == 0){
        printf("ERRO: MATRIZ VAZIA OU COM FORMATO INVALIDO\n");
        exit(1);
    }

    matriz.matriz = (int **)malloc(matriz.linhas * sizeof(int *));
    if(matriz.matriz == NULL){
        printf("ERRO DE ALOCACAO\n");
        exit(1);
    }

    for (int i = 0; i < matriz.linhas; i++) {
        matriz.matriz[i] = (int *)malloc(matriz.colunas * sizeof(int));
    }

    rewind(arquivo);

    for (int i = 0; i < matriz.linhas; i++) {
        for (int j = 0; j < matriz.colunas; j++) {
            fscanf(arquivo, "%d", &matriz.matriz[i][j]);
        }
    }

    return matriz;
}

Node produtoTensorial(Node A, Node B){
    Node matrizResult;
    matrizResult.linhas = A.linhas * B.linhas;
    matrizResult.colunas = A.colunas * B.colunas;

    matrizResult.matriz = (int**)malloc(matrizResult.linhas * sizeof(int *));
    for(int i = 0; i < matrizResult.linhas; i++){
        matrizResult.matriz[i] = (int *)malloc(matrizResult.colunas * sizeof(int));
    }

    for (int i = 0; i < matrizResult.linhas; i++) {
        for (int j = 0; j < matrizResult.colunas; j++) {
            int linhaA = i / B.linhas;
            int colunaA = j / B.colunas;
            int linhaB = i % B.linhas;
            int colunaB = j % B.colunas;

            matrizResult.matriz[i][j] = A.matriz[linhaA][colunaA] * B.matriz[linhaB][colunaB];
        }
    }
    return matrizResult;
}

void liberarMatriz(Node matriz) {
    for (int i = 0; i < matriz.linhas; i++) {
        free(matriz.matriz[i]);
    }
    free(matriz.matriz);
}
