#include <stdio.h>
#include <stdlib.h>
#include "tensor.h"

int main(int argc, char **argv) {
    if(argc < 3){
        printf("ERRO: NUMERO INSUFICIENTE DE ARQUIVOS\n");
        return 1;
    }

    Node matrizes[argc - 1];
    FILE *arquivo;
    int matrizes_lidas = 0;

    for(int i = 1; i < argc; i++){
        arquivo = fopen(argv[i], "r");
        if(arquivo == NULL){
            printf("ERRO AO ABRIR ARQUIVO\n");
            for(int j = 0; j < matrizes_lidas; j++){
                liberarMatriz(matrizes[j]);
            }
            return 1;
        }

        Node matrizArq = lerArquivo(arquivo);
        matrizes[i - 1] = matrizArq;
        matrizes_lidas++;
        fclose(arquivo);
    }

    Node acumulador = matrizes[0];

    for (int i = 1; i < argc - 1; i++) {
        Node matriz = matrizes[i];
        Node mult_resultado = produtoTensorial_Pthreads2D(acumulador, matriz, 4);

        if (i > 1) {
             liberarMatriz(acumulador);
        }
        acumulador = mult_resultado;
    }

    FILE *arq_saida = fopen("tensor_maolf.out", "w");
    if(arq_saida == NULL){
        printf("ERRO AO ABRIR ARQUIVO DE SAIDA\n");
        for(int i = 0; i < argc - 1; i++){
            liberarMatriz(matrizes[i]);
        }
        if (argc > 2) {
            liberarMatriz(acumulador);
        }
        return 1;
    }

    for(int j = 0; j < acumulador.linhas; j++){
        for(int k = 0; k < acumulador.colunas; k++){
            fprintf(arq_saida, "%d ", (acumulador).matriz[j][k]);
        }
        fprintf(arq_saida, "\n");
    }

    fclose(arq_saida);

    for(int i = 0; i < argc - 1; i++){
        liberarMatriz(matrizes[i]);
    }
    if (argc > 2) {
        liberarMatriz(acumulador);
    }

    return 0;
}
