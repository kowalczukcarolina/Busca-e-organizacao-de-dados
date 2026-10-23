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
};

static bool lista_redimensionar_seq(LISTA_SEQ *lista) {
    int nova_capacidade = lista->capacidade * 2;
    ITEM **novos_itens = realloc(lista->itens, sizeof(ITEM *) * nova_capacidade);
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

static int lista_busca_posicao_seq(const LISTA_SEQ *lista, const char *chave) {
    return lista->ordenada ? lista_busca_binaria_seq(lista, chave)
                           : lista_busca_sequencial_seq(lista, chave);
}

static bool lista_inserir_posicao_seq(LISTA_SEQ *lista, int pos, ITEM *item) {
    if (pos < 0 || pos > lista->fim)
        return false;

    if (lista->fim == lista->capacidade) {
        if (!lista_redimensionar_seq(lista))
            return false;
    }

    for (int i = lista->fim; i > pos; i--)
        lista->itens[i] = lista->itens[i - 1];

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
    lista->itens = malloc(sizeof(ITEM *) * lista->capacidade);

    if (lista->itens == NULL) {
        free(lista);
        return NULL;
    }

    return lista;
}

void lista_apagar_seq(LISTA_SEQ **lista) {
    if (lista == NULL || *lista == NULL)
        return;

    for (int i = 0; i < (*lista)->fim; i++) {
        item_destruir(&(*lista)->itens[i]);
    }
    free((*lista)->itens);
    free(*lista);
    *lista = NULL;
}

bool lista_inserir_seq(LISTA_SEQ *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    int pos = lista->ordenada ? lista_lower_bound_seq(lista, item_get_chave(item)) : lista->fim;
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

    for (int i = pos; i < lista->fim - 1; i++)
        lista->itens[i] = lista->itens[i + 1];

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

// Retorna o item na posição 'index' do vetor interno
ITEM *lista_obter_item_seq(const LISTA_SEQ *lista, int index) {
    if (lista == NULL || index < 0 || index >= lista->fim) {
        return NULL;
    }
    return lista->itens[index];
}