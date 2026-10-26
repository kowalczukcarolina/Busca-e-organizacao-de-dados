#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/tads/lista_enc.h"

typedef struct no_ NO;

struct no_ {
    ITEM *item;
    NO *proximo;
};

struct lista_enc {
    NO *inicio;
    NO *fim;
    int tamanho;
    bool ordenada;
    char campo[64];   /* "" = ordena pela chave padrão (item_get_chave), crescente */
    bool desc;
};

static NO *no_criar(ITEM *item) {
    NO *novo = malloc(sizeof(NO));
    if (novo != NULL) {
        novo->item = item;
        novo->proximo = NULL;
    }
    return novo;
}

/* Comparador da lista ordenada: respeita o campo/direção definidos pelo ORDER. */
static int comparar_enc(const LISTA_ENC *l, const ITEM *a, const ITEM *b) {
    if (l->campo[0] == '\0')
        return strcmp(item_get_chave(a), item_get_chave(b));
    int c = item_comparar(a, b, l->campo);
    return l->desc ? -c : c;
}

static bool inserir_fim_enc(LISTA_ENC *lista, ITEM *item) {
    NO *novo = no_criar(item);
    if (novo == NULL)
        return false;

    if (lista->inicio == NULL)
        lista->inicio = novo;
    else
        lista->fim->proximo = novo;

    lista->fim = novo;
    lista->tamanho++;
    return true;
}

/* A ordem é mantida na inserção. Insere DEPOIS dos iguais (ordem estável). */
static bool inserir_ordenado_enc(LISTA_ENC *lista, ITEM *item) {
    NO *novo = no_criar(item);
    if (novo == NULL)
        return false;

    NO *anterior = NULL;
    NO *atual = lista->inicio;

    while (atual != NULL && comparar_enc(lista, atual->item, item) <= 0) {
        anterior = atual;
        atual = atual->proximo;
    }

    novo->proximo = atual;
    if (anterior == NULL)
        lista->inicio = novo;
    else
        anterior->proximo = novo;

    if (atual == NULL)
        lista->fim = novo;

    lista->tamanho++;
    return true;
}

static ITEM *busca_no_recursiva_enc(const NO *no, const char *chave, bool ordenada) {
    if (no == NULL)
        return NULL;

    const char *atual = item_get_chave(no->item);
    int comp = strcmp(atual, chave);

    if (comp == 0)
        return no->item;

    if (ordenada && comp > 0)
        return NULL;

    return busca_no_recursiva_enc(no->proximo, chave, ordenada);
}

LISTA_ENC *lista_criar_enc(bool ordenada) {
    LISTA_ENC *lista = malloc(sizeof(LISTA_ENC));
    if (lista == NULL)
        return NULL;

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    lista->ordenada = ordenada;
    lista->campo[0] = '\0';
    lista->desc = false;
    return lista;
}

void lista_definir_ordem_enc(LISTA_ENC *lista, const char *campo, bool desc) {
    if (lista == NULL || campo == NULL || strlen(campo) >= sizeof lista->campo)
        return;
    strcpy(lista->campo, campo);
    lista->desc = desc;
}

void lista_apagar_enc(LISTA_ENC **lista, bool destruir_itens) {
    if (lista == NULL || *lista == NULL)
        return;

    NO *p = (*lista)->inicio;
    while (p != NULL) {
        NO *proximo = p->proximo;
        if (destruir_itens)
            item_destruir(&p->item);
        free(p);
        p = proximo;
    }

    free(*lista);
    *lista = NULL;
}

bool lista_inserir_enc(LISTA_ENC *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    return lista->ordenada
        ? inserir_ordenado_enc(lista, item)
        : inserir_fim_enc(lista, item);
}

ITEM *lista_busca_enc(const LISTA_ENC *lista, const char *chave) {
    if (lista == NULL)
        return NULL;

    /* o corte antecipado só vale quando a ordem é a da chave padrão */
    bool corte = lista->ordenada && lista->campo[0] == '\0';

    for (const NO *p = lista->inicio; p != NULL; p = p->proximo) {
        const char *atual = item_get_chave(p->item);
        int comp = strcmp(atual, chave);
        if (comp == 0)
            return p->item;
        if (corte && comp > 0)
            return NULL;
    }
    return NULL;
}

ITEM *lista_busca_recursiva_enc(const LISTA_ENC *lista, const char *chave) {
    if (lista == NULL)
        return NULL;

    return busca_no_recursiva_enc(lista->inicio, chave,
                                  lista->ordenada && lista->campo[0] == '\0');
}

ITEM *lista_remover_enc(LISTA_ENC *lista, const char *chave) {
    if (lista == NULL || lista->inicio == NULL)
        return NULL;

    bool corte = lista->ordenada && lista->campo[0] == '\0';
    NO *anterior = NULL;
    NO *atual = lista->inicio;

    while (atual != NULL) {
        int comp = strcmp(item_get_chave(atual->item), chave);
        if (comp == 0)
            break;
        if (corte && comp > 0)
            return NULL;
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual == NULL)
        return NULL;

    if (anterior == NULL)
        lista->inicio = atual->proximo;
    else
        anterior->proximo = atual->proximo;

    if (atual == lista->fim)
        lista->fim = anterior;

    ITEM *item = atual->item;
    free(atual);
    lista->tamanho--;
    if (lista->tamanho == 0)
        lista->fim = NULL;
    return item;
}

int lista_tamanho_enc(const LISTA_ENC *lista) {
    return lista != NULL ? lista->tamanho : 0;
}

bool lista_vazia_enc(const LISTA_ENC *lista) {
    return lista == NULL || lista->inicio == NULL;
}

bool lista_cheia_enc(const LISTA_ENC *lista) {
    (void)lista;
    return false;
}

void lista_imprimir_enc(const LISTA_ENC *lista) {
    if (lista == NULL) {
        printf("<lista inexistente>\n");
        return;
    }

    printf("inicio -> ");
    for (const NO *p = lista->inicio; p != NULL; p = p->proximo) {
        item_imprimir(p->item);
        printf(" -> ");
    }
    printf("NULL\n");
}

void lista_percorrer_enc(const LISTA_ENC *lista, ITEM_VISITOR f, void *ctx) {
    if (lista == NULL || f == NULL)
        return;

    for (const NO *p = lista->inicio; p != NULL; p = p->proximo) {
        if (!f(p->item, ctx))
            return;
    }
}
