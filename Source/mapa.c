#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../headers/mapa.h"

typedef struct pos {
    int x;
    int y;
} pos;
pos origem = {0, 0};
int ponto_existe(pos* localiza, int total, int x, int y) {
    for (int i = 0; i < total; i++) {
        if (localiza[i].x == x && localiza[i].y == y) {
            return 1;
        }
    }
    return 0;
}
int calcular_distancia(pos p1, pos p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}
int comparar_proximidade(const void *a, const void *b) {
    const pos *p1 = (const pos *)a;
    const pos *p2 = (const pos *)b;
    int dist1 = calcular_distancia(*p1, origem);
    int dist2 = calcular_distancia(*p2, origem);
    return dist1 - dist2;
}

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

 void gerar_caminho(int** mapa, int altura, int largura, int x1, int y1, int x2, int y2, unsigned int seed) {
    (void)altura;
    (void)largura;
    srand(seed);
    int x = x1, y = y1;
    while (x != x2 || y != y2) {
        if (x != x2 && (y == y2 || rand() % 2)) {
            int passo = (x < x2) ? 1 : -1;
            x += passo;
        } else if (y != y2) {
            int passo = (y < y2) ? 1 : -1;
            y += passo;
        }
        if (mapa[x][y] == 1){
            mapa[x][y] = 0;
        }
    }
}

int** gerar_mapa(int altura, int largura, int chaves, unsigned int seed) {
    srand(seed);
    int **mapa = malloc(altura * sizeof(int*));
    if (!mapa) return NULL;
    for (int i = 0; i < altura; i++) {
        mapa[i] = malloc(largura * sizeof(int));
        if (!mapa[i]) return NULL;

    }
    // Inicializa tudo como parede
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            mapa[i][j] = 1;
        }
    }
    int total_pontos = chaves + 2;
    pos* localiza = malloc(total_pontos * sizeof(pos));
    if (!localiza) return mapa;
    int verificacao = 0;
    while (verificacao < total_pontos) {
        int x = rand() % altura;
        int y = rand() % largura;
        if (!ponto_existe(localiza, verificacao, x, y)) {
            localiza[verificacao] = (pos){x, y};
            verificacao++;
        }
    }
    qsort(localiza, total_pontos, sizeof(pos), comparar_proximidade);
    mapa[localiza[0].x][localiza[0].y] = 2; // Início
    for (int i = 1; i <= chaves; i++){
        mapa[localiza[i].x][localiza[i].y] = 3; // Chaves
    }
    mapa[localiza[total_pontos - 1].x][localiza[total_pontos - 1].y] = 4; // Fim
    for (int i = 1; i < total_pontos; i++){
        gerar_caminho(mapa, altura, largura,
                      localiza[i - 1].x, localiza[i - 1].y,
                      localiza[i].x, localiza[i].y,
                      seed + (unsigned int)i);
    }
    int caminhos_extras = (altura * largura) / 5;
    for (int i = 0; i < caminhos_extras; i++){
        int cx = rand() % altura;
        int cy = rand() % largura;
        if (mapa[cx][cy] == 1) {
            mapa[cx][cy] = 0;
        }
    }
    for (int i = 0; i < altura; i++){
        for (int j = 0; j < largura; j++) {
            printf("%d ", mapa[i][j]);
        }
        printf("\n");
    }
    free(localiza);
    return mapa;
} 