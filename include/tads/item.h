#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// Identificador do leitor em uso (--reader)
typedef enum {
    READER_NASA,
    READER_CHAMADOS_TI
} ReaderType;

// Dataset Asteroides NASA
typedef struct {
    char asteroid_designation[64];
    char asteroid_fullname[128];
    char close_approach_date[32];
    int year;
    int month;
    double distance_au;
    double velocity_km_s;
    double absolute_magnitude;
    int days_until_approach;
    bool is_past_event;
    bool is_future_event;
    double risk_score;
    double panic_level;
    char threat_category[64];
    char panic_verdict[64];
    bool on_sentry_list;
    double sentry_impact_prob;
    double sentry_torino_scale;
    double sentry_palermo_scale;
    double sentry_diameter_km;
} AsteroidData;

// TAD ITEM

// Declaração opaca do item
typedef struct item_st ITEM;

// Callback de percurso das estruturas: devolva true para continuar, false para parar.
typedef bool (*ITEM_VISITOR)(ITEM *item, void *ctx);

// Construtores e Destrutores
// linha_bruta: linha original do CSV (sem \r\n); é reimpressa fielmente na saída.
ITEM *item_criar_asteroide(const AsteroidData *data, const char *linha_bruta);
void item_destruir(ITEM **item);

// Métodos de Acesso Genéricos (Interface do TAD)
ReaderType item_get_type(const ITEM *item);
const char *item_get_chave(const ITEM *item);
const char *item_linha_bruta(const ITEM *item);

// Métodos de Avaliação de Filtro e Similaridade
double item_obter_valor_numerico(const ITEM *item, const char *campo);
const char *item_obter_valor_string(const ITEM *item, const char *campo);
bool item_atribuir_valor_numerico(ITEM *item, const char *campo, double valor);

// true se "campo" é um campo conhecido (texto ou numérico)
bool item_campo_valido(const char *campo);
// Converte QUALQUER campo (texto ou numérico) para texto. false se o campo não existe.
bool item_obter_valor_texto(const ITEM *item, const char *campo, char *buf, size_t tam);

// Métodos de Ordenação e Impressão
int item_comparar(const ITEM *a, const ITEM *b, const char *campo_chave);
void item_imprimir_linha_csv(const ITEM *item, FILE *stream);
void item_imprimir(const ITEM *item);

#endif // ITEM_H
