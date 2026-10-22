#ifndef LISTA_CAB_H
#define LISTA_CAB_H

#include <stdbool.h>
#include <stdio.h>
#include "item.h"

typedef struct lista_cab LISTA_CAB;

LISTA_CAB *lista_criar_cab(void);
void lista_apagar_cab(LISTA_CAB **lista);
bool lista_inserir_cab(LISTA_CAB *lista, ITEM *item);
ITEM *lista_buscar_cab(const LISTA_CAB *lista, const char *chave);
ITEM *lista_remover_cab(LISTA_CAB *lista, const char *chave);
int lista_tamanho_cab(const LISTA_CAB *lista);
bool lista_vazia_cab(const LISTA_CAB *lista);
void lista_imprimir_cab(const LISTA_CAB *lista);

#endif