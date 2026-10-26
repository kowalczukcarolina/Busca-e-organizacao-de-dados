#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/estrutura.h"
#include "../include/tads/pilha.h"
#include "../include/tads/fila.h"
#include "../include/tads/lista_seq.h"
#include "../include/tads/lista_enc.h"
#include "../include/tads/lista_cab.h"
#include "../include/tads/lista_gen.h"
#include "../include/tads/lista_cruz.h"

#define MAX_NIVEIS 8
#define TAM_CHAVE  64

typedef enum { T_PILHA, T_FILA, T_SEQ, T_ORD, T_ENC, T_CAB, T_GEN, T_CRUZ } TIPO;

struct estrutura {
    TIPO tipo;
    void *ptr;
};

static bool parse_tipo(const char *s, TIPO *t) {
    if (strcmp(s, "pilha") == 0)          *t = T_PILHA;
    else if (strcmp(s, "fila") == 0)      *t = T_FILA;
    else if (strcmp(s, "lista_seq") == 0) *t = T_SEQ;
    else if (strcmp(s, "lista_ord") == 0) *t = T_ORD;
    else if (strcmp(s, "lista_enc") == 0) *t = T_ENC;
    else if (strcmp(s, "lista_cabeca") == 0 || strcmp(s, "lista_cab") == 0) *t = T_CAB;
    else if (strcmp(s, "lista_gen") == 0) *t = T_GEN;
    else if (strcmp(s, "lista_cruz") == 0) *t = T_CRUZ;
    else return false;
    return true;
}

ESTRUTURA *est_criar(const char *nome) {
    TIPO t;
    if (!nome || !parse_tipo(nome, &t)) return NULL;

    ESTRUTURA *e = malloc(sizeof *e);
    if (!e) return NULL;

    e->tipo = t;
    switch (t) {
        case T_PILHA: e->ptr = pilha_criar(); break;
        case T_FILA:  e->ptr = fila_criar(); break;
        case T_SEQ:   e->ptr = lista_criar_seq(false); break;
        case T_ORD:   e->ptr = lista_criar_seq(true); break;
        case T_ENC:   e->ptr = lista_criar_enc(false); break;
        case T_CAB:   e->ptr = lista_criar_cab(); break;
        case T_GEN:   e->ptr = lgen_criar(); break;
        case T_CRUZ:  e->ptr = lcruz_criar(); break;
        default:      e->ptr = NULL;
    }
    if (!e->ptr) {
        free(e);
        return NULL;
    }
    return e;
}

void est_configurar_ordem(ESTRUTURA *e, const char *campo, bool desc) {
    if (e && e->tipo == T_ORD)
        lista_definir_ordem_seq(e->ptr, campo, desc);
}

bool est_inserir(ESTRUTURA *e, ITEM *item, const QUERY *q) {
    if (!e || !item) return false;

    switch (e->tipo) {
        case T_PILHA: return pilha_empilhar(e->ptr, item);
        case T_FILA:  return fila_inserir(e->ptr, item);
        case T_SEQ:
        case T_ORD:   return lista_inserir_seq(e->ptr, item);
        case T_ENC:   return lista_inserir_enc(e->ptr, item);
        case T_CAB:   return lista_inserir_cab(e->ptr, item);
        case T_GEN: {
            // GROUP|campo1|campo2 -> grupo por campo1 > subgrupo por campo2 > item
            int n = q ? query_group_tamanho(q) : 0;
            char buf[MAX_NIVEIS][TAM_CHAVE];
            const char *chaves[MAX_NIVEIS];
            if (n > MAX_NIVEIS) n = MAX_NIVEIS;
            for (int i = 0; i < n; i++) {
                if (!item_obter_valor_texto(item, query_group_campo(q, i), buf[i], TAM_CHAVE))
                    snprintf(buf[i], TAM_CHAVE, "desconhecido");
                chaves[i] = buf[i];
            }
            return lgen_inserir_agrupado(e->ptr, item, chaves, n);
        }
        case T_CRUZ: {
            // CROSS|campo_linha|campo_coluna -> o mesmo nó entra na linha e na coluna
            char l[TAM_CHAVE] = "geral";
            char c[TAM_CHAVE] = "geral";
            if (q && query_cross_tamanho(q) > 0 &&
                !item_obter_valor_texto(item, query_cross_campo(q, 0), l, sizeof l))
                snprintf(l, sizeof l, "desconhecido");
            if (q && query_cross_tamanho(q) > 1 &&
                !item_obter_valor_texto(item, query_cross_campo(q, 1), c, sizeof c))
                snprintf(c, sizeof c, "desconhecido");
            return lcruz_inserir(e->ptr, item, l, c) != 0;
        }
    }
    return false;
}

int est_tamanho(ESTRUTURA *e) {
    if (!e) return 0;
    switch (e->tipo) {
        case T_PILHA: return pilha_tamanho(e->ptr);
        case T_FILA:  return fila_tamanho(e->ptr);
        case T_SEQ:
        case T_ORD:   return lista_tamanho_seq(e->ptr);
        case T_ENC:   return lista_tamanho_enc(e->ptr);
        case T_CAB:   return lista_tamanho_cab(e->ptr);
        case T_GEN:   return lgen_tamanho(e->ptr);
        case T_CRUZ:  return lcruz_tamanho(e->ptr);
    }
    return 0;
}

void est_percorrer(ESTRUTURA *e, ITEM_VISITOR f, void *ctx) {
    if (!e || !f) return;
    switch (e->tipo) {
        case T_PILHA: pilha_percorrer(e->ptr, f, ctx); break;
        case T_FILA:  fila_percorrer(e->ptr, f, ctx); break;
        case T_SEQ:
        case T_ORD:   lista_percorrer_seq(e->ptr, f, ctx); break;
        case T_ENC:   lista_percorrer_enc(e->ptr, f, ctx); break;
        case T_CAB:   lista_percorrer_cab(e->ptr, f, ctx); break;
        case T_GEN:   lgen_percorrer(e->ptr, f, ctx); break;
        case T_CRUZ:  lcruz_percorrer(e->ptr, f, ctx); break;
    }
}

void est_apagar(ESTRUTURA **e, bool destruir_itens) {
    if (!e || !*e) return;

    void *p = (*e)->ptr;
    switch ((*e)->tipo) {
        case T_PILHA: pilha_apagar((PILHA **)&p, destruir_itens); break;
        case T_FILA:  fila_apagar((FILA **)&p, destruir_itens); break;
        case T_SEQ:
        case T_ORD:   lista_apagar_seq((LISTA_SEQ **)&p, destruir_itens); break;
        case T_ENC:   lista_apagar_enc((LISTA_ENC **)&p, destruir_itens); break;
        case T_CAB:   lista_apagar_cab((LISTA_CAB **)&p, destruir_itens); break;
        case T_GEN:   lgen_destruir((LISTA_GEN **)&p, destruir_itens); break;
        case T_CRUZ:  lcruz_destruir((LISTA_CRUZADA **)&p, destruir_itens); break;
    }
    free(*e);
    *e = NULL;
}
