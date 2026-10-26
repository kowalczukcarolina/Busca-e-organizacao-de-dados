#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"

#ifndef CAPACIDADE_INICIAL
#define CAPACIDADE_INICIAL 10
#endif

typedef struct lista_seq LISTA_SEQ;

LISTA_SEQ *lista_criar_seq(bool ordenada);
// destruir_itens = false: apenas libera o vetor (os itens pertencem a outra estrutura)
void lista_apagar_seq(LISTA_SEQ **lista, bool destruir_itens);
bool lista_inserir_seq(LISTA_SEQ *lista, ITEM *item);
ITEM *lista_busca_seq(const LISTA_SEQ *lista, const char *chave);
ITEM *lista_remover_seq(LISTA_SEQ *lista, const char *chave);
int lista_tamanho_seq(const LISTA_SEQ *lista);
bool lista_vazia_seq(const LISTA_SEQ *lista);
bool lista_cheia_seq(const LISTA_SEQ *lista);
void lista_imprimir_seq(const LISTA_SEQ *lista);

// Define o critério da lista ORDENADA (comando ORDER da consulta).
// Deve ser chamado com a lista ainda vazia. Sem chamada: ordena por item_get_chave (crescente).
void lista_definir_ordem_seq(LISTA_SEQ *lista, const char *campo, bool desc);

// Percorre do índice 0 até o fim, em O(n) total. Para quando o visitante devolve false.
void lista_percorrer_seq(const LISTA_SEQ *lista, ITEM_VISITOR f, void *ctx);

#endif
