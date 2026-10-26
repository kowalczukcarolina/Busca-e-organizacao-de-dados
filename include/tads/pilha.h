#ifndef PILHA_H
#define PILHA_H

#include "item.h"
#include <stdbool.h>

typedef struct pilha PILHA;

PILHA *pilha_criar(void);
// destruir_itens = false: apenas libera os nós (os itens pertencem a outra estrutura)
void pilha_apagar(PILHA **pilha, bool destruir_itens);
bool pilha_vazia(PILHA *pilha);
bool pilha_cheia(PILHA *pilha);
int pilha_tamanho(PILHA *pilha);
ITEM *pilha_topo(PILHA *pilha);
bool pilha_empilhar(PILHA *pilha, ITEM *item);
ITEM *pilha_desempilhar(PILHA *pilha);
void pilha_imprimir(PILHA *pilha);
void pilha_inverter(PILHA *pilha);

// Percorre do topo até a base, em O(n) total. Para quando o visitante devolve false.
void pilha_percorrer(PILHA *pilha, ITEM_VISITOR f, void *ctx);

#endif
