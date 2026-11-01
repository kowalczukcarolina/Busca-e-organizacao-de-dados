#ifndef LISTA_CRUZ_H
#define LISTA_CRUZ_H

#include <stdbool.h>
#include "item.h"

typedef struct no_cruzado NO_CRUZ;
typedef struct cabeca_eixo CABECA_EIXO;
typedef struct lista_cruzada LISTA_CRUZADA;

LISTA_CRUZADA *lcruz_criar(void);
// destruir_itens = false: apenas libera os nós (os itens pertencem a outra estrutura)
void lcruz_destruir(LISTA_CRUZADA **lc, bool destruir_itens);
// Um único nó por item participa da linha e da coluna ao mesmo tempo (sem duplicar o registro).
int lcruz_inserir(LISTA_CRUZADA *lc, ITEM *item, const char *chave_linha, const char *chave_coluna);
void lcruz_imprimir_por_linhas(const LISTA_CRUZADA *lc);
void lcruz_imprimir_por_colunas(const LISTA_CRUZADA *lc);

int lcruz_tamanho(const LISTA_CRUZADA *lc);

// Percurso por linhas (ex.: por threat_category) ou por colunas (ex.: por year), O(n) total.
void lcruz_percorrer(const LISTA_CRUZADA *lc, ITEM_VISITOR f, void *ctx);
void lcruz_percorrer_colunas(const LISTA_CRUZADA *lc, ITEM_VISITOR f, void *ctx);

#endif
