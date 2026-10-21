#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "item.h"

//Implementação da struct
struct item_st {
    ReaderType type;
    AsteroidData asteroide;
};

//Dataset Asteroides NASA
ITEM *item_criar_asteroide(AsteroidData *data) {
    if (!data) return NULL;
    ITEM *item = (ITEM *) malloc(sizeof(ITEM)); //alocando memória
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

//Retorna o tipo de leitor associado (reader)
ReaderType item_get_type(const ITEM *item) {
    return item ? item->type : READER_NASA;
}

//Retorna campos numéricos com base no nome da coluna solicitada na consulta
double item_obter_valor_numerico(const ITEM *item, const char *campo) {
    if (!item) return 0.0;
    const AsteroidData *a = &item->asteroide; //"a" aponta diretamente para o struct do asteroide dentro do item

    //strcmp: função para comparar strings, se duas strings forem idênticas ela devolve 0
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

//Retorna campos textuais com base no nome da coluna solicitada
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

//Função  para comparar os asteroides antes de adcionar na lista ordenada
//Compara dois itens (a e b) com base num campo-chave numérico fornecido
//Retornos da função:
//Negativo: Se o primeiro item (a) for menor que o segundo (b)
//Zero: Se os dois itens forem estritamente iguais
//Positivo: Se o primeiro item (a) for maior que o segundo (b)

int item_comparar(const ITEM *a, const ITEM *b, const char *campo_chave) {
    if (!a || !b) return 0;//testando se algum deles é nulo
    double val_a = item_obter_valor_numerico(a, campo_chave);//pegando valor de a
    double val_b = item_obter_valor_numerico(b, campo_chave);//pegando valor de b

    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;

    //Desempate para comparação de strings caso os valores numéricos sejam idênticos
    return strcmp(item_obter_valor_string(a, campo_chave),
                  item_obter_valor_string(b, campo_chave));
}

//Imprime o registro no formato CSV mantendo estrutura do arquivo original
void item_imprimir_linha_csv(const ITEM *item, FILE *stream) {
    if (!item || !stream) return;
    const AsteroidData *a = &item->asteroide;

    //fprintf: envia o texto formatado para o stream escolhido em vez de ir obrigatoriamente para o terminal
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
            //condição ? valor_se_verdadeiro : valor_se_falso
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