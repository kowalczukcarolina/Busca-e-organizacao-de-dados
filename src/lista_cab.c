#include "../include/lista_cab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct no_ NO;

struct no_ {
    ITEM *item;
    NO *proximo;  //no para  proximo item da fila
};

struct lista_cab {
    NO *cabeca;   // nó estrutural -- não representa um item da lista
    NO *fim;      // último nó de dados é NULL quando a lista está vazia
    int tamanho;
};

//Construtor -- cria a lista e o nó cabeça (sem item)
LISTA_CAB *lista_criar_cab(void) {
    LISTA_CAB *lista = malloc(sizeof(LISTA_CAB));
    if (lista == NULL)
        return NULL;

    lista->cabeca = malloc(sizeof(NO));
    if (lista->cabeca == NULL) {
        free(lista);
        return NULL;
    }

    lista->cabeca->item = NULL;
    lista->cabeca->proximo = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    return lista;
}

//Apaga lista_cab e liberar memória
void lista_apagar_cab(LISTA_CAB **lista, bool destruir_itens) {
    if (lista == NULL || *lista == NULL)
        return;

    NO *p = (*lista)->cabeca->proximo;
    while (p != NULL) {
        NO *proximo = p->proximo;
        if (destruir_itens)
            item_destruir(&p->item);
        free(p);
        p = proximo;
    }

    free((*lista)->cabeca);
    free(*lista);
    *lista = NULL;
}

//Consultas se a lista esta vazia e tamanho da lista
bool lista_vazia_cab(const LISTA_CAB *lista) {
    return lista == NULL || lista->cabeca->proximo == NULL;
}

int lista_tamanho_cab(const LISTA_CAB *lista) {
    return lista != NULL ? lista->tamanho : 0;
}

// Insere no fim da lista (após o último nó de dados)
bool lista_inserir_cab(LISTA_CAB *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    NO *novo = malloc(sizeof(NO));
    if (novo == NULL)
        return false;

    novo->item = item;
    novo->proximo = NULL;

    if (lista->fim == NULL)
        lista->cabeca->proximo = novo;
    else
        lista->fim->proximo = novo;

    lista->fim = novo;
    lista->tamanho++;
    return true;
}

// Busca o item com a chave especificada na lista. Retorna NULL se não encontrado.
ITEM *lista_buscar_cab(const LISTA_CAB *lista, const char *chave) {
    if (lista == NULL)
        return NULL;

    for (const NO *p = lista->cabeca->proximo; p != NULL; p = p->proximo) {
        if (strcmp(item_get_chave(p->item), chave) == 0)
            return p->item;
    }

    return NULL;
}

// Remove o item com a chave especificada da lista. Retorna o item removido ou NULL se não encontrado.
ITEM *lista_remover_cab(LISTA_CAB *lista, const char *chave) {
    if (lista == NULL)
        return NULL;

    NO *p = lista->cabeca;
    while (p->proximo != NULL && strcmp(item_get_chave(p->proximo->item), chave) != 0)
        p = p->proximo;

    if (p->proximo == NULL)
        return NULL;

    NO *removido = p->proximo;
    ITEM *item = removido->item;
    p->proximo = removido->proximo;

    if (removido == lista->fim)
        lista->fim = (p == lista->cabeca) ? NULL : p;

    free(removido);
    lista->tamanho--;
    return item;
}

// Percorre os nós, começando em cabeca->proximo (a cabeça nunca é colocada)
void lista_imprimir_cab(const LISTA_CAB *lista) {
    if (lista == NULL) {
        puts("(lista inexistente)");
        return;
    }

    printf("cabeca -> ");
    const NO *p = lista->cabeca->proximo;
    while (p != NULL) {
        item_imprimir(p->item);
        printf(" -> ");
        p = p->proximo;
    }
    printf("NULL  [tamanho=%d]\n", lista->tamanho);
}

// Percorre os nós, começando em cabeca->proximo (a cabeça nunca é colocada)
void lista_percorrer_cab(const LISTA_CAB *lista, ITEM_VISITOR f, void *ctx) {
    if (lista == NULL || f == NULL)
        return;

    for (const NO *p = lista->cabeca->proximo; p != NULL; p = p->proximo) {
        if (!f(p->item, ctx))
            return;
    }
}
