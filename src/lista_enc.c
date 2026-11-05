#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/lista_enc.h"

typedef struct no_ NO;

struct no_ {
    ITEM *item;
    NO *proximo; //ponteiro para o próximo item da lista encadeada
};

struct lista_enc {
    NO *inicio;  // ponteiro para o primeiro nó da lista
    NO *fim;     // ponteiro para o último nó da lista
    int tamanho;   // quantidade atual de elementos na lista
    bool ordenada; // flag que indica se a lista deve se manter ordenada
    char campo[64];// campo usado para ordenação ("" = ordena pela chave padrão)
    bool desc;     // flag para ordenação decrescente (true) ou crescente (false)
};

// Função auxiliar para criar um novo nó com o item dado como parâmetro. Retorna NULL se não houver memória suficiente
static NO *no_criar(ITEM *item) {
    NO *novo = malloc(sizeof(NO));
    if (novo != NULL) {
        novo->item = item;
        novo->proximo = NULL;
    }
    return novo;
}

//Comparador da lista ordenada: respeita o campo/direção definidos pelo ORDER
static int comparar_enc(const LISTA_ENC *l, const ITEM *a, const ITEM *b) {
    if (l->campo[0] == '\0')
        return strcmp(item_get_chave(a), item_get_chave(b));
    int c = item_comparar(a, b, l->campo);
    return l->desc ? -c : c;
}

// Insere no fim da lista encadeada (sem ordenação)
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

//A ordem é mantida na inserção. Insere DEPOIS dos iguais (ordem estável)
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

// Função auxiliar para busca recursiva de um item na lista encadeada. Retorna o item se encontrado, ou NULL caso contrário
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

//construtor -- cria uma lista encadeada vazia, com a flag de ordenação definida pelo parâmetro
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

// função para definir o campo e a direção de ordenação da lista encadeada
void lista_definir_ordem_enc(LISTA_ENC *lista, const char *campo, bool desc) {
    if (lista == NULL || campo == NULL || strlen(campo) >= sizeof lista->campo)
        return;
    strcpy(lista->campo, campo);
    lista->desc = desc;
}

// Função para apagar a lista encadeada e liberar a memória alocada
void lista_apagar_enc(LISTA_ENC **lista, bool destruir_itens) {
    if (lista == NULL || *lista == NULL)
        return;

    NO *p = (*lista)->inicio;
    while (p != NULL) {
        NO *proximo = p->proximo;
        if (destruir_itens) // Libera o item se destruir_itens for TRUE
            item_destruir(&p->item);
        free(p);
        p = proximo;
    }

    free(*lista);
    *lista = NULL;
}

// Função para inserir um item na lista encadeada(respeitando a ordenação caso o bool for verdade)
bool lista_inserir_enc(LISTA_ENC *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    return lista->ordenada
        ? inserir_ordenado_enc(lista, item)
        : inserir_fim_enc(lista, item);
}

// Função para buscar um item na lista encadeada retorna o item se encontrado, ou NULL caso contrário
ITEM *lista_busca_enc(const LISTA_ENC *lista, const char *chave) {
    if (lista == NULL)
        return NULL;

    //serve para otimizar a busca em listas ordenadas, evitando percorrer elementos desnecessariamente para a execução do algoritmo de busca(Se o elemento atual é maior do que o que estamos procurando, todos os próximos elementos da lista serão ainda maiores.)
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

// Função para buscar um item na lista encadeada de forma recursiva retorna o item se encontrado, ou NULL caso contrário
ITEM *lista_busca_recursiva_enc(const LISTA_ENC *lista, const char *chave) {
    if (lista == NULL)
        return NULL;

    return busca_no_recursiva_enc(lista->inicio, chave,
                                  lista->ordenada && lista->campo[0] == '\0');
}


// Função para remover um item da lista encadeada com a chave especificada retorna o item removido, ou NULL caso não encontrado
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

//consulta tamanho
int lista_tamanho_enc(const LISTA_ENC *lista) {
    return lista != NULL ? lista->tamanho : 0;
}
//consulta se a lista está vazia
bool lista_vazia_enc(const LISTA_ENC *lista) {
    return lista == NULL || lista->inicio == NULL;
}

//consulta se a lista está cheia (sempre retorna false, pois a lista encadeada não tem limite de tamanho)
bool lista_cheia_enc(const LISTA_ENC *lista) {
    (void)lista;
    return false;
}

//imprime a lista encadeada
void lista_imprimir_enc(const LISTA_ENC *lista) {
    if (lista == NULL) {
        printf("<lista inexistente>\n");
        return;
    }

    //formato de impressão: inicio -> item1 -> item2 -> ... -> NULL
    printf("inicio -> ");
    for (const NO *p = lista->inicio; p != NULL; p = p->proximo) {
        item_imprimir(p->item);
        printf(" -> ");
    }
    printf("NULL\n");
}

//percorre a lista encadeada
void lista_percorrer_enc(const LISTA_ENC *lista, ITEM_VISITOR f, void *ctx) {
    if (lista == NULL || f == NULL)
        return;

    for (const NO *p = lista->inicio; p != NULL; p = p->proximo) {
        if (!f(p->item, ctx))
            return;
    }
}
