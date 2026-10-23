#ifndef LISTA_CRUZ_H
#define LISTA_CRUZ_H

#include "item.h"

typedef struct no_cruzado NO_CRUZ;
typedef struct cabeca_eixo CABECA_EIXO;
typedef struct lista_cruzada LISTA_CRUZADA;

LISTA_CRUZADA *lcruz_criar(void);
void lcruz_destruir(LISTA_CRUZADA **lc);
int lcruz_inserir(LISTA_CRUZADA *lc, ITEM *item, const char *chave_linha, const char *chave_coluna);
void lcruz_imprimir_por_linhas(const LISTA_CRUZADA *lc);
void lcruz_imprimir_por_colunas(const LISTA_CRUZADA *lc);

// Funções adicionadas para permitir o percurso por índice compatível com a query
int lcruz_tamanho(const LISTA_CRUZADA *lc);
ITEM *lcruz_obter_item(const LISTA_CRUZADA *lc, int index);

#endif