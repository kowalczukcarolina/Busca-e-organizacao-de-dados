#ifndef LISTA_CAB_H
#define LISTA_CAB_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_ LISTA;

LISTA *lista_criar_cab(void);
void lista_apagar_cab(LISTA **lista);
bool lista_inserir_cab(LISTA *lista, ITEM *item);
ITEM *lista_buscar_cab(LISTA *lista, int chave);
ITEM *lista_remover_cab(LISTA *lista, int chave);
int lista_tamanho_cab(const LISTA *lista);
bool lista_vazia_cab(const LISTA *lista);
void lista_imprimir_cab(const LISTA *lista);

#endif /* LISTA_CAB_H */