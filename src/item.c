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
    char chave[64];   /* cópia própria: evita buffer estático compartilhado entre itens */
};

static const char *const CAMPOS_TEXTO_NASA[] = {
    "asteroid_designation", "asteroid_fullname", "close_approach_date",
    "threat_category", "panic_verdict", NULL
};
static const char *const CAMPOS_NUM_NASA[] = {
    "year", "month", "distance_au", "velocity_km_s", "absolute_magnitude",
    "days_until_approach", "is_past_event", "is_future_event", "risk_score",
    "panic_level", "on_sentry_list", "sentry_impact_prob", "sentry_torino_scale",
    "sentry_palermo_scale", "sentry_diameter_km", NULL
};
static const char *const CAMPOS_TEXTO_CHAMADOS[] = {
    "atividade", "data_hora_abertura", "sla", "data_hora_fechamento",
    "grupo", "situacao", "unidade", NULL
};
static const char *const CAMPOS_NUM_CHAMADOS[] = {
    "solicitacao", "ano_abertura", "mes_abertura", "dia_abertura", "ano_fechamento", NULL
};

static bool em_lista(const char *const *lista, const char *campo) {
    for (int i = 0; lista[i]; i++)
        if (strcmp(lista[i], campo) == 0) return true;
    return false;
}

static char *copiar_texto(const char *s) {
    if (!s) s = "";
    size_t n = strlen(s);
    char *c = malloc(n + 1);
    if (c) memcpy(c, s, n + 1);
    return c;
}

/* ---------- datas ---------- */

/* Lê "DD/MM/AAAA[ HH:MM]" ou "AAAA-MM-DD[ HH:MM]". */
static bool ler_data(const char *s, int *d, int *m, int *y, int *hh, int *mi) {
    *hh = 0; *mi = 0;
    if (!s || !s[0]) return false;
    if (sscanf(s, "%d/%d/%d %d:%d", d, m, y, hh, mi) >= 3) return true;
    *hh = 0; *mi = 0;
    if (sscanf(s, "%d-%d-%d %d:%d", y, m, d, hh, mi) >= 3) return true;
    return false;
}

double item_data_para_chave(const char *s) {
    int d, m, y, hh, mi;
    if (!ler_data(s, &d, &m, &y, &hh, &mi)) return 0.0;
    return ((((double)y * 100 + m) * 100 + d) * 100 + hh) * 100 + mi;
}

/* ---------- encoding ---------- */

static bool utf8_valido(const char *s) {
    const unsigned char *p = (const unsigned char *)s;
    while (*p) {
        int extra;
        if (*p < 0x80) extra = 0;
        else if ((*p & 0xE0) == 0xC0 && *p >= 0xC2) extra = 1;
        else if ((*p & 0xF0) == 0xE0) extra = 2;
        else if ((*p & 0xF8) == 0xF0) extra = 3;
        else return false;
        p++;
        for (int i = 0; i < extra; i++, p++)
            if ((*p & 0xC0) != 0x80) return false;
    }
    return true;
}

void item_normalizar_utf8(const char *src, char *dest, size_t tam) {
    static const unsigned short cp[32] = {
        0x20AC, 0x81, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
        0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x8D, 0x017D, 0x8F,
        0x90, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
        0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x9D, 0x017E, 0x0178
    };
    if (!dest || tam == 0) return;
    dest[0] = '\0';
    if (!src) return;
    if (utf8_valido(src)) {
        snprintf(dest, tam, "%s", src);
        return;
    }
    size_t j = 0;
    for (size_t i = 0; src[i] && j + 4 < tam; i++) {
        unsigned char c = (unsigned char)src[i];
        unsigned v = c < 0x80 ? c : (c < 0xA0 ? cp[c - 0x80] : c);
        if (v < 0x80) {
            dest[j++] = (char)v;
        } else if (v < 0x800) {
            dest[j++] = (char)(0xC0 | (v >> 6));
            dest[j++] = (char)(0x80 | (v & 0x3F));
        } else {
            dest[j++] = (char)(0xE0 | (v >> 12));
            dest[j++] = (char)(0x80 | ((v >> 6) & 0x3F));
            dest[j++] = (char)(0x80 | (v & 0x3F));
        }
    }
    dest[j] = '\0';
}

/* ---------- construtores ---------- */

ITEM *item_criar_asteroide(const AsteroidData *data, const char *linha_bruta) {
    if (!data) return NULL;
    ITEM *item = malloc(sizeof(ITEM));
    if (!item) return NULL;

    item->type = READER_NASA;
    item->dados.asteroide = *data;
    item->similarity = 1.0;
    snprintf(item->chave, sizeof item->chave, "%s", data->asteroid_designation);
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

    if (item->dados.chamado.ano_abertura == 0) {
        int d, m, y, hh, mi;
        if (ler_data(data->data_hora_abertura, &d, &m, &y, &hh, &mi))
            item->dados.chamado.ano_abertura = y;
    }
    snprintf(item->chave, sizeof item->chave, "%d", data->solicitacao);
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
    return item && item->linha_bruta ? item->linha_bruta : "";
}

/* ---------- campos ---------- */

bool item_campo_valido(const char *campo) {
    if (!campo) return false;
    return strcmp(campo, "similarity") == 0
        || em_lista(CAMPOS_TEXTO_NASA, campo) || em_lista(CAMPOS_NUM_NASA, campo)
        || em_lista(CAMPOS_TEXTO_CHAMADOS, campo) || em_lista(CAMPOS_NUM_CHAMADOS, campo);
}

bool item_campo_eh_texto(const ITEM *item, const char *campo) {
    if (!item || !campo) return false;
    return em_lista(item->type == READER_NASA ? CAMPOS_TEXTO_NASA : CAMPOS_TEXTO_CHAMADOS, campo);
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
        /* estes campos eram lidos pelo reader mas não eram acessíveis (FILTER dava sempre 0) */
        if (strcmp(campo, "on_sentry_list") == 0) return a->on_sentry_list ? 1.0 : 0.0;
        if (strcmp(campo, "sentry_impact_prob") == 0) return a->sentry_impact_prob;
        if (strcmp(campo, "sentry_torino_scale") == 0) return a->sentry_torino_scale;
        if (strcmp(campo, "sentry_palermo_scale") == 0) return a->sentry_palermo_scale;
        if (strcmp(campo, "sentry_diameter_km") == 0) return a->sentry_diameter_km;
    } else {
        const ChamadoData *c = &item->dados.chamado;
        int d, m, y, hh, mi;
        if (strcmp(campo, "solicitacao") == 0) return (double)c->solicitacao;
        if (strcmp(campo, "ano_abertura") == 0) return (double)c->ano_abertura;
        if (strcmp(campo, "mes_abertura") == 0)
            return ler_data(c->data_hora_abertura, &d, &m, &y, &hh, &mi) ? (double)m : 0.0;
        if (strcmp(campo, "dia_abertura") == 0)
            return ler_data(c->data_hora_abertura, &d, &m, &y, &hh, &mi) ? (double)d : 0.0;
        if (strcmp(campo, "ano_fechamento") == 0)
            return ler_data(c->data_hora_fechamento, &d, &m, &y, &hh, &mi) ? (double)y : 0.0;
        /* datas viram chave cronológica (usada por ORDER e SIM) */
        if (strcmp(campo, "data_hora_abertura") == 0) return item_data_para_chave(c->data_hora_abertura);
        if (strcmp(campo, "data_hora_fechamento") == 0) return item_data_para_chave(c->data_hora_fechamento);
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
        if (strcmp(campo, "solicitacao") == 0) return item->chave;
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

/* Texto do campo para GROUP/CROSS. false => chamador usa "desconhecido". */
bool item_obter_valor_texto(const ITEM *item, const char *campo, char *buf, size_t tam) {
    if (!buf || tam == 0) return false;
    buf[0] = '\0';
    if (!item || !campo || !item_campo_valido(campo)) return false;

    if (item_campo_eh_texto(item, campo)) {
        const char *str = item_obter_valor_string(item, campo);
        if (!str || str[0] == '\0') return false;
        snprintf(buf, tam, "%s", str);
        return true;
    }
    snprintf(buf, tam, "%.15g", item_obter_valor_numerico(item, campo));
    return true;
}

int item_comparar(const ITEM *a, const ITEM *b, const char *campo_chave) {
    if (!a || !b || !campo_chave) return 0;

    if (item_campo_eh_texto(a, campo_chave)) {
        /* datas de chamados: ordem cronológica; os demais textos valem 0 aqui */
        double da = item_obter_valor_numerico(a, campo_chave);
        double db = item_obter_valor_numerico(b, campo_chave);
        if (da < db) return -1;
        if (da > db) return 1;
        int c = strcmp(item_obter_valor_string(a, campo_chave), item_obter_valor_string(b, campo_chave));
        return (c > 0) - (c < 0);
    }

    double va = item_obter_valor_numerico(a, campo_chave);
    double vb = item_obter_valor_numerico(b, campo_chave);
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

void item_imprimir_linha_csv(const ITEM *item, FILE *stream) {
    if (!item || !stream) return;
    fprintf(stream, "%s\n", item->linha_bruta ? item->linha_bruta : "");
}

void item_imprimir(const ITEM *item) {
    item_imprimir_linha_csv(item, stdout);
}

const char *item_get_chave(const ITEM *item) {
    return item ? item->chave : "";
}