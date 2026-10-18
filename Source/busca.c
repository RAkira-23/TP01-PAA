#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "../headers/busca.h"

typedef struct { int linha, coluna; } Posicao;
typedef struct {
    int **mapa;
    int linhas, colunas, chaves;
    bool *visitado;
    Posicao *caminho;
    size_t tamanho, profundidade;
    unsigned long long chamadas;
    unsigned long long total;
    bool todos, erro;
    FILE *arquivo;
} Busca;

static void imprimir_caminho(FILE *saida, const Busca *b){
    for (size_t i = 0; i < b->tamanho; i++)
        fprintf(saida, "%s[%d,%d]", i ? "," : "", b->caminho[i].linha, b->caminho[i].coluna);
    fputc('\n', saida);
}

static bool buscar(Busca *b, int l, int c, int chaves) {
    b->chamadas++;
    size_t indice = (size_t)l * b->colunas + c;
    b->visitado[indice] = true;
    b->caminho[b->tamanho++] = (Posicao){l, c};
    if (b->tamanho > b->profundidade) b->profundidade = b->tamanho;
    if (b->mapa[l][c] == 3) chaves++;
    if (b->mapa[l][c] == 4 && chaves == b->chaves) {
        if (!b->todos) return true;
        b->total++;
        printf("Caminho %llu:\n", b->total);
        imprimir_caminho(stdout, b);
        fprintf(b->arquivo, "Caminho %llu:\n", b->total);
        imprimir_caminho(b->arquivo, b);
        if (ferror(b->arquivo)) {
            b->erro = true;
            return true; // Interrompe a busca se nao for possivel salvar.
        }
        // Uma solucao termina no bau volta para explorar outras escolhas.
        b->visitado[indice] = false;
        b->tamanho--;
        return false;
    }
    static const int dl[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
    for (int i = 0; i < 4; i++) {
        int nl = l + dl[i], nc = c + dc[i];
        if (nl >= 0 && nl < b->linhas && nc >= 0 && nc < b->colunas &&
            b->mapa[nl][nc] != 1 && !b->visitado[(size_t)nl * b->colunas + nc] &&
            buscar(b, nl, nc, chaves)) return true;
    }
    // Desfaz as escolhas do caminho atual sem alterar o mapa.
    b->visitado[indice] = false;
    b->tamanho--;
    return false;
}

static bool executar_busca(int **mapa, int linhas, int colunas, int chaves, bool analise, bool todos, const char *nome_arquivo, int ***matriz_saida, int *tam_saida) {
    if (!mapa || linhas <= 0 || colunas <= 0 || chaves < 0) return false;
    if ((size_t)linhas > SIZE_MAX / (size_t)colunas) return false;
    size_t n = (size_t)linhas * colunas;
    if (n > SIZE_MAX / sizeof(Posicao)) return false;
    Busca b = {0};
    b.mapa = mapa; b.linhas = linhas; b.colunas = colunas; b.chaves = chaves;
    b.todos = todos;
    b.visitado = calloc(n, sizeof(*b.visitado));
    b.caminho = malloc(n * sizeof(*b.caminho));
    if (!b.visitado || !b.caminho) {
        puts("Memoria insuficiente para a busca.");
        free(b.visitado); free(b.caminho); return false;
    }
    if (todos) {
        b.arquivo = fopen(nome_arquivo, "w");
        if (!b.arquivo) {
            puts("Nao foi possivel abrir o arquivo de caminhos.");
            free(b.visitado); free(b.caminho); return false;
        }
        fprintf(b.arquivo, "Todos os caminhos validos; coordenadas iniciam em zero.\n");
    }
    int l = -1, c = -1;
    for (int i = 0; i < linhas; i++)
        for (int j = 0; j < colunas; j++)
            if (mapa[i][j] == 2) { l = i; c = j; }
    clock_t inicio = clock();
    bool ok = l >= 0 && buscar(&b, l, c, 0);
    clock_t fim = clock();
    if (todos) {
        ok = b.total > 0;
        if (!b.erro) {
            fprintf(b.arquivo, "Total de caminhos: %llu\n", b.total);
            if (!ok) fprintf(b.arquivo, "Nao ha um caminho possivel para a entrada de dados fornecida.\n");
            if (ferror(b.arquivo)) b.erro = true;
        }
        if (fclose(b.arquivo) != 0) b.erro = true;
        if (b.erro) {
            puts("Erro ao salvar os caminhos. O arquivo pode estar incompleto.");
            ok = false;
        } else {
            printf("Total de caminhos: %llu\nSalvos em: %s\n", b.total, nome_arquivo);
            if (!ok) puts("Nao ha um caminho possivel para a entrada de dados fornecida.");
        }
    } else if (ok) {
        puts("Caminho encontrado (coordenadas iniciam em zero):");
        imprimir_caminho(stdout, &b);
    } else puts("Nao ha um caminho possivel para a entrada de dados fornecida.");
    if (analise) {
        printf("Chamadas recursivas: %llu\nProfundidade maxima: %zu\n", b.chamadas, b.profundidade);
        if (inicio != (clock_t)-1 && fim != (clock_t)-1)
            printf("Tempo (ms): %.3f\n", 1000.0 * (double)(fim - inicio) / CLOCKS_PER_SEC);
    }

    if (!todos && ok && matriz_saida && tam_saida) {
        *matriz_saida = malloc(b.tamanho * sizeof(int*));
        if (*matriz_saida) {
            for (size_t i = 0; i < b.tamanho; i++) {
                (*matriz_saida)[i] = malloc(2 * sizeof(int));
                if ((*matriz_saida)[i]) {
                    (*matriz_saida)[i][0] = b.caminho[i].linha;
                    (*matriz_saida)[i][1] = b.caminho[i].coluna;
                }
            }
            *tam_saida = (int)b.tamanho;
        }
    }

    free(b.visitado); free(b.caminho);
    return ok;
}

int** encontrar_caminho(int **mapa, int linhas, int colunas, int chaves, bool analise, int *tam_caminho) {
    int **matriz = NULL;
    if (tam_caminho) *tam_caminho = 0;
    executar_busca(mapa, linhas, colunas, chaves, analise, false, NULL, &matriz, tam_caminho);
    return matriz;
}

bool salvar_todos_caminhos(int **mapa, int linhas, int colunas, int chaves, bool analise, const char *nome_arquivo) {
    if (!nome_arquivo) return false;
    return executar_busca(mapa, linhas, colunas, chaves, analise, true, nome_arquivo, NULL, NULL);
}