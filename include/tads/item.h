#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    READER_NASA,
    READER_CHAMADOS
} ReaderType;

// Struct para os dados de asteroides (Parte 1)
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

// Struct para os dados de chamados de TI (Parte 2)
typedef struct {
    int solicitacao;               // ID do chamado
    char atividade[256];           // Descrição da atividade
    char data_hora_abertura[32];   // Data e hora de abertura
    char sla[32];                  // SLA estipulado
    char data_hora_fechamento[32]; // Data e hora de fechamento
    char grupo[128];               // Grupo responsável
    char situacao[64];             // Situação do chamado
    char unidade[128];             // Ministério/Unidade solicitante
    int ano_abertura;              // Ano extraído para fins de agrupamento/cruzamento
} ChamadoData;

typedef struct item_st ITEM;

typedef bool (*ITEM_VISITOR)(ITEM *item, void *ctx);

// Construtores específicos
ITEM *item_criar_asteroide(const AsteroidData *data, const char *linha_bruta);
ITEM *item_criar_chamado(const ChamadoData *data, const char *linha_bruta);

void item_destruir(ITEM **item);
ReaderType item_get_type(const ITEM *item);
const char *item_linha_bruta(const ITEM *item);

// Métodos genéricos de acesso a atributos para FILTER, SIM, ORDER, GROUP, CROSS
double item_obter_valor_numerico(const ITEM *item, const char *campo);
bool item_atribuir_valor_numerico(ITEM *item, const char *campo, double valor);
const char *item_obter_valor_string(const ITEM *item, const char *campo);
bool item_obter_valor_texto(const ITEM *item, const char *campo, char *buf, size_t tam);
bool item_campo_valido(const char *campo);
// true se o campo é textual (comparado como string em FILTER/ORDER); false se numérico
bool item_campo_eh_texto(const ITEM *item, const char *campo);
// "DD/MM/AAAA HH:MM" (ou "AAAA-MM-DD HH:MM") -> AAAAMMDDHHMM; 0 se não for data
double item_data_para_chave(const char *s);
// Copia src em dest convertendo cp1252 -> UTF-8 quando src não for UTF-8 válido
void item_normalizar_utf8(const char *src, char *dest, size_t tam);
int item_comparar(const ITEM *a, const ITEM *b, const char *campo_chave);
const char *item_get_chave(const ITEM *item);

void item_imprimir_linha_csv(const ITEM *item, FILE *stream);
void item_imprimir(const ITEM *item);

#endif