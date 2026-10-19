#ifndef MAPA_H
#define MAPA_H

int **gerar_mapa(int altura, int largura, int chaves, unsigned int seed);
void imprime_com_tempo(int **mapa, int altura, int largura, int **caminho, int linha_caminho);

int **ler_arquivo(int *linhas, int *colunas, int *qtd_chaves);
void limpar_mapa(int **mapa, int linhas, int colunas);
#endif