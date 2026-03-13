#include <stdio.h>
#include <time.h>
#include "tensor.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("ERRO: NUMERO INSUFICIENTE DE ARQUIVOS.\nUso: ./tensor [arq1] [arq2] ...\n");
        return 1;
    }

    int num_matrizes = argc - 1;
    Node matrizes[num_matrizes];
    FILE *arquivo;

    for (int i = 0; i < num_matrizes; i++) {
        arquivo = fopen(argv[i + 1], "r");
        if (arquivo == NULL) {
            printf("ERRO: NAO FOI POSSIVEL ABRIR '%s'\n", argv[i + 1]);
            for (int j = 0; j < i; j++) liberarMatriz(matrizes[j]);
            return 1;
        }
        matrizes[i] = lerArquivo(arquivo);
        fclose(arquivo);
    }

    struct timespec inicio, fim;
    double tempo_serial, tempo_omp, tempo_pth1d, tempo_pth2d;
    int num_threads = 4;


    clock_gettime(CLOCK_MONOTONIC, &inicio);
    Node acc_serial = matrizes[0];
    for (int i = 1; i < num_matrizes; i++) {
        Node res = produtoTensorial(acc_serial, matrizes[i]);
        if (i > 1) liberarMatriz(acc_serial);
        acc_serial = res;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_serial = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    Node acc_omp = matrizes[0];
    for (int i = 1; i < num_matrizes; i++) {
        Node res = produtoTensorial_OpenMP(acc_omp, matrizes[i]);
        if (i > 1) liberarMatriz(acc_omp);
        acc_omp = res;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_omp = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    Node acc_pth1d = matrizes[0];
    for (int i = 1; i < num_matrizes; i++) {
        Node res = produtoTensorial_Pthreads1D(acc_pth1d, matrizes[i], num_threads);
        if (i > 1) liberarMatriz(acc_pth1d);
        acc_pth1d = res;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_pth1d = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    Node acc_pth2d = matrizes[0];
    for (int i = 1; i < num_matrizes; i++) {
        Node res = produtoTensorial_Pthreads2D(acc_pth2d, matrizes[i], num_threads);
        if (i > 1) liberarMatriz(acc_pth2d);
        acc_pth2d = res;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_pth2d = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Serial:      %f segundos\n", tempo_serial);
    printf("OpenMP:      %f segundos\n", tempo_omp);
    printf("Pthreads 1D: %f segundos\n", tempo_pth1d);
    printf("Pthreads 2D: %f segundos\n", tempo_pth2d);

    FILE *arq_saida = fopen("tensor_maolf.out", "w");
    if (arq_saida == NULL) {
        printf("ERRO AO CRIAR O ARQUIVO DE SAIDA\n");
    } else {
        for (int i = 0; i < acc_pth2d.linhas; i++) {
            for (int j = 0; j < acc_pth2d.colunas; j++) {
                fprintf(arq_saida, "%d ", acc_pth2d.matriz[i][j]);
            }
            fprintf(arq_saida, "\n");
        }
        fclose(arq_saida);
        printf("Arquivo 'tensor_maolf.out' gerado com sucesso.\n");
    }

    for (int i = 0; i < num_matrizes; i++) {
        liberarMatriz(matrizes[i]);
    }
    if (num_matrizes > 1) {
        liberarMatriz(acc_serial);
        liberarMatriz(acc_omp);
        liberarMatriz(acc_pth1d);
        liberarMatriz(acc_pth2d);
    }

    return 0;
}
