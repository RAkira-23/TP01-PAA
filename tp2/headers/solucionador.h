#ifndef SOLUCIONADOR_H
#define SOLUCIONADOR_H

#include "mapa.h"

/* Retorna false apenas em caso de erro de memoria, aritmetica ou arquivo. */
bool resolver_e_salvar(const Mapa *mapa, const char *arquivo);

#endif
