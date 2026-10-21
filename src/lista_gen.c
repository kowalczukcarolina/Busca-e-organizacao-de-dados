#include <stdio.h>
#include <stdlib.h>
#include "./include/tads/lista_gen.h"

//Cria uma lista generalizada vazia
LISTA *lgen_criar(void) {
    LISTA *l = (LISTA *) malloc(sizeof(LISTA));//alocando memória
    if (l) {
        l->inicio = NULL;//lista começa vazia
        l->profundidade = 0;//lista começa sem profundidade
    }
    return l;
}

//Função auxiliar recursiva para destruir os nós de uma sub-lista
static void destruir_nos(NO *no) {
    while (no != NULL) {
        NO *prox = no->prox;

        //percorrendo os nós através do prox
        //0: átomo
        //1: sub-lista
        if (no->tipo == 1) {//
            //Se for sub-lista, destrói recursivamente os nós dela
            destruir_nos(no->info.sublista);
        } else {
            //Se for átomo, destrói o ITEM utilizando o TAD Item
            item_destruir(&(no->info.atomo));
        }
        free(no);
        no = prox;
    }
}

//Destrói toda a lista generalizada e liberta a memória
void lgen_destruir(LISTA **l) {
    if (!l || !*l) return;
    destruir_nos((*l)->inicio);
    free(*l);
    *l = NULL;
}

//Função auxiliar recursiva para imprimir os nós
static void imprimir_nos(NO *no) {
    printf("(");
    NO *p = no;
    while (p != NULL) {
        if (p->tipo == 1) {
            imprimir_nos(p->info.sublista); //Chamada recursiva para sub-lista
        } else {
            //Imprime o valor do átomo (adaptado para a designação do asteroide do nosso projeto)
            printf("%s", item_obter_valor_string(p->info.atomo, "asteroid_designation"));
        }
        
        if (p->prox != NULL) {
            printf(", ");
        }
        p = p->prox;
    }
    printf(")");
}

// Sub-rotina para imprimir a lista generalizada (Exercício do Slide 15)[cite: 15]
void lgen_imprimir(const LISTA *l) {
    if (!l) {
        printf("()\n");
        return;
    }
    imprimir_nos(l->inicio);
    printf("\n");
}

//Função auxiliar para calcular a profundidade recursivamente
static int calcular_profundidade_nos(NO *no) {
    if (no == NULL) return 0;
    
    int max_prof = 0;
    NO *p = no;
    while (p != NULL) {
        int prof_atual = 0;
        if (p->tipo == 1) {
            prof_atual = 1 + calcular_profundidade_nos(p->info.sublista);
        } else {
            prof_atual = 1; //Um átomo no nível atual conta como profundidade 1 em relação a si
        }
        if (prof_atual > max_prof) {
            max_prof = prof_atual;
        }
        p = p->prox;
    }
    return max_prof;
}

// Sub-rotina para determinar a profundidade máxima (Exercício do Slide 19)[cite: 19]
int lgen_obter_profundidade(const LISTA *l) {
    if (!l || !l->inicio) return 0;
    return calcular_profundidade_nos(l->inicio);
}