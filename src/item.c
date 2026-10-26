#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tads/item.h"

struct item_st {
    ReaderType type;
    union {
        AsteroidData asteroide;
        ChamadoData chamado;
    } dados;
    double similarity;
    char *linha_bruta;
};

static char *copiar_texto(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *c = malloc(n + 1);
    if (c) memcpy(c, s, n + 1);
    return c;
}

ITEM *item_criar_asteroide(const AsteroidData *data, const char *linha_bruta) {
    if (!data) return NULL;
    ITEM *item = malloc(sizeof(ITEM));
    if (!item) return NULL;

    item->type = READER_NASA;
    item->dados.asteroide = *data;
    item->similarity = 1.0;
    item->linha_bruta = copiar_texto(linha_bruta);
    if (!item->linha_bruta) { free(item); return NULL; }
    return item;
}

ITEM *item_criar_chamado(const ChamadoData *data, const char *linha_bruta) {
    if (!data) return NULL;
    ITEM *item = malloc(sizeof(ITEM));
    if (!item) return NULL;

    item->type = READER_CHAMADOS;
    item->dados.chamado = *data;
    item->similarity = 1.0;
    item->linha_bruta = copiar_texto(linha_bruta);
    if (!item->linha_bruta) { free(item); return NULL; }
    return item;
}

void item_destruir(ITEM **item) {
    if (item && *item) {
        free((*item)->linha_bruta);
        free(*item);
        *item = NULL;
    }
}

ReaderType item_get_type(const ITEM *item) {
    return item ? item->type : READER_NASA;
}

const char *item_linha_bruta(const ITEM *item) {
    return item ? item->linha_bruta : "";
}

double item_obter_valor_numerico(const ITEM *item, const char *campo) {
    if (!item || !campo) return 0.0;
    if (strcmp(campo, "similarity") == 0) return item->similarity;

    if (item->type == READER_NASA) {
        const AsteroidData *a = &item->dados.asteroide;
        if (strcmp(campo, "distance_au") == 0) return a->distance_au;
        if (strcmp(campo, "velocity_km_s") == 0) return a->velocity_km_s;
        if (strcmp(campo, "absolute_magnitude") == 0) return a->absolute_magnitude;
        if (strcmp(campo, "year") == 0) return (double)a->year;
        if (strcmp(campo, "month") == 0) return (double)a->month;
        if (strcmp(campo, "days_until_approach") == 0) return (double)a->days_until_approach;
        if (strcmp(campo, "is_past_event") == 0) return a->is_past_event ? 1.0 : 0.0;
        if (strcmp(campo, "is_future_event") == 0) return a->is_future_event ? 1.0 : 0.0;
        if (strcmp(campo, "risk_score") == 0) return a->risk_score;
        if (strcmp(campo, "panic_level") == 0) return a->panic_level;
    } else {
        const ChamadoData *c = &item->dados.chamado;
        if (strcmp(campo, "solicitacao") == 0) return (double)c->solicitacao;
        if (strcmp(campo, "ano_abertura") == 0) return (double)c->ano_abertura;
    }
    return 0.0;
}

bool item_atribuir_valor_numerico(ITEM *item, const char *campo, double valor) {
    if (!item || !campo) return false;
    if (strcmp(campo, "similarity") == 0) {
        item->similarity = valor;
        return true;
    }
    return false;
}

const char *item_obter_valor_string(const ITEM *item, const char *campo) {
    if (!item || !campo) return "";

    if (item->type == READER_NASA) {
        const AsteroidData *a = &item->dados.asteroide;
        if (strcmp(campo, "asteroid_designation") == 0) return a->asteroid_designation;
        if (strcmp(campo, "asteroid_fullname") == 0) return a->asteroid_fullname;
        if (strcmp(campo, "close_approach_date") == 0) return a->close_approach_date;
        if (strcmp(campo, "threat_category") == 0) return a->threat_category;
        if (strcmp(campo, "panic_verdict") == 0) return a->panic_verdict;
    } else {
        const ChamadoData *c = &item->dados.chamado;
        if (strcmp(campo, "solicitacao") == 0) { static char buf[32]; snprintf(buf, sizeof(buf), "%d", c->solicitacao); return buf; }
        if (strcmp(campo, "atividade") == 0) return c->atividade;
        if (strcmp(campo, "data_hora_abertura") == 0) return c->data_hora_abertura;
        if (strcmp(campo, "sla") == 0) return c->sla;
        if (strcmp(campo, "data_hora_fechamento") == 0) return c->data_hora_fechamento;
        if (strcmp(campo, "grupo") == 0) return c->grupo;
        if (strcmp(campo, "situacao") == 0) return c->situacao;
        if (strcmp(campo, "unidade") == 0) return c->unidade;
    }
    return "";
}

bool item_obter_valor_texto(const ITEM *item, const char *campo, char *buf, size_t tam) {
    if (!buf || tam == 0) return false;
    buf[0] = '\0';
    if (!item || !campo) return false;

    const char *str = item_obter_valor_string(item, campo);
    if (str && str[0] != '\0') {
        snprintf(buf, tam, "%s", str);
        return true;
    }
    snprintf(buf, tam, "%.15g", item_obter_valor_numerico(item, campo));
    return true;
}

int item_comparar(const ITEM *a, const ITEM *b, const char *campo_chave) {
    if (!a || !b) return 0;
    double val_a = item_obter_valor_numerico(a, campo_chave);
    double val_b = item_obter_valor_numerico(b, campo_chave);

    if (val_a != 0.0 || val_b != 0.0 || strcmp(campo_chave, "similarity") == 0 || strcmp(campo_chave, "solicitacao") == 0) {
        if (val_a < val_b) return -1;
        if (val_a > val_b) return 1;
    }
    return strcmp(item_obter_valor_string(a, campo_chave), item_obter_valor_string(b, campo_chave));
}

void item_imprimir_linha_csv(const ITEM *item, FILE *stream) {
    if (!item || !stream) return;
    fprintf(stream, "%s\n", item->linha_bruta ? item->linha_bruta : "");
}

void item_imprimir(const ITEM *item) {
    item_imprimir_linha_csv(item, stdout);
}

const char *item_get_chave(const ITEM *item) {
    if (!item) return "";
    if (item->type == READER_NASA) return item->dados.asteroide.asteroid_designation;
    static char buf[32];
    snprintf(buf, sizeof(buf), "%d", item->dados.chamado.solicitacao);
    return buf;
}