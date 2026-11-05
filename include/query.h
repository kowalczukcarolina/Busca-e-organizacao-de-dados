#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>
#include "item.h"

//TAD QUERY: representa uma consulta lida de arquivo texto:
typedef struct query QUERY;

//Lê e valida o arquivo. Retorna NULL em erro.
QUERY *query_carregar(const char *caminho);
void query_apagar(QUERY **q);

//Avalia um item - aplica todos os FILTER, calcula a similaridade total (média ponderada dos SIM) e compara com ACCEPT. Retorna true se o item é aceito
bool query_avaliar(const QUERY *q, const ITEM *item, double *similaridade);

//ORDER - padrão "similarity" DESC quando a consulta não tem ORDER.
const char *query_order_campo(const QUERY *q);
bool query_order_desc(const QUERY *q);

//GROUP e CROSS
int query_group_tamanho(const QUERY *q);
const char *query_group_campo(const QUERY *q, int i);
int query_cross_tamanho(const QUERY *q);
const char *query_cross_campo(const QUERY *q, int i);

//imprime a consulta interpretada.
void query_imprimir(const QUERY *q);

#endif
