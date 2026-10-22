#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tads/item.h"
#include "../include/tads/pilha.h"
#include "../include/tads/fila.h"
#include "../include/tads/lista_seq.h"
#include "../include/tads/lista_enc.h"
#include "../include/tads/lista_cab.h"
#include "../include/tads/lista_gen.h"
#include "../include/tads/lista_cruz.h"
#include "../include/reader.h"

bool carregar_base_nasa(const char *caminho_arquivo, void *estrutura_base, const char *tipo_base, int limite) {
    FILE *file = fopen(caminho_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o ficheiro de entrada");
        return false;
    }

    char linha[1024];
    
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

        AsteroidData data;
        memset(&data, 0, sizeof(AsteroidData));

        int campos_lidos = sscanf(linha, "%49[^;];%99[^;];%19[^;];%d;%d;%lf;%lf;%lf;%d;%d;%d;%lf;%lf;%49[^;];%49[^;];%d;%lf;%lf;%lf;%lf",
            data.asteroid_designation,
            data.asteroid_fullname,
            data.close_approach_date,
            &data.year,
            &data.month,
            &data.distance_au,
            &data.velocity_km_s,
            &data.absolute_magnitude,
            &data.days_until_approach,
            &data.is_past_event,
            &data.is_future_event,
            &data.risk_score,
            &data.panic_level,
            data.threat_category,
            data.panic_verdict,
            &data.on_sentry_list,
            &data.sentry_impact_prob,
            &data.sentry_torino_scale,
            &data.sentry_palermo_scale,
            &data.sentry_diameter_km
        );

        if (campos_lidos < 5) {
            continue; 
        }

        ITEM *novo_item = item_criar_asteroide(&data);
        if (!novo_item) continue;

        if (strcmp(tipo_base, "pilha") == 0) {
            pilha_empilhar((PILHA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "fila") == 0) {
            fila_inserir((FILA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_seq") == 0 || strcmp(tipo_base, "lista_ord") == 0) {
            lista_inserir_seq((LISTA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_enc") == 0) {
            lista_inserir_enc((LISTA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_cabeca") == 0) {
            lista_inserir_cab((LISTA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_cruz") == 0) {
            const char *chave_linha = item_obter_valor_string(novo_item, "threat_category");
            char chave_coluna_str[20];
            snprintf(chave_coluna_str, sizeof(chave_coluna_str), "%d", (int)item_obter_valor_numerico(novo_item, "year"));
            lcruz_inserir((LISTA_CRUZADA *)estrutura_base, novo_item, chave_linha, chave_coluna_str);
        } else if (strcmp(tipo_base, "lista_gen") == 0) {
            // Tratamento posterior via comandos GROUP
        }

        registros_lidos++;
    }

    fclose(file);
    return true;
}


/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tads/item.h"
#include "../include/tads/pilha.h"
#include "../include/tads/fila.h"
#include "../include/tads/lista_seq.h"
#include "../include/tads/lista_enc.h"
#include "../include/tads/lista_cab.h"
#include "../include/tads/lista_gen.h"
#include "../include/tads/lista_cruz.h"
#include "../include/reader.h"

bool carregar_base_nasa(const char *caminho_arquivo, void *estrutura_base, const char *tipo_base, int limite) {
    FILE *file = fopen(caminho_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o ficheiro de entrada");
        return false;
    }

    char linha[1024];
    
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

        AsteroidData data;
        memset(&data, 0, sizeof(AsteroidData));

        int campos_lidos = sscanf(linha, "%49[^;];%99[^;];%19[^;];%d;%d;%lf;%lf;%lf;%d;%d;%d;%lf;%lf;%49[^;];%49[^;];%d;%lf;%lf;%lf;%lf",
            data.asteroid_designation,
            data.asteroid_fullname,
            data.close_approach_date,
            &data.year,
            &data.month,
            &data.distance_au,
            &data.velocity_km_s,
            &data.absolute_magnitude,
            &data.days_until_approach,
            &data.is_past_event,
            &data.is_future_event,
            &data.risk_score,
            &data.panic_level,
            data.threat_category,
            data.panic_verdict,
            &data.on_sentry_list,
            &data.sentry_impact_prob,
            &data.sentry_torino_scale,
            &data.sentry_palermo_scale,
            &data.sentry_diameter_km
        );

        if (campos_lidos < 5) {
            continue; 
        }

        ITEM *novo_item = item_criar_asteroide(&data);
        if (!novo_item) continue;

        // Inserção polimórfica / condicional baseada na string --base
        if (strcmp(tipo_base, "pilha") == 0) {
            pilha_empilhar((PILHA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "fila") == 0) {
            fila_inserir((FILA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_seq") == 0 || strcmp(tipo_base, "lista_ord") == 0) {
            // Tanto a lista sequencial comum quanto a ordenada partilham da mesma TAD, mudando apenas a flag interna
            lista_inserir_seq((LISTA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_enc") == 0) {
            lista_inserir_enc((LISTA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_cabeca") == 0) {
            lista_inserir_cab((LISTA *)estrutura_base, novo_item);
        } else if (strcmp(tipo_base, "lista_cruz") == 0) {
            const char *chave_linha = item_obter_valor_string(novo_item, "threat_category");
            char chave_coluna_str[20];
            snprintf(chave_coluna_str, sizeof(chave_coluna_str), "%d", (int)item_obter_valor_numerico(novo_item, "year"));
            lcruz_inserir((LISTA_CRUZADA *)estrutura_base, novo_item, chave_linha, chave_coluna_str);
        } else if (strcmp(tipo_base, "lista_gen") == 0) {
            // A lista generalizada será preenchida posteriormente através dos comandos GROUP do ficheiro de consulta
            // Caso deseje inserir diretamente na base sem grupos definidos, pode tratar aqui.
        }

        registros_lidos++;
    }

    fclose(file);
    return true;
}*/