#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int** ler_arquivo(int *linhas, int *colunas, int *qtd_chaves) {
    FILE *arq = fopen("../arquivo.txt", "r");
    if (arq == NULL){
        printf("Nao foi possivel ler o arquivo\n");
        return NULL;
    }
    if (fscanf(arq, "%d %d", linhas, colunas) != 2){
        printf("Erro ao ler as dimensões.\n");
        fclose(arq);
        return NULL;
    }
    if (fscanf(arq, "%d", qtd_chaves) != 1){
        printf("Erro ao ler a quantidade de chaves.\n");
        fclose(arq);
        return NULL;
    }
    char buffer_limpeza[256];
    fgets(buffer_limpeza, sizeof(buffer_limpeza), arq);
    int **matriz = (int**) malloc((*linhas) * sizeof(int*));
    for (int i = 0; i < *linhas; i++) {
        matriz[i] = (int*) malloc((*colunas) * sizeof(int));
    }
    char linha[256];
    int i = 0;
    while (i < *linhas && fgets(linha, sizeof(linha), arq) != NULL){
        linha[strcspn(linha, "\r\n")] = 0;
        if (strlen(linha) == 0) continue;
        char *token = strtok(linha, " ");
        int j = 0;
        while (token != NULL && j < *colunas){
            if (strcmp(token, "0") == 0){
                matriz[i][j] = 0;
            }else if (strcmp(token, "1") == 0){
                matriz[i][j] = 1;
            }else if (strcmp(token, "A") == 0){
                matriz[i][j] = 2;
            }else if (strcmp(token, "C") == 0){
                matriz[i][j] = 3;
            }else if (strcmp(token, "X") == 0){
                matriz[i][j] = 4;
            }else {
                matriz[i][j] = 0;
            }
            token = strtok(NULL, " ");
            j++;
        }
        i++;
    }
    fclose(arq);
    return matriz;
}
void limpar_mapa(int** mapa, int linha, int coluna){
    for (int i = 0; i<linha; i++){
        free(mapa[i]);
    }
    free(mapa);
}