#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"

#ifndef CAPACIDADE_INICIAL
#define CAPACIDADE_INICIAL 10
#endif

typedef struct lista_ LISTA;

LISTA *lista_criar_seq(bool ordenada);
void lista_apagar_seq(LISTA **lista);
bool lista_inserir_seq(LISTA *lista, ITEM *item);
ITEM *lista_busca_seq(const LISTA *lista, int chave);
ITEM *lista_remover_seq(LISTA *lista, int chave);
int lista_tamanho_seq(const LISTA *lista);
bool lista_vazia_seq(const LISTA *lista);
bool lista_cheia_seq(const LISTA *lista);
void lista_imprimir_seq(const LISTA *lista);

#endif 
/* LISTA_SEQ_H */




/*#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"

#define CAPACIDADE_INICIAL 100

typedef struct lista_ LISTA;

    Cria uma lista sequencial dinâmica.
   Se 'ordenada' for true, comporta-se como lista_ord (inserção mantendo a ordem).
   Se 'ordenada' for false, comporta-se como lista_seq comum (inserção no final). 
LISTA *lista_criar(bool ordenada);

void lista_apagar(LISTA **lista);
bool lista_inserir(LISTA *lista, ITEM *item);
ITEM *lista_busca(const LISTA *lista, int chave);
ITEM *lista_remover(LISTA *lista, int chave);

int lista_tamanho(const LISTA *lista);
bool lista_vazia(const LISTA *lista);
bool lista_cheia(const LISTA *lista); // Retorna true apenas se faltar memória no sistema
void lista_imprimir(const LISTA *lista);

#endif*/