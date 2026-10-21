#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"

#define CAPACIDADE_INICIAL 100

typedef struct lista_ LISTA;

/* Cria uma lista sequencial dinâmica.
   Se 'ordenada' for true, comporta-se como lista_ord (inserção mantendo a ordem).
   Se 'ordenada' for false, comporta-se como lista_seq comum (inserção no final). */
LISTA *lista_criar(bool ordenada);

void lista_apagar(LISTA **lista);
bool lista_inserir(LISTA *lista, ITEM *item);
ITEM *lista_busca(const LISTA *lista, int chave);
ITEM *lista_remover(LISTA *lista, int chave);

int lista_tamanho(const LISTA *lista);
bool lista_vazia(const LISTA *lista);
bool lista_cheia(const LISTA *lista); // Retorna true apenas se faltar memória no sistema
void lista_imprimir(const LISTA *lista);

#endif