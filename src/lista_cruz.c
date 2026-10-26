#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tads/lista_cruz.h"

/*
 * Lista cruzada: cada item possui UM único nó, ligado simultaneamente a uma lista de linha
 * (ex.: threat_category) e a uma lista de coluna (ex.: year). Sem duplicar o registro.
 */
struct no_cruzado {
    ITEM *item;           // ponteiro para o elemento original (compartilhado)
    NO_CRUZ *prox_linha;  // próximo elemento na mesma linha
    NO_CRUZ *prox_coluna; // próximo elemento na mesma coluna
};

struct cabeca_eixo {
    char *chave;            // nome da categoria (linha) ou ano (coluna)
    NO_CRUZ *inicio;        // primeiro nó do grupo
    NO_CRUZ *fim;           // último nó do grupo (inserção O(1), mantém a ordem de chegada)
    CABECA_EIXO *prox_eixo; // próximo grupo do mesmo eixo
};

struct lista_cruzada {
    CABECA_EIXO *linhas;  // lista de eixos de linhas
    CABECA_EIXO *colunas; // lista de eixos de colunas
    int total_itens;      // quantidade total de nós cruzados
};

static CABECA_EIXO *criar_cabeca_eixo(const char *chave) {
    CABECA_EIXO *eixo = malloc(sizeof(CABECA_EIXO));
    if (!eixo) return NULL;

    eixo->chave = malloc(strlen(chave) + 1);
    if (!eixo->chave) {
        free(eixo);
        return NULL;
    }

    strcpy(eixo->chave, chave);
    eixo->inicio = NULL;
    eixo->fim = NULL;
    eixo->prox_eixo = NULL;
    return eixo;
}

static CABECA_EIXO *obter_ou_criar_eixo(CABECA_EIXO **lista_eixos, const char *chave) {
    if (!lista_eixos || !chave) return NULL;

    CABECA_EIXO *atual = *lista_eixos;
    CABECA_EIXO *anterior = NULL;

    while (atual != NULL) {
        if (strcmp(atual->chave, chave) == 0)
            return atual;
        anterior = atual;
        atual = atual->prox_eixo;
    }

    CABECA_EIXO *novo_eixo = criar_cabeca_eixo(chave);
    if (!novo_eixo) return NULL;

    if (anterior == NULL)
        *lista_eixos = novo_eixo;
    else
        anterior->prox_eixo = novo_eixo;

    return novo_eixo;
}

LISTA_CRUZADA *lcruz_criar(void) {
    LISTA_CRUZADA *lc = malloc(sizeof(LISTA_CRUZADA));
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

    NO_CRUZ *novo_no = malloc(sizeof(NO_CRUZ));
    if (!novo_no) return 0;

    novo_no->item = item;
    novo_no->prox_linha = NULL;
    novo_no->prox_coluna = NULL;

    // Inserção no fim da linha
    if (eixo_linha->fim) eixo_linha->fim->prox_linha = novo_no;
    else eixo_linha->inicio = novo_no;
    eixo_linha->fim = novo_no;

    // Inserção no fim da coluna
    if (eixo_coluna->fim) eixo_coluna->fim->prox_coluna = novo_no;
    else eixo_coluna->inicio = novo_no;
    eixo_coluna->fim = novo_no;

    lc->total_itens++;
    return 1;
}

void lcruz_imprimir_por_linhas(const LISTA_CRUZADA *lc) {
    if (!lc || !lc->linhas) {
        printf("Lista cruzada vazia.\n");
        return;
    }

    printf("=== IMPRESSÃO POR LINHAS ===\n");
    for (const CABECA_EIXO *e = lc->linhas; e != NULL; e = e->prox_eixo) {
        printf("\n[ Linha: %s ]\n", e->chave);
        for (const NO_CRUZ *n = e->inicio; n != NULL; n = n->prox_linha) {
            printf("  -> ");
            item_imprimir(n->item);
        }
    }
}

void lcruz_imprimir_por_colunas(const LISTA_CRUZADA *lc) {
    if (!lc || !lc->colunas) {
        printf("Lista cruzada vazia.\n");
        return;
    }

    printf("=== IMPRESSÃO POR COLUNAS ===\n");
    for (const CABECA_EIXO *e = lc->colunas; e != NULL; e = e->prox_eixo) {
        printf("\n[ Coluna: %s ]\n", e->chave);
        for (const NO_CRUZ *n = e->inicio; n != NULL; n = n->prox_coluna) {
            printf("  -> ");
            item_imprimir(n->item);
        }
    }
}

void lcruz_destruir(LISTA_CRUZADA **lc, bool destruir_itens) {
    if (!lc || !*lc) return;

    LISTA_CRUZADA *l = *lc;

    // Cada nó pertence a exatamente uma linha: liberar pelas linhas libera cada nó (e item) uma vez
    CABECA_EIXO *e_linha = l->linhas;
    while (e_linha != NULL) {
        NO_CRUZ *no_atual = e_linha->inicio;
        while (no_atual != NULL) {
            NO_CRUZ *aux = no_atual;
            no_atual = no_atual->prox_linha;
            if (destruir_itens)
                item_destruir(&aux->item);
            free(aux);
        }
        CABECA_EIXO *e_linha_aux = e_linha;
        e_linha = e_linha->prox_eixo;
        free(e_linha_aux->chave);
        free(e_linha_aux);
    }

    // Os nós das colunas já foram liberados acima: só as cabeças restam
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

int lcruz_tamanho(const LISTA_CRUZADA *lc) {
    return lc != NULL ? lc->total_itens : 0;
}

// Percurso pelas linhas (agrupado por categoria)
void lcruz_percorrer(const LISTA_CRUZADA *lc, ITEM_VISITOR f, void *ctx) {
    if (!lc || !f) return;

    for (const CABECA_EIXO *e = lc->linhas; e != NULL; e = e->prox_eixo) {
        for (const NO_CRUZ *n = e->inicio; n != NULL; n = n->prox_linha) {
            if (!f(n->item, ctx))
                return;
        }
    }
}

// Percurso pelas colunas (agrupado por ano)
void lcruz_percorrer_colunas(const LISTA_CRUZADA *lc, ITEM_VISITOR f, void *ctx) {
    if (!lc || !f) return;

    for (const CABECA_EIXO *e = lc->colunas; e != NULL; e = e->prox_eixo) {
        for (const NO_CRUZ *n = e->inicio; n != NULL; n = n->prox_coluna) {
            if (!f(n->item, ctx))
                return;
        }
    }
}
