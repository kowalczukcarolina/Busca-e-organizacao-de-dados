#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/tads/lista_seq.h"

struct lista_seq {
    ITEM **itens;
    int capacidade;
    int fim;
    bool ordenada;
    char campo[64];   /* "" = ordena pela chave padrão (item_get_chave), crescente */
    bool desc;
};

/* Comparador da lista ordenada: respeita o campo/direção definidos pelo ORDER. */
static int comparar_seq(const LISTA_SEQ *l, const ITEM *a, const ITEM *b) {
    if (l->campo[0] == '\0')
        return strcmp(item_get_chave(a), item_get_chave(b));
    int c = item_comparar(a, b, l->campo);
    return l->desc ? -c : c;
}

/* Posição de inserção por busca binária: logo DEPOIS dos iguais (ordem estável). */
static int posicao_insercao_seq(const LISTA_SEQ *l, const ITEM *novo) {
    int inf = 0;
    int sup = l->fim;
    while (inf < sup) {
        int meio = inf + (sup - inf) / 2;
        if (comparar_seq(l, l->itens[meio], novo) <= 0)
            inf = meio + 1;
        else
            sup = meio;
    }
    return inf;
}

static bool lista_redimensionar_seq(LISTA_SEQ *lista) {
    int nova_capacidade = lista->capacidade * 2;
    ITEM **novos_itens = realloc(lista->itens, sizeof(ITEM *) * (size_t)nova_capacidade);
    if (novos_itens == NULL)
        return false;

    lista->itens = novos_itens;
    lista->capacidade = nova_capacidade;
    return true;
}

static int lista_lower_bound_seq(const LISTA_SEQ *lista, const char *chave) {
    int inf = 0;
    int sup = lista->fim;
    while (inf < sup) {
        int meio = inf + (sup - inf) / 2;
        if (strcmp(item_get_chave(lista->itens[meio]), chave) < 0)
            inf = meio + 1;
        else
            sup = meio;
    }
    return inf;
}

static int lista_busca_binaria_seq(const LISTA_SEQ *lista, const char *chave) {
    int pos = lista_lower_bound_seq(lista, chave);
    if (pos < lista->fim && strcmp(item_get_chave(lista->itens[pos]), chave) == 0)
        return pos;
    return -1;
}

static int lista_busca_sequencial_seq(const LISTA_SEQ *lista, const char *chave) {
    for (int i = 0; i < lista->fim; i++) {
        if (strcmp(item_get_chave(lista->itens[i]), chave) == 0)
            return i;
    }
    return -1;
}

/* Busca binária só é válida quando a lista está ordenada pela chave padrão. */
static int lista_busca_posicao_seq(const LISTA_SEQ *lista, const char *chave) {
    return (lista->ordenada && lista->campo[0] == '\0')
               ? lista_busca_binaria_seq(lista, chave)
               : lista_busca_sequencial_seq(lista, chave);
}

static bool lista_inserir_posicao_seq(LISTA_SEQ *lista, int pos, ITEM *item) {
    if (pos < 0 || pos > lista->fim)
        return false;

    if (lista->fim == lista->capacidade) {
        if (!lista_redimensionar_seq(lista))
            return false;
    }

    if (pos < lista->fim)
        memmove(&lista->itens[pos + 1], &lista->itens[pos],
                (size_t)(lista->fim - pos) * sizeof(ITEM *));

    lista->itens[pos] = item;
    lista->fim++;
    return true;
}

LISTA_SEQ *lista_criar_seq(bool ordenada) {
    LISTA_SEQ *lista = malloc(sizeof(LISTA_SEQ));
    if (lista == NULL)
        return NULL;

    lista->capacidade = CAPACIDADE_INICIAL;
    lista->fim = 0;
    lista->ordenada = ordenada;
    lista->campo[0] = '\0';
    lista->desc = false;
    lista->itens = malloc(sizeof(ITEM *) * (size_t)lista->capacidade);

    if (lista->itens == NULL) {
        free(lista);
        return NULL;
    }

    return lista;
}

void lista_definir_ordem_seq(LISTA_SEQ *lista, const char *campo, bool desc) {
    if (lista == NULL || campo == NULL || strlen(campo) >= sizeof lista->campo)
        return;
    strcpy(lista->campo, campo);
    lista->desc = desc;
}

void lista_apagar_seq(LISTA_SEQ **lista, bool destruir_itens) {
    if (lista == NULL || *lista == NULL)
        return;

    if (destruir_itens) {
        for (int i = 0; i < (*lista)->fim; i++)
            item_destruir(&(*lista)->itens[i]);
    }
    free((*lista)->itens);
    free(*lista);
    *lista = NULL;
}

bool lista_inserir_seq(LISTA_SEQ *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    int pos = lista->ordenada ? posicao_insercao_seq(lista, item) : lista->fim;
    return lista_inserir_posicao_seq(lista, pos, item);
}

ITEM *lista_busca_seq(const LISTA_SEQ *lista, const char *chave) {
    if (lista == NULL || lista_vazia_seq(lista))
        return NULL;

    int pos = lista_busca_posicao_seq(lista, chave);
    return pos >= 0 ? lista->itens[pos] : NULL;
}

ITEM *lista_remover_seq(LISTA_SEQ *lista, const char *chave) {
    if (lista == NULL || lista_vazia_seq(lista))
        return NULL;

    int pos = lista_busca_posicao_seq(lista, chave);
    if (pos < 0)
        return NULL;

    ITEM *item = lista->itens[pos];

    memmove(&lista->itens[pos], &lista->itens[pos + 1],
            (size_t)(lista->fim - pos - 1) * sizeof(ITEM *));

    lista->fim--;
    return item;
}

int lista_tamanho_seq(const LISTA_SEQ *lista) {
    return lista != NULL ? lista->fim : 0;
}

bool lista_vazia_seq(const LISTA_SEQ *lista) {
    return lista == NULL || lista->fim == 0;
}

bool lista_cheia_seq(const LISTA_SEQ *lista) {
    (void)lista;
    return false;
}

void lista_imprimir_seq(const LISTA_SEQ *lista) {
    if (lista == NULL)
        return;

    for (int i = 0; i < lista->fim; i++) {
        item_imprimir(lista->itens[i]);
    }
}

void lista_percorrer_seq(const LISTA_SEQ *lista, ITEM_VISITOR f, void *ctx) {
    if (lista == NULL || f == NULL)
        return;

    for (int i = 0; i < lista->fim; i++) {
        if (!f(lista->itens[i], ctx))
            return;
    }
}
