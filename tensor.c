#include <stdio.h>
#include <stdlib.h>
#include "tensor.h"

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
