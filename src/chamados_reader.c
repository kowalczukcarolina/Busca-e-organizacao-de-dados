#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../include/reader.h"
#include "../include/estrutura.h"
#include "../include/tads/item.h"

#define TAM_LINHA 8192

static char g_cabecalho_chamados[TAM_LINHA] = "";

const char *chamados_reader_cabecalho(void) {
    return g_cabecalho_chamados;
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
        fclose(file);
        return false;
    }
    linha[strcspn(linha, "\r\n")] = '\0';
    
    // Remove BOM se presente
    char *cab = linha;
    if ((unsigned char)cab[0] == 0xEF && (unsigned char)cab[1] == 0xBB && (unsigned char)cab[2] == 0xBF)
        cab += 3;
    snprintf(g_cabecalho_chamados, sizeof(g_cabecalho_chamados), "%s", cab);

    int registros_lidos = 0;

    while (fgets(linha, sizeof(linha), file) != NULL) {
        if (limite > 0 && registros_lidos >= limite) break;

        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') continue;

        char original[TAM_LINHA];
        snprintf(original, sizeof(original), "%s", linha);

        // Parser simples baseado em ';' (delimitador padrão do dataset de chamados)
        char *campos[8];
        int n = 0;
        char *ptr = linha;
        char *token = strtok(ptr, ";");
        while (token != NULL && n < 8) {
            campos[n++] = token;
            token = strtok(NULL, ";");
        }

        if (n < 8) continue;

        ChamadoData cd;
        memset(&cd, 0, sizeof(ChamadoData));

        cd.solicitacao = atoi(campos[0]);
        snprintf(cd.atividade, sizeof(cd.atividade), "%s", campos[1]);
        snprintf(cd.data_hora_abertura, sizeof(cd.data_hora_abertura), "%s", campos[2]);
        snprintf(cd.sla, sizeof(cd.sla), "%s", campos[3]);
        snprintf(cd.data_hora_fechamento, sizeof(cd.data_hora_fechamento), "%s", campos[4]);
        snprintf(cd.grupo, sizeof(cd.grupo), "%s", campos[5]);
        snprintf(cd.situacao, sizeof(cd.situacao), "%s", campos[6]);
        snprintf(cd.unidade, sizeof(cd.unidade), "%s", campos[7]);

        // Extrai o ano da data de abertura (formato DD/MM/AAAA HH:MM)
        if (strlen(cd.data_hora_abertura) >= 10) {
            cd.ano_abertura = atoi(cd.data_hora_abertura + 6);
        } else {
            cd.ano_abertura = 2026;
        }

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