#ifndef LISTA_GEN_H
#define LISTA_GEN_H

#include "item.h"
//Implementação do professor de lista generalizada

//Declarações opacas
typedef struct no_ NO;
typedef struct lista_ LISTA;



//Funções principais de gestão e operações pedidas
LISTA *lgen_criar(void);
void lgen_destruir(LISTA **l);
void lgen_imprimir(const LISTA *l);
int lgen_obter_profundidade(const LISTA *l);

#endif