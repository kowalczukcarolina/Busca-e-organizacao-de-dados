#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tads/item.h"

// Implementação da struct opaca
struct item_st {
    ReaderType type;
    AsteroidData asteroide;
    double similarity;  // score de similaridade calculado pela query
    char *linha_bruta;  // linha original do CSV, reimpressa fielmente no --output
};

static char *copiar_texto(const char *s) {
    size_t n = strlen(s);
    char *c = malloc(n + 1);
    if (c) memcpy(c, s, n + 1);
    return c;
}

// Dataset Asteroides NASA
ITEM *item_criar_asteroide(const AsteroidData *data, const char *linha_bruta) {
    if (!data) return NULL;
    ITEM *item = malloc(sizeof(ITEM));
    if (!item) return NULL;

    item->type = READER_NASA;
    item->asteroide = *data;
    item->similarity = 1.0; // valor padrão
    item->linha_bruta = copiar_texto(linha_bruta ? linha_bruta : "");
    if (!item->linha_bruta) {
        free(item);
        return NULL;
    }
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

// Retorna campos numéricos (e booleanos como 0/1) com base no nome da coluna
double item_obter_valor_numerico(const ITEM *item, const char *campo) {
    if (!item || !campo) return 0.0;
    const AsteroidData *a = &item->asteroide;

    if (strcmp(campo, "similarity") == 0) return item->similarity;
    if (strcmp(campo, "distance_au") == 0) return a->distance_au;
    if (strcmp(campo, "velocity_km_s") == 0) return a->velocity_km_s;
    if (strcmp(campo, "absolute_magnitude") == 0) return a->absolute_magnitude;
    if (strcmp(campo, "year") == 0) return (double)a->year;
    if (strcmp(campo, "month") == 0) return (double)a->month;
    if (strcmp(campo, "days_until_approach") == 0) return (double)a->days_until_approach;
    if (strcmp(campo, "is_past_event") == 0) return a->is_past_event ? 1.0 : 0.0;
    if (strcmp(campo, "is_future_event") == 0) return a->is_future_event ? 1.0 : 0.0;
    if (strcmp(campo, "on_sentry_list") == 0) return a->on_sentry_list ? 1.0 : 0.0;
    if (strcmp(campo, "risk_score") == 0) return a->risk_score;
    if (strcmp(campo, "panic_level") == 0) return a->panic_level;
    if (strcmp(campo, "sentry_impact_prob") == 0) return a->sentry_impact_prob;
    if (strcmp(campo, "sentry_torino_scale") == 0) return a->sentry_torino_scale;
    if (strcmp(campo, "sentry_palermo_scale") == 0) return a->sentry_palermo_scale;
    if (strcmp(campo, "sentry_diameter_km") == 0) return a->sentry_diameter_km;

    return 0.0;
}

// Permite atribuir dinamicamente valores numéricos ao ITEM (ex: similarity)
bool item_atribuir_valor_numerico(ITEM *item, const char *campo, double valor) {
    if (!item || !campo) return false;

    if (strcmp(campo, "similarity") == 0) {
        item->similarity = valor;
        return true;
    }

    return false;
}

// Retorna campos textuais com base no nome da coluna solicitada
const char *item_obter_valor_string(const ITEM *item, const char *campo) {
    if (!item || !campo) return "";
    const AsteroidData *a = &item->asteroide;

    if (strcmp(campo, "asteroid_designation") == 0) return a->asteroid_designation;
    if (strcmp(campo, "asteroid_fullname") == 0) return a->asteroid_fullname;
    if (strcmp(campo, "close_approach_date") == 0) return a->close_approach_date;
    if (strcmp(campo, "threat_category") == 0) return a->threat_category;
    if (strcmp(campo, "panic_verdict") == 0) return a->panic_verdict;

    return "";
}

static const char *const CAMPOS_TEXTO[] = {
    "asteroid_designation", "asteroid_fullname", "close_approach_date",
    "threat_category", "panic_verdict"
};

static const char *const CAMPOS_NUM[] = {
    "year", "month", "distance_au", "velocity_km_s", "absolute_magnitude",
    "days_until_approach", "is_past_event", "is_future_event", "risk_score",
    "panic_level", "on_sentry_list", "sentry_impact_prob", "sentry_torino_scale",
    "sentry_palermo_scale", "sentry_diameter_km", "similarity"
};

static bool esta_em(const char *const *v, size_t n, const char *c) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(v[i], c) == 0) return true;
    return false;
}

#define N_TEXTO (sizeof CAMPOS_TEXTO / sizeof *CAMPOS_TEXTO)
#define N_NUM   (sizeof CAMPOS_NUM / sizeof *CAMPOS_NUM)

bool item_campo_valido(const char *campo) {
    if (!campo) return false;
    return esta_em(CAMPOS_TEXTO, N_TEXTO, campo) || esta_em(CAMPOS_NUM, N_NUM, campo);
}

// Converte qualquer campo para texto (usado por GROUP e CROSS: year vira "2025")
bool item_obter_valor_texto(const ITEM *item, const char *campo, char *buf, size_t tam) {
    if (!buf || tam == 0) return false;
    buf[0] = '\0';
    if (!item || !campo) return false;

    if (esta_em(CAMPOS_TEXTO, N_TEXTO, campo)) {
        snprintf(buf, tam, "%s", item_obter_valor_string(item, campo));
        return true;
    }
    if (esta_em(CAMPOS_NUM, N_NUM, campo)) {
        snprintf(buf, tam, "%.15g", item_obter_valor_numerico(item, campo));
        return true;
    }
    return false;
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

// Reimprime a linha exatamente como veio no CSV de entrada (mesmo delimitador e formatação)
void item_imprimir_linha_csv(const ITEM *item, FILE *stream) {
    if (!item || !stream) return;
    fprintf(stream, "%s\n", item->linha_bruta ? item->linha_bruta : "");
}

void item_imprimir(const ITEM *item) {
    item_imprimir_linha_csv(item, stdout);
}

// Retorna a chave textual padrão (asteroid_designation)
const char *item_get_chave(const ITEM *item) {
    if (!item) return "";
    return item_obter_valor_string(item, "asteroid_designation");
}
