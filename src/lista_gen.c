#include <stdio.h>
#include <stdlib.h>
#include "../include/tads/lista_gen.h"

// Forward declaration para o nó (struct no_)
typedef struct no_ NO;

// Átomo: não contém outras listas lá dentro -> átomo é o item para o asteroide
struct no_ {
    int tipo; // 0 para átomo, 1 para sub-lista
    union { // union info funciona como um if else de structs
        ITEM *atomo; // se tipo==0, aponta para um item (asteroide)
        NO *sublista; // se tipo == 1, aponta para o inicio de outra sublista
    } info;
    NO *prox; // aponta para o proximo nó do mesmo nível
};

struct lista_gen {
    NO *inicio;
    int profundidade;
};

//Cria uma lista generalizada vazia
LISTA_GEN *lgen_criar(void) {
    LISTA_GEN *l = (LISTA_GEN *) malloc(sizeof(LISTA_GEN)); // alocando memória
    if (l) {
        l->inicio = NULL; // lista começa vazia
        l->profundidade = 0; // lista começa sem profundidade
    }
    return l;
}

//Auxiliar: Cria um nó do tipo Átomo (tipo = 0)
static NO *criar_no_atomo(ITEM *item) {
    NO *novo = (NO *) malloc(sizeof(NO));
    if (!novo) return NULL;
    novo->tipo = 0;
    novo->info.atomo = item;
    novo->prox = NULL;
    return novo;
}

//Auxiliar: Cria um nó do tipo Sublista (tipo = 1)
static NO *criar_no_sublista(NO *inicio_sublista) {
    NO *novo = (NO *) malloc(sizeof(NO));
    if (!novo) return NULL;
    novo->tipo = 1;
    novo->info.sublista = inicio_sublista;
    novo->prox = NULL;
    return novo;
}

//Insere um átomo (ITEM*) no final da lista generalizada atual
bool lgen_inserir_atomo(LISTA_GEN *l, ITEM *item) {
    if (!l || !item) return false;

    NO *novo = criar_no_atomo(item);
    if (!novo) return false;

    if (l->inicio == NULL) {
        l->inicio = novo;
    } else {
        NO *p = l->inicio;
        while (p->prox != NULL) {
            p = p->prox;
        }
        p->prox = novo;
    }
    return true;
}

//Insere uma sublista completa como um elemento/nó no final da lista principal
bool lgen_inserir_sublista(LISTA_GEN *l, LISTA_GEN *sublista) {
    if (!l || !sublista || !sublista->inicio) return false; //verificando se não são nulas

    NO *novo = criar_no_sublista(sublista->inicio); //criando no de sublista
    if (!novo) return false;

    if (l->inicio == NULL) { //se a lista principal estiver vazia, insere como primeiro elemento
        l->inicio = novo;
    } else { //se a lista principal não esta vazia, percorre os nós irmão pelo "prox" até encontrar o ultimo elemento da lista
        NO *p = l->inicio;
        while (p->prox != NULL) {
            p = p->prox;
        }
        p->prox = novo;
    }

    // Libera apenas o descritor da sublista, pois os nós internos foram transferidos
    free(sublista);
    return true;
}

// Função auxiliar recursiva para destruir os nós de uma sub-lista
static void destruir_nos(NO *no) {
    while (no != NULL) {
        NO *prox = no->prox;

        // percorrendo os nós através do prox
        // 0: átomo
        // 1: sub-lista
        if (no->tipo == 1) {
            // Se for sub-lista, destrói recursivamente os nós dela
            destruir_nos(no->info.sublista);
        } else {
            // Se for átomo, destrói o ITEM utilizando o TAD Item
            item_destruir(&(no->info.atomo));
        }
        free(no);
        no = prox;
    }
}

// Destrói toda a lista generalizada e liberta a memória
void lgen_destruir(LISTA_GEN **l) {
    if (!l || !*l) return;
    destruir_nos((*l)->inicio);
    free(*l);
    *l = NULL;
}

// Função auxiliar recursiva para imprimir os nós
static void imprimir_nos(NO *no) {
    printf("(");
    NO *p = no;
    while (p != NULL) {
        if (p->tipo == 1) {
            imprimir_nos(p->info.sublista); // Chamada recursiva para sub-lista
        } else {
            // Imprime o valor do átomo (adaptado para a designação do asteroide do nosso projeto)
            printf("%s", item_obter_valor_string(p->info.atomo, "asteroid_designation"));
        }
        
        if (p->prox != NULL) {
            printf(", ");
        }
        p = p->prox;
    }
    printf(")");
}

// Sub-rotina para imprimir a lista generalizada
void lgen_imprimir(const LISTA_GEN *l) {
    if (!l) {
        printf("()\n");
        return;
    }
    imprimir_nos(l->inicio);
    printf("\n");
}

// Função auxiliar para calcular a profundidade recursivamente
static int calcular_profundidade_nos(NO *no) {
    if (no == NULL) return 0;
    
    int max_prof = 0;
    NO *p = no;
    while (p != NULL) {
        int prof_atual = 0;
        if (p->tipo == 1) {
            prof_atual = 1 + calcular_profundidade_nos(p->info.sublista);
        } else {
            prof_atual = 1; // Um átomo no nível atual conta como profundidade 1 em relação a si
        }
        if (prof_atual > max_prof) {
            max_prof = prof_atual;
        }
        p = p->prox;
    }
    return max_prof;
}

// Sub-rotina para determinar a profundidade máxima
int lgen_obter_profundidade(const LISTA_GEN *l) {
    if (!l || !l->inicio) return 0;
    return calcular_profundidade_nos(l->inicio);
}

// Função auxiliar recursiva para contar todos os átomos armazenados na lista generalizada (incluindo sublistas)
static int contar_atomos_nos(NO *no) {
    int count = 0;
    NO *p = no;
    while (p != NULL) {
        if (p->tipo == 1) {
            count += contar_atomos_nos(p->info.sublista);
        } else if (p->tipo == 0 && p->info.atomo != NULL) {
            count++;
        }
        p = p->prox;
    }
    return count;
}

int lgen_tamanho(const LISTA_GEN *l) {
    if (!l || !l->inicio) return 0;
    return contar_atomos_nos(l->inicio);
}

// Função auxiliar recursiva para obter o átomo pelo índice global linearizado
static ITEM *obter_atomo_por_indice_nos(NO *no, int *atual_idx, int alvo_idx) {
    NO *p = no;
    while (p != NULL) {
        if (p->tipo == 1) {
            ITEM *encontrado = obter_atomo_por_indice_nos(p->info.sublista, atual_idx, alvo_idx);
            if (encontrado != NULL) return encontrado;
        } else if (p->tipo == 0) {
            if (*atual_idx == alvo_idx) {
                return p->info.atomo;
            }
            (*atual_idx)++;
        }
        p = p->prox;
    }
    return NULL;
}

ITEM *lgen_obter_item(const LISTA_GEN *l, int index) {
    if (!l || index < 0) return NULL;
    int atual_idx = 0;
    return obter_atomo_por_indice_nos(l->inicio, &atual_idx, index);
}