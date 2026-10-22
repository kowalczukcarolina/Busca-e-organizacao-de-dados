#ifndef LISTA_GEN_H
#define LISTA_GEN_H

#include "item.h"
// Implementação do professor de lista generalizada

// Declarações opacas corrigidas para LISTA_GEN
typedef struct lista_gen LISTA_GEN;

// Funções principais de gestão e operações pedidas
LISTA_GEN *lgen_criar(void);
void lgen_destruir(LISTA_GEN **l);
void lgen_imprimir(const LISTA_GEN *l);
int lgen_obter_profundidade(const LISTA_GEN *l);

#endif