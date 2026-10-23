#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"

#ifndef CAPACIDADE_INICIAL
#define CAPACIDADE_INICIAL 10
#endif

typedef struct lista_seq LISTA_SEQ;

LISTA_SEQ *lista_criar_seq(bool ordenada);
void lista_apagar_seq(LISTA_SEQ **lista);
bool lista_inserir_seq(LISTA_SEQ *lista, ITEM *item);
ITEM *lista_busca_seq(const LISTA_SEQ *lista, const char *chave);
ITEM *lista_remover_seq(LISTA_SEQ *lista, const char *chave);
int lista_tamanho_seq(const LISTA_SEQ *lista);
bool lista_vazia_seq(const LISTA_SEQ *lista);
bool lista_cheia_seq(const LISTA_SEQ *lista);
void lista_imprimir_seq(const LISTA_SEQ *lista);

// Função adicionada para permitir percorrer os elementos por índice (0 até tamanho - 1)
ITEM *lista_obter_item_seq(const LISTA_SEQ *lista, int index);

#endif