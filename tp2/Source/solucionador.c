#define __USE_MINGW_ANSI_STDIO 1
#include "../headers/solucionador.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Zero indica estado inalcancavel: toda vida valida e estritamente positiva. */
static bool atualizar(const Mapa *mapa, size_t origem, size_t destino,
                      long long *vida, size_t *pai) {
    long long valor = mapa->valores[destino];
    if ((valor > 0 && vida[origem] > LLONG_MAX - valor) ||
        (valor < 0 && vida[origem] < LLONG_MIN - valor)) return false;
    long long nova = vida[origem] + valor;
    if (nova > vida[destino]) {
        vida[destino] = nova;
        pai[destino] = origem;
    }
    return true;
}

bool resolver_e_salvar(const Mapa *mapa, const char *arquivo) {
    size_t total = mapa->linhas * mapa->colunas;
    if (total > SIZE_MAX / sizeof(size_t)) return false;
    long long *vida = calloc(total, sizeof(*vida));
    size_t *pai = malloc(total * sizeof(*pai));
    size_t *caminho = malloc(total * sizeof(*caminho));
    bool ok = false;
    FILE *saida = NULL;
    if (!vida || !pai || !caminho) goto finalizar;
    for (size_t p = 0; p < total; p++) pai[p] = SIZE_MAX;
    if (mapa->vida_inicial > 0) vida[mapa->inicio] = mapa->vida_inicial;

    /* Ordem topologica: baixo para cima, direita para esquerda.
       Cada estado guarda a maior vida entre os caminhos que sobreviveram.
       Mais vida na mesma celula sempre domina menos vida. */
    for (size_t i = mapa->linhas; i-- > 0;) {
        for (size_t j = mapa->colunas; j-- > 0;) {
            size_t p = i * mapa->colunas + j;
            if (vida[p] <= 0 || p == mapa->fim) continue;
            if (i > 0 && !atualizar(mapa, p, p - mapa->colunas, vida, pai))
                goto finalizar;
            if (j > 0 && !atualizar(mapa, p, p - 1, vida, pai))
                goto finalizar;
        }
    }

    saida = fopen(arquivo, "w");
    if (!saida) goto finalizar;
    if (vida[mapa->fim] <= 0) {
        /* UTF-8 para manter a palavra exigida pelo enunciado. */
        if (fputs("imposs\xC3\xADvel\n", saida) == EOF) goto finalizar;
    } else {
        size_t tamanho = 0;
        for (size_t p = mapa->fim; p != SIZE_MAX; p = pai[p])
            caminho[tamanho++] = p;
        while (tamanho > 0) {
            size_t p = caminho[--tamanho];
            if (fprintf(saida, "%zu %zu\n", p / mapa->colunas,
                        p % mapa->colunas) < 0) goto finalizar;
        }
    }
    ok = true;
finalizar:
    if (saida && fclose(saida) != 0) ok = false;
    free(vida);
    free(pai);
    free(caminho);
    return ok;
}
