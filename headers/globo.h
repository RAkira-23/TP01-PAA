#ifndef GLOBO_H
#define GLOBO_H

typedef struct
{
    int linha;
    int coluna;
} Posicao;

Posicao encontrar_inicio(int **mapa, int linhas, int colunas);
Posicao mover_globo(int **mapa, int linhas, int colunas, Posicao pos_atual, char direcao);
void mover_no_globo(int **mapa, int linhas, int colunas);

#endif