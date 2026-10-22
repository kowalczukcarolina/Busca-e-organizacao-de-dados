#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tads/item.h"

// Implementação da struct


struct item_st {
    ReaderType type;
    AsteroidData asteroide;
};

// Dataset Asteroides NASA
ITEM *item_criar_asteroide(AsteroidData *data) {
    if (!data) return NULL;
    ITEM *item = (ITEM *) malloc(sizeof(ITEM));
    if (item) {
        item->type = READER_NASA;
        item->asteroide = *data;
    }
    return item;
}

void item_destruir(ITEM **item) {
    if (item && *item) {
        free(*item);
        *item = NULL;
    }
}


// Retorna o tipo de leitor associado (reader)
ReaderType item_get_type(const ITEM *item) {
    return item ? item->type : READER_NASA;
}

// Retorna campos numéricos com base no nome da coluna solicitada na consulta
double item_obter_valor_numerico(const ITEM *item, const char *campo) {
    if (!item) return 0.0;
    const AsteroidData *a = &item->asteroide;

    if (strcmp(campo, "distance_au") == 0) return a->distance_au;
    if (strcmp(campo, "velocity_km_s") == 0) return a->velocity_km_s;
    if (strcmp(campo, "absolute_magnitude") == 0) return a->absolute_magnitude;
    if (strcmp(campo, "year") == 0) return (double)a->year;
    if (strcmp(campo, "month") == 0) return (double)a->month;
    if (strcmp(campo, "days_until_approach") == 0) return (double)a->days_until_approach;
    if (strcmp(campo, "risk_score") == 0) return a->risk_score;
    if (strcmp(campo, "panic_level") == 0) return a->panic_level;
    if (strcmp(campo, "sentry_impact_prob") == 0) return a->sentry_impact_prob;
    if (strcmp(campo, "sentry_torino_scale") == 0) return a->sentry_torino_scale;
    if (strcmp(campo, "sentry_palermo_scale") == 0) return a->sentry_palermo_scale;
    if (strcmp(campo, "sentry_diameter_km") == 0) return a->sentry_diameter_km;

    return 0.0;
}

// Retorna campos textuais com base no nome da coluna solicitada
const char *item_obter_valor_string(const ITEM *item, const char *campo) {
    if (!item) return "";
    const AsteroidData *a = &item->asteroide;

    if (strcmp(campo, "asteroid_designation") == 0) return a->asteroid_designation;
    if (strcmp(campo, "asteroid_fullname") == 0) return a->asteroid_fullname;
    if (strcmp(campo, "close_approach_date") == 0) return a->close_approach_date;
    if (strcmp(campo, "threat_category") == 0) return a->threat_category;
    if (strcmp(campo, "panic_verdict") == 0) return a->panic_verdict;

    return "";
}

// Funções de chave padronizadas para ordenação e busca
int item_chave(const ITEM *item) {
    if (!item) return 0;
    return (int)item_obter_valor_numerico(item, "year");
}

int item_get_chave_ano(const ITEM *item) {
    return item_chave(item);
}

int item_comparar(const ITEM *a, const ITEM *b, const char *campo_chave) {
    if (!a || !b) return 0;
    double val_a = item_obter_valor_numerico(a, campo_chave);
    double val_b = item_obter_valor_numerico(b, campo_chave);

    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;

    return strcmp(item_obter_valor_string(a, campo_chave),
                  item_obter_valor_string(b, campo_chave));
}

// Funções de impressão unificadas
void item_imprimir_linha_csv(const ITEM *item, FILE *stream) {
    if (!item || !stream) return;
    const AsteroidData *a = &item->asteroide;

    fprintf(stream, "%s;%s;%s;%d;%d;%.6f;%.2f;%.2f;%d;%d;%d;%.4f;%.4f;%s;%s;%d;%.6f;%.2f;%.2f;%.3f\n",
            a->asteroid_designation,
            a->asteroid_fullname,
            a->close_approach_date,
            a->year,
            a->month,
            a->distance_au,
            a->velocity_km_s,
            a->absolute_magnitude,
            a->days_until_approach,
            a->is_past_event ? 1 : 0,
            a->is_future_event ? 1 : 0,
            a->risk_score,
            a->panic_level,
            a->threat_category,
            a->panic_verdict,
            a->on_sentry_list ? 1 : 0,
            a->sentry_impact_prob,
            a->sentry_torino_scale,
            a->sentry_palermo_scale,
            a->sentry_diameter_km);
}

void item_imprimir(const ITEM *item) {
    item_imprimir_linha_csv(item, stdout);
}

// Retorna a chave textual padrão (asteroid_designation)
const char *item_get_chave(const ITEM *item) {
    if (!item) return "";
    return item_obter_valor_string(item, "asteroid_designation");
}