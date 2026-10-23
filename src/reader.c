#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/reader.h"
#include "../include/tads/item.h"
#include "../include/tads/pilha.h"
#include "../include/tads/fila.h"
#include "../include/tads/lista_seq.h"
#include "../include/tads/lista_enc.h"
#include "../include/tads/lista_cab.h"
#include "../include/tads/lista_gen.h"
#include "../include/tads/lista_cruz.h"

// Função auxiliar para extrair o próximo campo CSV, respeitando aspas duplas e vírgulas internas
static char *extrair_campo_csv(char **linha_ptr) {
    if (!linha_ptr || !*linha_ptr || **linha_ptr == '\0') return NULL;

    char *p = *linha_ptr;
    char *inicio = p;
    bool dentro_aspas = false;

    while (*p != '\0') {
        if (*p == '"') {
            dentro_aspas = !dentro_aspas;
        } else if (*p == ',' && !dentro_aspas) {
            *p = '\0';
            *linha_ptr = p + 1;
            // Remover aspas externas se existirem
            if (*inicio == '"') {
                inicio++;
                size_t len = strlen(inicio);
                if (len > 0 && inicio[len - 1] == '"') {
                    inicio[len - 1] = '\0';
                }
            }
            return inicio;
        }
        p++;
    }
    *linha_ptr = NULL;
    if (*inicio == '"') {
        inicio++;
        size_t len = strlen(inicio);
        if (len > 0 && inicio[len - 1] == '"') {
            inicio[len - 1] = '\0';
        }
    }
    return inicio;
}

bool carregar_base_nasa(const char *caminho_arquivo, void *estrutura_base, const char *tipo_base, int limite) {
    FILE *file = fopen(caminho_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o ficheiro de entrada");
        return false;
    }

    char linha[2048];
    
    // Ignorar a primeira linha (cabeçalho do CSV)
    if (!fgets(linha, sizeof(linha), file)) {
        fclose(file);
        return false;
    }

    int registros_lidos = 0;

    while (fgets(linha, sizeof(linha), file) != NULL) {
        if (limite > 0 && registros_lidos >= limite) {
            break;
        }

        linha[strcspn(linha, "\r\n")] = 0;
        if (strlen(linha) == 0) continue;

        char *ptr = linha;

        char *f_desig = extrair_campo_csv(&ptr);
        char *f_fullname = extrair_campo_csv(&ptr);
        char *f_date = extrair_campo_csv(&ptr);
        char *f_year = extrair_campo_csv(&ptr);
        char *f_month = extrair_campo_csv(&ptr);
        char *f_dist = extrair_campo_csv(&ptr);
        char *f_vel = extrair_campo_csv(&ptr);
        char *f_mag = extrair_campo_csv(&ptr);
        char *f_days = extrair_campo_csv(&ptr);
        char *f_past = extrair_campo_csv(&ptr);
        char *f_future = extrair_campo_csv(&ptr);
        char *f_risk = extrair_campo_csv(&ptr);
        char *f_panic_lvl = extrair_campo_csv(&ptr);
        char *f_threat = extrair_campo_csv(&ptr);
        char *f_verdict = extrair_campo_csv(&ptr);
        char *f_sentry = extrair_campo_csv(&ptr);
        char *f_prob = extrair_campo_csv(&ptr);
        char *f_torino = extrair_campo_csv(&ptr);
        char *f_palermo = extrair_campo_csv(&ptr);
        char *f_diam = extrair_campo_csv(&ptr);

        if (!f_desig) continue;

        AsteroidData data;
        memset(&data, 0, sizeof(AsteroidData));

        strncpy(data.asteroid_designation, f_desig ? f_desig : "", sizeof(data.asteroid_designation) - 1);
        strncpy(data.asteroid_fullname, f_fullname ? f_fullname : "", sizeof(data.asteroid_fullname) - 1);
        strncpy(data.close_approach_date, f_date ? f_date : "", sizeof(data.close_approach_date) - 1);
        
        data.year = f_year ? atoi(f_year) : 0;
        data.month = f_month ? atoi(f_month) : 0;
        data.distance_au = f_dist ? atof(f_dist) : 0.0;
        data.velocity_km_s = f_vel ? atof(f_vel) : 0.0;
        data.absolute_magnitude = f_mag ? atof(f_mag) : 0.0;
        data.days_until_approach = f_days ? atoi(f_days) : 0;

        data.is_past_event = (f_past && (strcasecmp(f_past, "true") == 0 || strcmp(f_past, "1") == 0));
        data.is_future_event = (f_future && (strcasecmp(f_future, "true") == 0 || strcmp(f_future, "1") == 0));
        
        data.risk_score = f_risk ? atof(f_risk) : 0.0;
        data.panic_level = f_panic_lvl ? atof(f_panic_lvl) : 0.0;

        strncpy(data.threat_category, f_threat ? f_threat : "", sizeof(data.threat_category) - 1);
        strncpy(data.panic_verdict, f_verdict ? f_verdict : "", sizeof(data.panic_verdict) - 1);

        data.on_sentry_list = (f_sentry && (strcasecmp(f_sentry, "true") == 0 || strcmp(f_sentry, "1") == 0));
        
        data.sentry_impact_prob = (f_prob && f_prob[0] != '\0') ? atof(f_prob) : 0.0;
        data.sentry_torino_scale = (f_torino && f_torino[0] != '\0') ? atof(f_torino) : 0.0;
        data.sentry_palermo_scale = (f_palermo && f_palermo[0] != '\0') ? atof(f_palermo) : 0.0;
        data.sentry_diameter_km = (f_diam && f_diam[0] != '\0') ? atof(f_diam) : 0.0;

        ITEM *novo_item = item_criar_asteroide(&data);
        if (!novo_item) continue;

        // Inserção polimórfica baseada na string --base escolhida
        if (strcmp(tipo_base, "pilha") == 0) {
            pilha_empilhar((PILHA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "fila") == 0) {
            fila_inserir((FILA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_seq") == 0 || strcmp(tipo_base, "lista_ord") == 0) {
            lista_inserir_seq((LISTA_SEQ *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_enc") == 0) {
            lista_inserir_enc((LISTA_ENC *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_cabeca") == 0 || strcmp(tipo_base, "lista_cab") == 0) {
            lista_inserir_cab((LISTA_CAB *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_cruz") == 0) {
            const char *chave_linha = item_obter_valor_string(novo_item, "threat_category");
            char chave_coluna_str[20];
            snprintf(chave_coluna_str, sizeof(chave_coluna_str), "%d", (int)item_obter_valor_numerico(novo_item, "year"));
            lcruz_inserir((LISTA_CRUZADA *)estrutura_base, novo_item, chave_linha, chave_coluna_str);
        } else if (strcmp(tipo_base, "lista_gen") == 0) {
            // Tratamento via comandos GROUP posterior
        }

        registros_lidos++;
    }

    fclose(file);
    return true;
}