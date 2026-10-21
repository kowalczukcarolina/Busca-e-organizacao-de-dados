#include <stdio.h>
#include <stdlib.h>
#include "lista_seq.h"

struct lista_ {
    ITEM **itens;     // Arranjo dinâmico de ponteiros para ITEM
    int capacidade;   // Capacidade atual alocada no vetor
    int fim;          // Quantidade de elementos / Próxima posição livre
    bool ordenada;    // Define se a lista é ordenada (lista_ord) ou não (lista_seq)
};

/* Função auxiliar para redimensionar o vetor quando estiver cheio */
static bool lista_redimensionar(LISTA *lista) {
    int nova_capacidade = lista->capacidade * 2;
    ITEM **novos_itens = realloc(lista->itens, sizeof(ITEM *) * nova_capacidade);
    if (novos_itens == NULL)
        return false;

    lista->itens = novos_itens;
    lista->capacidade = nova_capacidade;
    return true;
}

/* Busca binária para encontrar a posição onde a chave deveria estar ou ser inserida */
static int lista_lower_bound(const LISTA *lista, int chave) {
    int inf = 0;
    int sup = lista->fim;
    while (inf < sup) {
        int meio = inf + (sup - inf) / 2;
        if (item_get_chave(lista->itens[meio]) < chave)
            inf = meio + 1;
        else
            sup = meio;
    }
    return inf;
}

/* Busca binária exata para a lista ordenada */
static int lista_busca_binaria(const LISTA *lista, int chave) {
    int pos = lista_lower_bound(lista, chave);
    if (pos < lista->fim && item_get_chave(lista->itens[pos]) == chave)
        return pos;
    return -1;
}

/* Busca sequencial para a lista comum */
static int lista_busca_sequencial(const LISTA *lista, int chave) {
    for (int i = 0; i < lista->fim; i++) {
        if (item_get_chave(lista->itens[i]) == chave)
            return i;
    }
    return -1;
}

static int lista_busca_posicao(const LISTA *lista, int chave) {
    return lista->ordenada ? lista_busca_binaria(lista, chave)
                           : lista_busca_sequencial(lista, chave);
}

/* Insere um item em uma posição específica deslocando os demais elementos */
static bool lista_inserir_posicao(LISTA *lista, int pos, ITEM *item) {
    if (pos < 0 || pos > lista->fim)
        return false;

    // Se estiver cheia, expande dinamicamente a capacidade
    if (lista->fim == lista->capacidade) {
        if (!lista_redimensionar(lista))
            return false;
    }

    // Desloca elementos para abrir espaço
    for (int i = lista->fim; i > pos; i--)
        lista->itens[i] = lista->itens[i - 1];

    lista->itens[pos] = item;
    lista->fim++;
    return true;
}

LISTA *lista_criar(bool ordenada) {
    LISTA *lista = malloc(sizeof(LISTA));
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

void lista_apagar(LISTA **lista) {
    if (lista == NULL || *lista == NULL)
        return;

    for (int i = 0; i < (*lista)->fim; i++) {
        item_apagar(&(*lista)->itens[i]);
    }
    free((*lista)->itens);
    free(*lista);
    *lista = NULL;
}

bool lista_inserir(LISTA *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    // Se a lista for ordenada, descobre a posição correta por busca binária.
    // Se não for ordenada, insere diretamente no final.
    int pos = lista->ordenada ? lista_lower_bound(lista, item_get_chave(item)) : lista->fim;
    return lista_inserir_posicao(lista, pos, item);
}

ITEM *lista_busca(const LISTA *lista, int chave) {
    if (lista == NULL || lista_vazia(lista))
        return NULL;

    int pos = lista_busca_posicao(lista, chave);
    return pos >= 0 ? lista->itens[pos] : NULL;
}

ITEM *lista_remover(LISTA *lista, int chave) {
    if (lista == NULL || lista_vazia(lista))
        return NULL;

    int pos = lista_busca_posicao(lista, chave);
    if (pos < 0)
        return NULL;

    ITEM *item = lista->itens[pos];

    // Desloca os elementos à esquerda para preencher a lacuna
    for (int i = pos; i < lista->fim - 1; i++)
        lista->itens[i] = lista->itens[i + 1];

    lista->fim--;
    return item;
}

int lista_tamanho(const LISTA *lista) {
    return lista != NULL ? lista->fim : 0;
}

bool lista_vazia(const LISTA *lista) {
    return lista == NULL || lista->fim == 0;
}

bool lista_cheia(const LISTA *lista) {
    /* Como é uma lista dinâmica, só estará cheia se não houver memória livre no sistema. */
    return false;
}

void lista_imprimir(const LISTA *lista) {
    if (lista == NULL)
        return;

    for (int i = 0; i < lista->fim; i++) {
        item_imprimir(lista->itens[i]);
        if (i + 1 < lista->fim)
            printf(" ");
    }
    printf("\n");
}