#ifndef LISTA_ENC_H
#define LISTA_ENC_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_enc LISTA_ENC;

LISTA_ENC *lista_criar_enc(bool ordenada);
void lista_apagar_enc(LISTA_ENC **lista);
bool lista_inserir_enc(LISTA_ENC *lista, ITEM *item);
ITEM *lista_busca_enc(const LISTA_ENC *lista, const char *chave);
ITEM *lista_busca_recursiva_enc(const LISTA_ENC *lista, const char *chave);
ITEM *lista_remover_enc(LISTA_ENC *lista, const char *chave);
int lista_tamanho_enc(const LISTA_ENC *lista);
bool lista_vazia_enc(const LISTA_ENC *lista);
bool lista_cheia_enc(const LISTA_ENC *lista);
void lista_imprimir_enc(const LISTA_ENC *lista);

// Função adicionada para permitir percorrer os elementos por índice (0 até tamanho - 1)
ITEM *lista_obter_item_enc(const LISTA_ENC *lista, int index);

#endif