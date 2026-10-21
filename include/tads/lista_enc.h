#ifndef LISTA_ENC_H
#define LISTA_ENC_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_ LISTA;

LISTA *lista_criar_enc(bool ordenada);
void lista_apagar_enc(LISTA **lista);
bool lista_inserir_enc(LISTA *lista, ITEM *item);
ITEM *lista_busca_enc(const LISTA *lista, int chave);
ITEM *lista_busca_recursiva(const LISTA *lista, int chave);
ITEM *lista_remover_enc(LISTA *lista, int chave);
int lista_tamanho_enc(const LISTA *lista);
bool lista_vazia_enc(const LISTA *lista);
bool lista_cheia_enc(const LISTA *lista);
void lista_imprimir_enc(const LISTA *lista);

#endif