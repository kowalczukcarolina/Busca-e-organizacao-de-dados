#ifndef LISTA_GEN_H
#define LISTA_GEN_H

#include "item.h"
#include <stdbool.h>

// Lista generalizada: cada elemento é um átomo (ITEM) ou uma sublista rotulada (grupo).
typedef struct lista_gen LISTA_GEN;

LISTA_GEN *lgen_criar(void);
// destruir_itens = false: apenas libera os nós (os itens pertencem a outra estrutura)
void lgen_destruir(LISTA_GEN **l, bool destruir_itens);

// Inserção de átomo (nível principal) e de sublista já montada, com rótulo
bool lgen_inserir_atomo(LISTA_GEN *l, ITEM *item);
bool lgen_inserir_sublista(LISTA_GEN *l, LISTA_GEN *sublista, const char *chave);

// Agrupamento: desce (criando se preciso) uma sublista por chave e insere o item no último nível.
// Ex.: chaves = {"Safe", "2025"} -> grupo "Safe" > subgrupo "2025" > item. n = 0 insere no topo.
bool lgen_inserir_agrupado(LISTA_GEN *l, ITEM *item, const char *chaves[], int n);

// Operações de consulta e exibição
void lgen_imprimir(const LISTA_GEN *l);
int lgen_obter_profundidade(const LISTA_GEN *l);
int lgen_tamanho(const LISTA_GEN *l); // total de átomos, O(1)

// Percorre todos os átomos em profundidade (grupo por grupo), em O(n) total.
void lgen_percorrer(const LISTA_GEN *l, ITEM_VISITOR f, void *ctx);

#endif
