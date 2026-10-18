#include <stdio.h>
#include <stdlib.h>
#include "../headers/globo.h"

// Encontra a posição inicial (valor 2 = 'A') na matriz dinâmica
Posicao encontrar_inicio(int **mapa, int linhas, int colunas) {
    Posicao pos = {-1, -1};
    if (!mapa || linhas <= 0 || colunas <= 0) return pos;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (mapa[i][j] == 2) { // Valor 2 representa a posição inicial ('A')
                pos.linha = i;
                pos.coluna = j;
                return pos;
            }
        }
    }
    return pos;
}

// Realiza o movimento com lógica de borda infinita (matriz toroidal)
Posicao mover_globo(int **mapa, int linhas, int colunas, Posicao pos_atual, char direcao) {
    if (!mapa || linhas <= 0 || colunas <= 0) return pos_atual;

    int d_linha = 0;
    int d_coluna = 0;

    switch (direcao) {
        case 'C': case 'c': d_linha = -1; break; // Cima
        case 'B': case 'b': d_linha =  1; break; // Baixo
        case 'E': case 'e': d_coluna = -1; break; // Esquerda
        case 'D': case 'd': d_coluna =  1; break; // Direita
        default:
            printf("Direção inválida!\n");
            return pos_atual;
    }

    // Aplicação do módulo para atravessar a borda
    int nova_linha = (pos_atual.linha + d_linha + linhas) % linhas;
    int nova_coluna = (pos_atual.coluna + d_coluna + colunas) % colunas;

    // Verifica se bateu em uma parede (valor 1)
    if (mapa[nova_linha][nova_coluna] == 1) {
        printf("Bateu em uma parede na posição [%d, %d]! Movimento cancelado.\n", nova_linha, nova_coluna);
        return pos_atual;
    }

    printf("Moveu para a posição [%d, %d]\n", nova_linha, nova_coluna);
    Posicao nova_pos = {nova_linha, nova_coluna};
    return nova_pos;
}

// Função para ser invocada pelo menu principal (opção 5)
void mover_no_globo(int **mapa, int linhas, int colunas) {
    if (!mapa) {
        puts("Carregue ou crie o mapa primeiro.");
        return;
    }

    Posicao atual = encontrar_inicio(mapa, linhas, colunas);
    if (atual.linha == -1) {
        puts("Posição inicial ('A' / valor 2) não encontrada no mapa.");
        return;
    }

    printf("Posição Inicial do Aventureiro: [%d, %d]\n", atual.linha, atual.coluna);

    char direcao;
    while (1) {
        printf("\nDigite a direção (C: Cima, B: Baixo, E: Esquerda, D: Direita, S: Sair): ");
        if (scanf(" %c", &direcao) != 1 || direcao == 'S' || direcao == 's') {
            break;
        }
        atual = mover_globo(mapa, linhas, colunas, atual, direcao);
    }
}