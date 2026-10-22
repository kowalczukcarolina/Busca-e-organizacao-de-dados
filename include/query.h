#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>
#include "tads/item.h"

/*
 * TAD QUERY: representa uma consulta lida de arquivo texto.
 *
 * Formato suportado (um comando por linha, campos separados por '|'):
 *   FILTER|campo|op|valor        op: == != < <= > >=
 *   SIM|campo|alvo|tolerancia|peso
 *   ACCEPT|limiar
 *   ORDER|campo|ASC/DESC         (campo pode ser "similarity")
 *   GROUP|campo1|campo2|...
 *   CROSS|campo1|campo2
 *
 * Linhas em branco e linhas iniciadas por '#' são ignoradas.
 */

typedef struct query QUERY;

/* Lê e valida o arquivo. Retorna NULL em erro (mensagem em stderr). */
QUERY *query_carregar(const char *caminho);
void query_apagar(QUERY **q);

/*
 * Avalia um item: aplica todos os FILTER, calcula a similaridade total
 * (média ponderada dos SIM) e compara com ACCEPT.
 * Retorna true se o item é aceito; *similaridade recebe o valor calculado
 * (1.0 quando a consulta não tem SIM).
 */
bool query_avaliar(const QUERY *q, const ITEM *item, double *similaridade);

/* ORDER: padrão "similarity" DESC quando a consulta não tem ORDER. */
const char *query_order_campo(const QUERY *q);
bool query_order_desc(const QUERY *q);

/* GROUP e CROSS: quantidade de campos e acesso por índice. */
int query_group_tamanho(const QUERY *q);
const char *query_group_campo(const QUERY *q, int i);
int query_cross_tamanho(const QUERY *q);
const char *query_cross_campo(const QUERY *q, int i);

/* Depuração: imprime a consulta interpretada. */
void query_imprimir(const QUERY *q);

#endif
