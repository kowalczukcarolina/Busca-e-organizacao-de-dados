#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lista_gen.h"

typedef struct no_ NO;

struct no_ {
    int tipo; // 0 = átomo 1 = sublista
    union { //aloca espaço suficiente apenas para o maior dos seus campos, pois o nó nunca será um átomo e uma sublista ao mesmo tempo
        ITEM *atomo;
        struct {
            char *chave; //rótulo do grupo
            NO *inicio;  //primeiro nó do grupo 
            NO *fim;     //último nó do grupo 
        } sub;
    } info;
    NO *prox; /* próximo nó do mesmo nível */
};

struct lista_gen {
    NO *inicio; //ponteiro para o inicio
    NO *fim; //ponteiro para o fim 
    int total; // total de átomos em todos os níveis 
};

// Função auxiliar para anexar um nó ao final de uma lista
static void anexar(NO **inicio, NO **fim, NO *no) {
    if (*fim != NULL)
        (*fim)->prox = no;
    else
        *inicio = no;
    *fim = no;
}

// Função auxiliar para criar um nó do tipo átomo
static NO *criar_no_atomo(ITEM *item) {
    NO *n = malloc(sizeof(NO));
    if (!n) return NULL;
    n->tipo = 0;
    n->info.atomo = item;
    n->prox = NULL;
    return n;
}

// função auxiliar para criar um nó do tipo sublista
static NO *criar_no_sublista(const char *chave) {
    NO *n = malloc(sizeof(NO));
    if (!n) return NULL;
    n->info.sub.chave = malloc(strlen(chave) + 1);
    if (!n->info.sub.chave) {
        free(n);
        return NULL;
    }
    strcpy(n->info.sub.chave, chave);
    n->tipo = 1;
    n->info.sub.inicio = NULL;
    n->info.sub.fim = NULL;
    n->prox = NULL;
    return n;
}

//Procura no nível fornecido como parametro, a sublista com a chave informada.
static NO *achar_sublista(NO *inicio, const char *chave) {
    for (NO *p = inicio; p != NULL; p = p->prox) {
        if (p->tipo == 1 && strcmp(p->info.sub.chave, chave) == 0)
            return p;
    }
    return NULL;
}

//Construtor -- cria a lista generalizada vazia, com ponteiros para o início e fim nulos e total de átomos igual a zero
LISTA_GEN *lgen_criar(void) {
    LISTA_GEN *l = malloc(sizeof(LISTA_GEN));
    if (l) {
        l->inicio = NULL;
        l->fim = NULL;
        l->total = 0;
    }
    return l;
}

//Função para inserir um item na lista generalizada como átomo
bool lgen_inserir_atomo(LISTA_GEN *l, ITEM *item) {
    if (!l || !item) return false;

    NO *no = criar_no_atomo(item);
    if (!no) return false;

    anexar(&l->inicio, &l->fim, no);
    l->total++;
    return true;
}

// Função para inserir um item na lista generalizada em uma sublista especificada por uma sequência de chaves
bool lgen_inserir_agrupado(LISTA_GEN *l, ITEM *item, const char *chaves[], int n) {
    if (!l || !item || n < 0) return false;

    NO **inicio = &l->inicio;
    NO **fim = &l->fim;

    for (int i = 0; i < n; i++) {
        NO *sub = achar_sublista(*inicio, chaves[i]);
        if (!sub) {
            sub = criar_no_sublista(chaves[i]);
            if (!sub) return false;
            anexar(inicio, fim, sub);
        }
        inicio = &sub->info.sub.inicio;
        fim = &sub->info.sub.fim;
    }

    NO *no = criar_no_atomo(item);
    if (!no) return false;

    anexar(inicio, fim, no);
    l->total++;
    return true;
}

// Função para inserir uma sublista na lista generalizada, especificando a chave da sublista
bool lgen_inserir_sublista(LISTA_GEN *l, LISTA_GEN *sublista, const char *chave) {
    if (!l || !sublista || !sublista->inicio) return false;

    NO *no = criar_no_sublista(chave ? chave : "");
    if (!no) return false;

    no->info.sub.inicio = sublista->inicio;
    no->info.sub.fim = sublista->fim;
    anexar(&l->inicio, &l->fim, no);
    l->total += sublista->total;

    //os nós internos foram transferidos: libera só o descritor
    free(sublista);
    return true;
}

// Função auxiliar para destruir os nós de uma lista generalizada, liberando memória de forma recursiva
static void destruir_nos(NO *no, bool destruir_itens) {
    while (no != NULL) {
        NO *prox = no->prox;

        if (no->tipo == 1) {
            destruir_nos(no->info.sub.inicio, destruir_itens);
            free(no->info.sub.chave);
        } else if (destruir_itens) {
            item_destruir(&no->info.atomo);
        }
        free(no);
        no = prox;
    }
}

// Função para destruir a lista generalizada, liberando toda a memória alocada, incluindo os itens caso seja pedido
void lgen_destruir(LISTA_GEN **l, bool destruir_itens) {
    if (!l || !*l) return;
    destruir_nos((*l)->inicio, destruir_itens);
    free(*l);
    *l = NULL;
}

// Função auxiliar para percorrer os nós de uma lista generalizada e imprimir recursivamente a estrutura de uma Lista Generalizada
static void imprimir_nos(const NO *no) {
    printf("(");
    for (const NO *p = no; p != NULL; p = p->prox) {
        if (p->tipo == 1) {
            printf("%s", p->info.sub.chave);
            imprimir_nos(p->info.sub.inicio);
        } else {
            printf("%s", item_obter_valor_string(p->info.atomo, "asteroid_designation"));
        }

        if (p->prox != NULL)
            printf(", ");
    }
    printf(")");
}

// Função para imprimir a estrutura da lista generalizada
void lgen_imprimir(const LISTA_GEN *l) {
    if (!l) {
        printf("()\n");
        return;
    }
    imprimir_nos(l->inicio);
    printf("\n");
}

// Função auxiliar para calcular a profundidade máxima da lista generalizada de forma recursiva
static int calcular_profundidade_nos(const NO *no) {
    int max_prof = 0;
    for (const NO *p = no; p != NULL; p = p->prox) {
        int prof_atual = 1;
        if (p->tipo == 1)
            prof_atual = 1 + calcular_profundidade_nos(p->info.sub.inicio);
        if (prof_atual > max_prof)
            max_prof = prof_atual;
    }
    return max_prof;
}

//consulta -- profundidade maxima da lista generalizada
int lgen_obter_profundidade(const LISTA_GEN *l) {
    if (!l || !l->inicio) return 0;
    return calcular_profundidade_nos(l->inicio);
}

//consulta -- tamanho da lista generalizada
int lgen_tamanho(const LISTA_GEN *l) {
    return l ? l->total : 0;
}

// Função auxiliar para percorrer os nós de uma lista generalizada 
static bool percorrer_nos(const NO *no, ITEM_VISITOR f, void *ctx) {
    for (const NO *p = no; p != NULL; p = p->prox) {
        if (p->tipo == 1) {
            if (!percorrer_nos(p->info.sub.inicio, f, ctx))
                return false;
        } else if (!f(p->info.atomo, ctx)) {
            return false;
        }
    }
    return true;
}

// Função para percorrer todos os itens da lista generalizada
void lgen_percorrer(const LISTA_GEN *l, ITEM_VISITOR f, void *ctx) {
    if (l && f)
        percorrer_nos(l->inicio, f, ctx);
}
