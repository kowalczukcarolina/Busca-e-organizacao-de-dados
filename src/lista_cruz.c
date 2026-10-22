#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tads/lista_cruz.h"

struct no_cruzado { 
    ITEM *item;           // ponteiro para o elemento original (compartilhado)
    NO_CRUZ *prox_linha;  // próximo elemento na mesma linha
    NO_CRUZ *prox_coluna; // próximo elemento na mesma coluna
};

struct cabeca_eixo {
    char *chave;            // nome da categoria (linha) ou ano (coluna)
    NO_CRUZ *inicio;        // primeiro nó do grupo
    CABECA_EIXO *prox_eixo; // próximo grupo do mesmo eixo
};

struct lista_cruzada {
    CABECA_EIXO *linhas;  // lista de eixos de linhas
    CABECA_EIXO *colunas; // lista de eixos de colunas
    int total_itens;      // quantidade total de nós cruzados
};

/*static CABECA_EIXO *criar_cabeca_eixo(const char *chave) {
    CABECA_EIXO *eixo = (CABECA_EIXO *) malloc(sizeof(CABECA_EIXO));
    if (!eixo) return NULL;

    eixo->chave = malloc(strlen(chave) + 1);
    if (!eixo->chave) {
        free(eixo);
        return NULL;
    }
    strcpy(eixo->chave, chave);
    eixo->inicio = NULL;
    eixo->prox_eixo = NULL;
    return eixo;
}*/

static CABECA_EIXO *criar_cabeca_eixo(const char *chave) {
    CABECA_EIXO *eixo = (CABECA_EIXO *) malloc(sizeof(CABECA_EIXO));
    if (!eixo) return NULL;
    
    if (!eixo->chave) {
        free(eixo);
        return NULL;
    }

    strcpy(eixo->chave, chave);
    eixo->inicio = NULL;
    eixo->prox_eixo = NULL;
    return eixo;
}

static CABECA_EIXO *obter_ou_criar_eixo(CABECA_EIXO **lista_eixos, const char *chave) {
    if (!lista_eixos || !chave) return NULL;

    CABECA_EIXO *atual = *lista_eixos;
    CABECA_EIXO *anterior = NULL;

    while (atual != NULL) {
        if (strcmp(atual->chave, chave) == 0) {
            return atual;
        }
        anterior = atual;
        atual = atual->prox_eixo;
    }

    CABECA_EIXO *novo_eixo = criar_cabeca_eixo(chave);
    if (!novo_eixo) return NULL;

    if (anterior == NULL) {
        *lista_eixos = novo_eixo;
    } else {
        anterior->prox_eixo = novo_eixo;
    }

    return novo_eixo;
}

LISTA_CRUZADA *lcruz_criar(void) {
    LISTA_CRUZADA *lc = (LISTA_CRUZADA *) malloc(sizeof(LISTA_CRUZADA));
    if (!lc) return NULL;

    lc->linhas = NULL;
    lc->colunas = NULL;
    lc->total_itens = 0;
    return lc;
}

int lcruz_inserir(LISTA_CRUZADA *lc, ITEM *item, const char *chave_linha, const char *chave_coluna) {
    if (!lc || !item || !chave_linha || !chave_coluna) return 0;

    CABECA_EIXO *eixo_linha = obter_ou_criar_eixo(&(lc->linhas), chave_linha);
    CABECA_EIXO *eixo_coluna = obter_ou_criar_eixo(&(lc->colunas), chave_coluna);

    if (!eixo_linha || !eixo_coluna) return 0;

    NO_CRUZ *novo_no = (NO_CRUZ *) malloc(sizeof(NO_CRUZ));
    if (!novo_no) return 0;

    novo_no->item = item;
    novo_no->prox_linha = NULL;
    novo_no->prox_coluna = NULL;

    // Inserção no início da linha
    novo_no->prox_linha = eixo_linha->inicio;
    eixo_linha->inicio = novo_no;

    // Inserção no início da coluna
    novo_no->prox_coluna = eixo_coluna->inicio;
    eixo_coluna->inicio = novo_no;

    lc->total_itens++;
    return 1;
}

void lcruz_imprimir_por_linhas(const LISTA_CRUZADA *lc) {
    if (!lc || !lc->linhas) {
        printf("Lista cruzada vazia.\n");
        return;
    }

    printf("=== IMPRESSÃO POR LINHAS ===\n");
    CABECA_EIXO *e_linha = lc->linhas;

    while (e_linha != NULL) {
        printf("\n[ Linha: %s ]\n", e_linha->chave);
        NO_CRUZ *no_atual = e_linha->inicio;

        while (no_atual != NULL) {
            printf("  -> ");
            item_imprimir(no_atual->item); // Usando a assinatura correta de item_imprimir(ITEM *)
            no_atual = no_atual->prox_linha;
        }
        e_linha = e_linha->prox_eixo;
    }
}

void lcruz_imprimir_por_colunas(const LISTA_CRUZADA *lc) {
    if (!lc || !lc->colunas) {
        printf("Lista cruzada vazia.\n");
        return;
    }

    printf("=== IMPRESSÃO POR COLUNAS ===\n");
    CABECA_EIXO *e_coluna = lc->colunas;

    while (e_coluna != NULL) {
        printf("\n[ Coluna: %s ]\n", e_coluna->chave);
        NO_CRUZ *no_atual = e_coluna->inicio;

        while (no_atual != NULL) {
            printf("  -> ");
            item_imprimir(no_atual->item); // Usando a assinatura correta de item_imprimir(ITEM *)
            no_atual = no_atual->prox_coluna;
        }
        e_coluna = e_coluna->prox_eixo;
    }
}

void lcruz_destruir(LISTA_CRUZADA **lc) {
    if (!lc || !*lc) return;

    LISTA_CRUZADA *l = *lc;

    // Libera os nós cruzados percorrendo pelas linhas
    CABECA_EIXO *e_linha = l->linhas;
    while (e_linha != NULL) {
        NO_CRUZ *no_atual = e_linha->inicio;
        while (no_atual != NULL) {
            NO_CRUZ *aux = no_atual;
            no_atual = no_atual->prox_linha;
            free(aux); // Os itens são gerenciados externamente, liberamos apenas os nós de cruzamento
        }
        CABECA_EIXO *e_linha_aux = e_linha;
        e_linha = e_linha->prox_eixo;
        free(e_linha_aux->chave);
        free(e_linha_aux);
    }

    // Libera os cabeçalhos das colunas
    CABECA_EIXO *e_coluna = l->colunas;
    while (e_coluna != NULL) {
        CABECA_EIXO *e_coluna_aux = e_coluna;
        e_coluna = e_coluna->prox_eixo;
        free(e_coluna_aux->chave);
        free(e_coluna_aux);
    }

    free(l);
    *lc = NULL;
}