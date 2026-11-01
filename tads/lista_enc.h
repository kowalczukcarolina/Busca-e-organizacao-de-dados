#ifndef LISTA_ENC_H
#define LISTA_ENC_H

#include <stdbool.h>
#include "item.h"


typedef struct lista_enc LISTA_ENC;

LISTA_ENC *lista_criar_enc(bool ordenada);
// destruir_itens = false: apenas libera os nós (os itens pertencem a outra estrutura)
void lista_apagar_enc(LISTA_ENC **lista, bool destruir_itens);
bool lista_inserir_enc(LISTA_ENC *lista, ITEM *item);
ITEM *lista_busca_enc(const LISTA_ENC *lista, const char *chave);
ITEM *lista_busca_recursiva_enc(const LISTA_ENC *lista, const char *chave);
ITEM *lista_remover_enc(LISTA_ENC *lista, const char *chave);
int lista_tamanho_enc(const LISTA_ENC *lista);
bool lista_vazia_enc(const LISTA_ENC *lista);
bool lista_cheia_enc(const LISTA_ENC *lista);
void lista_imprimir_enc(const LISTA_ENC *lista);

// Critério da lista ORDENADA (comando ORDER). Chamar com a lista vazia.
void lista_definir_ordem_enc(LISTA_ENC *lista, const char *campo, bool desc);

// Percorre do início ao fim, em O(n) total. Para quando o visitante devolve false.
void lista_percorrer_enc(const LISTA_ENC *lista, ITEM_VISITOR f, void *ctx);

#endif
