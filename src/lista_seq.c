#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/tads/lista_seq.h"

struct lista_seq {
    ITEM **itens;   //ponteiro duplo para vetor dinâmico de ponteiros
    int capacidade; //capacidade total alocada
    int fim;        //quantidade de elementos
    bool ordenada;  //indica se está ordenado
    char campo[64]; // Se está vazio ("")= ordena pela chave padrão (item_get_chave), crescente
    bool desc;      //crescente ou decrescente
};

//Comparador da lista ordenada: respeita o campo/direção definidos pelo ORDER
static int comparar_seq(const LISTA_SEQ *l, const ITEM *a, const ITEM *b) {
    if (l->campo[0] == '\0')
        return strcmp(item_get_chave(a), item_get_chave(b));
    int c = item_comparar(a, b, l->campo);
    return l->desc ? -c : c;
}

//Posição de inserção por busca binária: logo DEPOIS dos iguais (ordem estável)
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

//Função para aumentar a capacidade do vetor quando estiver cheio
static bool lista_redimensionar_seq(LISTA_SEQ *lista) {
    int nova_capacidade = lista->capacidade * 2;
    ITEM **novos_itens = realloc(lista->itens, sizeof(ITEM *) * (size_t)nova_capacidade); //utilizando realloc para aumentar a capacidade
    if (novos_itens == NULL)
        return false;

    lista->itens = novos_itens;
    lista->capacidade = nova_capacidade;
    return true;
}

//Função auxiliar para busca binária
//Lower bound = limite inferior
static int lista_lower_bound_seq(const LISTA_SEQ *lista, const char *chave) {
    int inf = 0;
    int sup = lista->fim;
    //Divisão e conquista/busca binária
    while (inf < sup) {
        int meio = inf + (sup - inf) / 2;
        if (strcmp(item_get_chave(lista->itens[meio]), chave) < 0)
            inf = meio + 1;
        else
            sup = meio;
    }
    return inf;
}

//Função de busca binária
static int lista_busca_binaria_seq(const LISTA_SEQ *lista, const char *chave) {
    int pos = lista_lower_bound_seq(lista, chave);
    if (pos < lista->fim && strcmp(item_get_chave(lista->itens[pos]), chave) == 0)
        return pos;
    return -1;
}

//Função que percorre o vetor de forma linear, retorna a posição int
static int lista_busca_sequencial_seq(const LISTA_SEQ *lista, const char *chave) {
    for (int i = 0; i < lista->fim; i++) {
        if (strcmp(item_get_chave(lista->itens[i]), chave) == 0)
            return i;
    }
    return -1;
}

//Busca binária só é válida quando a lista está ordenada pela chave padrão
static int lista_busca_posicao_seq(const LISTA_SEQ *lista, const char *chave) {
    return (lista->ordenada && lista->campo[0] == '\0')
               ? lista_busca_binaria_seq(lista, chave)
               : lista_busca_sequencial_seq(lista, chave);
}

//Função para inserir item numa posição especifica (pos)
static bool lista_inserir_posicao_seq(LISTA_SEQ *lista, int pos, ITEM *item) {
    if (pos < 0 || pos > lista->fim)
        return false;

    if (lista->fim == lista->capacidade) { //verifica se está cheia
        if (!lista_redimensionar_seq(lista)) //tenta chamar a função de redimensionar
            return false;
    }
    //memmove: utilizado para abrir espaço se quiser inserir no meio ou no início da lista
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

//Função para definir se a lista é crescente ou decrescente
void lista_definir_ordem_seq(LISTA_SEQ *lista, const char *campo, bool desc) {
    if (lista == NULL || campo == NULL || strlen(campo) >= sizeof lista->campo)
        return;
    strcpy(lista->campo, campo);
    lista->desc = desc;
}

//Liberação de memória
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

//Função para inserir item na lista
bool lista_inserir_seq(LISTA_SEQ *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    int pos = lista->ordenada ? posicao_insercao_seq(lista, item) : lista->fim;
    return lista_inserir_posicao_seq(lista, pos, item);
}

//Busca item pela chave
ITEM *lista_busca_seq(const LISTA_SEQ *lista, const char *chave) {
    if (lista == NULL || lista_vazia_seq(lista))
        return NULL;

    int pos = lista_busca_posicao_seq(lista, chave);
    return pos >= 0 ? lista->itens[pos] : NULL;
}

//Remove item pela chave
ITEM *lista_remover_seq(LISTA_SEQ *lista, const char *chave) {
    if (lista == NULL || lista_vazia_seq(lista))
        return NULL;

    int pos = lista_busca_posicao_seq(lista, chave);//chamando função que retorna a posição do item
    if (pos < 0)
        return NULL;

    ITEM *item = lista->itens[pos];
    //Usando o memmove para tapar o buraco
    memmove(&lista->itens[pos], &lista->itens[pos + 1],
            (size_t)(lista->fim - pos - 1) * sizeof(ITEM *));

    lista->fim--;
    return item;
}

/*-----Consultas--------*/
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

//Impressão
void lista_imprimir_seq(const LISTA_SEQ *lista) {
    if (lista == NULL)
        return;

    for (int i = 0; i < lista->fim; i++) {
        item_imprimir(lista->itens[i]);
    }
}

//Função para ter segurança para e não expor os itens 
void lista_percorrer_seq(const LISTA_SEQ *lista, ITEM_VISITOR f, void *ctx) {
    if (lista == NULL || f == NULL)
        return;

    for (int i = 0; i < lista->fim; i++) {
        if (!f(lista->itens[i], ctx))
            return;
    }
}
