#ifndef BUSCA_H
#define BUSCA_H
#include <stdbool.h>
bool encontrar_caminho(int **mapa, int linhas, int colunas, int chaves);
bool salvar_todos_caminhos(int **mapa, int linhas, int colunas, int chaves,
const char *nome_arquivo);
#endif
