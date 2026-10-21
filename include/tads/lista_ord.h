#ifndef LISTA_GEN_H
#define LISTA_GEN_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_ LISTA;

LISTA *lista_ord_criar(void);
void lista_ord_apagar(LISTA **lista);
bool lista_ord_inserir(LISTA *lista, ITEM *item);
ITEM *lista_ord_buscar(LISTA *lista, int chave);
ITEM *lista_ord_remover(LISTA *lista, int chave);
bool lista_ord_vazia(const LISTA *lista);
int lista_ord_tamanho(const LISTA *lista);
void lista_ord_imprimir(const LISTA *lista);

#endif
