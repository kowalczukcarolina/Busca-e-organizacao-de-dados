#ifndef LISTA_GEN_H
#define LISTA_GEN_H

#include "item.h"
//Implementação do professor de lista generalizada

//Declarações opacas
typedef struct no_ NO;
typedef struct lista_ LISTA;

struct lista_ {
    NO *inicio;
    int profundidade;
};


//Atómo: não contém outras listas lá dentro -> átomo é o item para o asteroide
struct no_ {
    int tipo; //0 para átomo, 1 para sub-lista
    union {
        ITEM *atomo;
        struct no_ *sublista;
    } info;
    NO *prox;
};

//Funções principais de gestão e operações pedidas
LISTA *lgen_criar(void);
void lgen_destruir(LISTA **l);
void lgen_imprimir(const LISTA *l);
int lgen_obter_profundidade(const LISTA *l);

#endif