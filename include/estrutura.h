#ifndef ESTRUTURA_H
#define ESTRUTURA_H

#include <stdbool.h>
#include "tads/item.h"
#include "query.h"

/*
 * Fachada única sobre as 8 estruturas (pilha, fila, lista_seq, lista_ord, lista_enc,
 * lista_cabeca, lista_gen, lista_cruz). main e reader só falam com este módulo, então
 * trocar o dataset (Parte 2) não exige tocar nas estruturas.
 */
typedef struct estrutura ESTRUTURA;

ESTRUTURA *est_criar(const char *nome);                                   /* NULL se o nome não existe */
void est_configurar_ordem(ESTRUTURA *e, const char *campo, bool desc);   /* só afeta lista_ord */
bool est_inserir(ESTRUTURA *e, ITEM *item, const QUERY *q);              /* q pode ser NULL; usa GROUP/CROSS */
int est_tamanho(ESTRUTURA *e);
void est_percorrer(ESTRUTURA *e, ITEM_VISITOR f, void *ctx);
// destruir_itens = true somente na estrutura dona dos itens (--base)
void est_apagar(ESTRUTURA **e, bool destruir_itens);

#endif
