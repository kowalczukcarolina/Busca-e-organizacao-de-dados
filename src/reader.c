#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../include/reader.h"
#include "../include/estrutura.h"
#include "../include/tads/item.h"

#define TAM_LINHA  8192
#define MAX_COLS   64

/* Campos do dataset da NASA, na ordem em que aparecem no CSV original */
enum {
    F_DESIG, F_FULLNAME, F_DATE, F_YEAR, F_MONTH, F_DIST, F_VEL, F_MAG, F_DAYS,
    F_PAST, F_FUTURE, F_RISK, F_PANIC_LVL, F_THREAT, F_VERDICT, F_SENTRY,
    F_PROB, F_TORINO, F_PALERMO, F_DIAM, F_TOTAL
};

static const char *const NOMES_CAMPOS[F_TOTAL] = {
    "asteroid_designation", "asteroid_fullname", "close_approach_date", "year", "month",
    "distance_au", "velocity_km_s", "absolute_magnitude", "days_until_approach",
    "is_past_event", "is_future_event", "risk_score", "panic_level", "threat_category",
    "panic_verdict", "on_sentry_list", "sentry_impact_prob", "sentry_torino_scale",
    "sentry_palermo_scale", "sentry_diameter_km"
};

static char g_cabecalho[TAM_LINHA] = "";
static char g_delim = ',';

const char *reader_cabecalho(void) {
    return g_cabecalho;
}

// Extrai o próximo campo CSV respeitando aspas duplas e delimitadores internos.
// Modifica a linha (troca o delimitador por '\0').
static char *extrair_campo_csv(char **linha_ptr, char delim) {
    if (!linha_ptr || !*linha_ptr) return NULL;

    char *p = *linha_ptr;
    char *inicio = p;
    bool dentro_aspas = false;

    while (*p != '\0') {
        if (*p == '"') {
            dentro_aspas = !dentro_aspas;
        } else if (*p == delim && !dentro_aspas) {
            *p = '\0';
            *linha_ptr = p + 1;
            goto fim;
        }
        p++;
    }
    *linha_ptr = NULL; /* último campo da linha */

fim:
    // Remove aspas externas, se existirem
    if (*inicio == '"') {
        inicio++;
        size_t len = strlen(inicio);
        if (len > 0 && inicio[len - 1] == '"')
            inicio[len - 1] = '\0';
    }
    return inicio;
}

static bool eh_verdadeiro(const char *s) {
    if (!s) return false;
    if (strcmp(s, "1") == 0) return true;
    const char *t = "true";
    for (int i = 0; i < 5; i++) /* i = 4 compara o '\0' */
        if (tolower((unsigned char)s[i]) != t[i]) return false;
    return true;
}

// Converte texto em double. Campo vazio/ausente vira 0 sem interromper a leitura.
// Com delimitador ';' aceita vírgula decimal.
static double para_double(const char *s) {
    if (!s || s[0] == '\0') return 0.0;
    if (g_delim != ';') return atof(s);

    char tmp[64];
    size_t n = strlen(s);
    if (n >= sizeof tmp) n = sizeof tmp - 1;
    memcpy(tmp, s, n);
    tmp[n] = '\0';
    for (size_t i = 0; i < n; i++)
        if (tmp[i] == ',') tmp[i] = '.';
    return atof(tmp);
}

static int para_int(const char *s) {
    return (s && s[0] != '\0') ? (int)para_double(s) : 0;
}

static void copiar_campo(char *dest, size_t tam, const char *src) {
    snprintf(dest, tam, "%s", src ? src : "");
}

// Mapeia as colunas do cabeçalho para os campos conhecidos. Retorna quantos foram encontrados.
static int mapear_colunas(char *cabecalho_copia, int mapa[F_TOTAL]) {
    for (int i = 0; i < F_TOTAL; i++) mapa[i] = -1;

    int encontrados = 0;
    char *ptr = cabecalho_copia;
    for (int col = 0; ptr != NULL && col < MAX_COLS; col++) {
        char *nome = extrair_campo_csv(&ptr, g_delim);
        if (!nome) break;
        while (isspace((unsigned char)*nome)) nome++;
        for (int f = 0; f < F_TOTAL; f++) {
            if (mapa[f] < 0 && strcmp(nome, NOMES_CAMPOS[f]) == 0) {
                mapa[f] = col;
                encontrados++;
                break;
            }
        }
    }
    return encontrados;
}

bool carregar_base_nasa(const char *caminho_arquivo, ESTRUTURA *base, const QUERY *q, int limite) {
    if (!base) return false;

    FILE *file = fopen(caminho_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo de entrada");
        return false;
    }

    char linha[TAM_LINHA];

    // Primeira linha: cabeçalho. É guardado e reimpresso idêntico no --output.
    if (!fgets(linha, sizeof(linha), file)) {
        fprintf(stderr, "Arquivo de entrada vazio\n");
        fclose(file);
        return false;
    }
    linha[strcspn(linha, "\r\n")] = '\0';

    char *cab = linha;
    if ((unsigned char)cab[0] == 0xEF && (unsigned char)cab[1] == 0xBB && (unsigned char)cab[2] == 0xBF)
        cab += 3; /* ignora BOM UTF-8 */
    snprintf(g_cabecalho, sizeof g_cabecalho, "%s", cab);

    // Delimitador detectado pelo cabeçalho (o dataset tem 20 colunas)
    int n_pv = 0, n_vg = 0;
    for (const char *c = g_cabecalho; *c; c++) {
        if (*c == ';') n_pv++;
        else if (*c == ',') n_vg++;
    }
    g_delim = (n_pv > n_vg) ? ';' : ',';

    // Mapeia colunas pelo nome; se o cabeçalho não bater, assume a ordem do dataset original
    int mapa[F_TOTAL];
    char cab_copia[TAM_LINHA];
    snprintf(cab_copia, sizeof cab_copia, "%s", g_cabecalho);
    if (mapear_colunas(cab_copia, mapa) == 0) {
        for (int i = 0; i < F_TOTAL; i++) mapa[i] = i;
    }

    int registros_lidos = 0;

    while (fgets(linha, sizeof(linha), file) != NULL) {
        if (limite > 0 && registros_lidos >= limite)
            break;

        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') continue;

        // Guarda a linha original ANTES de extrair os campos (a extração a modifica)
        char original[TAM_LINHA];
        snprintf(original, sizeof original, "%s", linha);

        // Divide em colunas
        char *valores[MAX_COLS];
        int n_cols = 0;
        char *ptr = linha;
        while (ptr != NULL && n_cols < MAX_COLS) {
            valores[n_cols] = extrair_campo_csv(&ptr, g_delim);
            n_cols++;
        }

#define CAMPO(f) ((mapa[f] >= 0 && mapa[f] < n_cols) ? valores[mapa[f]] : NULL)

        const char *f_desig = CAMPO(F_DESIG);
        if (!f_desig || f_desig[0] == '\0') continue;

        AsteroidData data;
        memset(&data, 0, sizeof(AsteroidData));

        copiar_campo(data.asteroid_designation, sizeof data.asteroid_designation, f_desig);
        copiar_campo(data.asteroid_fullname, sizeof data.asteroid_fullname, CAMPO(F_FULLNAME));
        copiar_campo(data.close_approach_date, sizeof data.close_approach_date, CAMPO(F_DATE));
        copiar_campo(data.threat_category, sizeof data.threat_category, CAMPO(F_THREAT));
        copiar_campo(data.panic_verdict, sizeof data.panic_verdict, CAMPO(F_VERDICT));

        data.year = para_int(CAMPO(F_YEAR));
        data.month = para_int(CAMPO(F_MONTH));
        data.distance_au = para_double(CAMPO(F_DIST));
        data.velocity_km_s = para_double(CAMPO(F_VEL));
        data.absolute_magnitude = para_double(CAMPO(F_MAG));
        data.days_until_approach = para_int(CAMPO(F_DAYS));
        data.is_past_event = eh_verdadeiro(CAMPO(F_PAST));
        data.is_future_event = eh_verdadeiro(CAMPO(F_FUTURE));
        data.risk_score = para_double(CAMPO(F_RISK));
        data.panic_level = para_double(CAMPO(F_PANIC_LVL));
        data.on_sentry_list = eh_verdadeiro(CAMPO(F_SENTRY));
        data.sentry_impact_prob = para_double(CAMPO(F_PROB));
        data.sentry_torino_scale = para_double(CAMPO(F_TORINO));
        data.sentry_palermo_scale = para_double(CAMPO(F_PALERMO));
        data.sentry_diameter_km = para_double(CAMPO(F_DIAM));

#undef CAMPO

        ITEM *novo_item = item_criar_asteroide(&data, original);
        if (!novo_item) continue;

        // Inserção na estrutura --base (lista_gen/lista_cruz usam GROUP/CROSS da consulta)
        if (!est_inserir(base, novo_item, q)) {
            item_destruir(&novo_item);
            continue;
        }

        registros_lidos++;
    }

    fclose(file);
    return true;
}
