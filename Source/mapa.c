#include <stdio.h>
#include <stdlib.h>
#include "../headers/mapa.h"

void limpar_mapa(int **mapa, int linhas, int colunas) {
    (void)colunas;
    if (!mapa) return;
    for (int i = 0; i < linhas; i++) free(mapa[i]);
    free(mapa);
}

int **ler_arquivo(int *linhas, int *colunas, int *qtd_chaves) {
    FILE *arq = fopen("arquivo.txt", "r");
    if (!arq) arq = fopen("../arquivo.txt", "r");
    if (!arq) { puts("Nao foi possivel ler arquivo.txt."); return NULL; }
    int l, c, q;
    if (fscanf(arq, "%d %d %d", &l, &c, &q) != 3 || l <= 0 || c <= 0 || q < 0) {
        puts("Cabecalho invalido."); fclose(arq); return NULL;
    }
    int **mapa = calloc((size_t)l, sizeof(*mapa));
    if (!mapa) { fclose(arq); return NULL; }
    int inicios = 0, baus = 0, chaves = 0;
    for (int i = 0; i < l; i++) {
        mapa[i] = calloc((size_t)c, sizeof(**mapa));
        if (!mapa[i]) goto erro;
        for (int j = 0; j < c; j++) {
            char token[16];
            if (fscanf(arq, "%15s", token) != 1 || token[1] != '\0') goto erro;
            switch (token[0]) {
            case '0': mapa[i][j] = 0; break;
            case '1': mapa[i][j] = 1; break;
            case 'A': mapa[i][j] = 2; inicios++; break;
            case 'C': mapa[i][j] = 3; chaves++; break;
            case 'X': mapa[i][j] = 4; baus++; break;
            default: goto erro;
            }
        }
    }
    char extra;
    if (fscanf(arq, " %c", &extra) == 1 || inicios != 1 || baus != 1 || chaves != q) goto erro;
    fclose(arq);
    *linhas = l; *colunas = c; *qtd_chaves = q;
    return mapa;
erro:
    puts("Mapa invalido ou memoria insuficiente.");
    limpar_mapa(mapa, l, c); fclose(arq); return NULL;
}
