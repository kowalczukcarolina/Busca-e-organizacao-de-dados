#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../include/reader.h"
#include "../include/estrutura.h"
#include "../include/tads/item.h"

#define TAM_LINHA   8192
#define MAX_CAMPOS  64
#define N_CAMPOS    8
#define MAX_CONT    5   /* linhas extras aceitas para um campo entre aspas com quebra de linha */

static char g_cabecalho_chamados[TAM_LINHA] = "";

const char *chamados_reader_cabecalho(void) {
    return g_cabecalho_chamados;
}

static int contar_aspas(const char *s) {
    int n = 0;
    for (; *s; s++)
        if (*s == '"') n++;
    return n;
}

/* Lê um registro; junta linhas quando há aspas abertas (campo com quebra de linha). */
static bool ler_registro(FILE *f, char *buf, size_t tam) {
    if (!fgets(buf, (int)tam, f)) return false;
    if (contar_aspas(buf) % 2 == 0) return true;

    long pos = ftell(f);
    size_t len = strlen(buf);
    for (int i = 0; i < MAX_CONT && len + 1 < tam; i++) {
        if (!fgets(buf + len, (int)(tam - len), f)) break;
        len = strlen(buf);
        if (contar_aspas(buf) % 2 == 0) return true;
    }
    /* aspas solta: desiste da junção e fica só com a primeira linha */
    if (pos >= 0) {
        fseek(f, pos, SEEK_SET);
        size_t k = 0;
        while (buf[k] && buf[k] != '\n') k++;
        if (buf[k] == '\n') buf[k + 1] = '\0';
    }
    return true;
}

static char *limpar_campo(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    if (n >= 2 && s[0] == '"' && s[n - 1] == '"') {
        s[n - 1] = '\0';
        s++;
        while (*s && isspace((unsigned char)*s)) s++;
        n = strlen(s);
        while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    }
    return s;
}

/* Divide por delim respeitando aspas e PRESERVANDO campos vazios (strtok os engolia). */
static int dividir_campos(char *linha, char delim, char *campos[], int max) {
    int n = 0;
    char *p = linha;
    for (;;) {
        char *ini = p;
        bool aspas = false;
        while (*p && (aspas || *p != delim)) {
            if (*p == '"') aspas = !aspas;
            p++;
        }
        if (n < max) campos[n++] = ini;
        if (*p == '\0') break;
        *p++ = '\0';
    }
    for (int i = 0; i < n; i++) campos[i] = limpar_campo(campos[i]);
    return n;
}

bool carregar_base_chamados(const char *caminho_arquivo, ESTRUTURA *base, const QUERY *q, int limite) {
    if (!base) return false;

    FILE *file = fopen(caminho_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de chamados");
        return false;
    }

    char linha[TAM_LINHA];
    if (!fgets(linha, sizeof(linha), file)) {
        fprintf(stderr, "Arquivo de entrada vazio\n");
        fclose(file);
        return false;
    }
    linha[strcspn(linha, "\r\n")] = '\0';

    char *cab = linha;
    if ((unsigned char)cab[0] == 0xEF && (unsigned char)cab[1] == 0xBB && (unsigned char)cab[2] == 0xBF)
        cab += 3;
    snprintf(g_cabecalho_chamados, sizeof(g_cabecalho_chamados), "%s", cab);

    int n_pv = 0, n_vg = 0;
    for (const char *c = g_cabecalho_chamados; *c; c++) {
        if (*c == ';') n_pv++;
        else if (*c == ',') n_vg++;
    }
    char delim = (n_vg > n_pv) ? ',' : ';';

    int registros_lidos = 0;

    while (ler_registro(file, linha, sizeof(linha))) {
        if (limite > 0 && registros_lidos >= limite) break;

        /* remove só o fim de linha final; quebras internas (campo entre aspas) permanecem no original */
        size_t len = strlen(linha);
        while (len > 0 && (linha[len - 1] == '\n' || linha[len - 1] == '\r')) linha[--len] = '\0';
        if (len == 0) continue;

        char original[TAM_LINHA];
        snprintf(original, sizeof(original), "%s", linha);

        for (char *c = linha; *c; c++)
            if (*c == '\n' || *c == '\r') *c = ' ';

        char *campos[MAX_CAMPOS];
        int n = dividir_campos(linha, delim, campos, MAX_CAMPOS);
        if (n < 1 || campos[0][0] == '\0') continue;

        /* v[0..7] = solicitacao, atividade, abertura, sla, fechamento, grupo, situacao, unidade */
        const char *v[N_CAMPOS];
        char atividade_junta[512];
        for (int i = 0; i < N_CAMPOS; i++) v[i] = "";

        if (n <= N_CAMPOS) {
            for (int i = 0; i < n; i++) v[i] = campos[i];
        } else {
            /* ';' sem aspas dentro da atividade: os 6 últimos campos são fixos */
            v[0] = campos[0];
            atividade_junta[0] = '\0';
            for (int i = 1; i <= n - 7; i++) {
                size_t usado = strlen(atividade_junta);
                snprintf(atividade_junta + usado, sizeof atividade_junta - usado, "%s%s",
                         i > 1 ? ";" : "", campos[i]);
            }
            v[1] = atividade_junta;
            for (int i = 0; i < 6; i++) v[2 + i] = campos[n - 6 + i];
        }

        ChamadoData cd;
        memset(&cd, 0, sizeof(ChamadoData));

        cd.solicitacao = atoi(v[0]);
        /* campos em UTF-8 (o CSV vem em cp1252); a linha original é mantida intacta para o --output */
        item_normalizar_utf8(v[1], cd.atividade, sizeof cd.atividade);
        item_normalizar_utf8(v[2], cd.data_hora_abertura, sizeof cd.data_hora_abertura);
        item_normalizar_utf8(v[3], cd.sla, sizeof cd.sla);
        item_normalizar_utf8(v[4], cd.data_hora_fechamento, sizeof cd.data_hora_fechamento);
        item_normalizar_utf8(v[5], cd.grupo, sizeof cd.grupo);
        item_normalizar_utf8(v[6], cd.situacao, sizeof cd.situacao);
        item_normalizar_utf8(v[7], cd.unidade, sizeof cd.unidade);
        cd.ano_abertura = 0; /* item_criar_chamado extrai o ano da data de abertura */

        ITEM *novo_item = item_criar_chamado(&cd, original);
        if (!novo_item) continue;

        if (!est_inserir(base, novo_item, q)) {
            item_destruir(&novo_item);
            continue;
        }

        registros_lidos++;
    }

    fclose(file);
    return true;
}