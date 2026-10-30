#ifndef FILA_H
#define FILA_H
#include <stdbool.h>
#include "item.h"

typedef struct fila_ FILA;

//Construtores e destrutores
FILA *fila_criar(void);
// destruir_itens = false: apenas libera os nós (os itens pertencem a outra estrutura)
void fila_apagar(FILA **fila, bool destruir_itens);

//Operações
bool fila_inserir(FILA *fila, ITEM *item);
ITEM *fila_remover(FILA *fila);

//Consultas sobre a fila
bool fila_vazia(FILA *fila);
bool fila_cheia(FILA *fila);
ITEM *fila_frente(FILA *fila);
int fila_tamanho(FILA *fila);

//Percorre da frente até o fim, em O(n) total. Para quando o visitante devolve false
void fila_percorrer(FILA *fila, ITEM_VISITOR f, void *ctx);

void fila_imprimir(FILA *fila);
#endif
