#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../include/tads/item.h"
#include "../include/estrutura.h"
#include "../include/reader.h"
#include "../include/query.h"


//Função para medir o tempo de cada etapa (carregamento, busca e escrita)
static double agora_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

//Struct da busca
typedef struct {
    const QUERY *q; //ponteiro para consulta carregada, usada para validar se o item atual cumpre os critérios
    ESTRUTURA *res; //ponteiro para estrutura de dados secundária de resultados (--result)
    int matched; //contador para quantos itens parraram nos critérios de avaliação da consulta
    int inseridos; //contador para os itens inseridos no resultado
    double insert_ms; //tempo gasto para registrar na estrutura de dados
} CTX_BUSCA;


//Para cada item, a função visita_busca verifica se ele cumpre os critérios da consulta (query_avaliar)
static bool visita_busca(ITEM *item, void *arg) {
    CTX_BUSCA *c = arg;
    double sim = 1.0;

    //Avalia a query (filtros, similaridade, etc.) no item atual
    if (c->q && !query_avaliar(c->q, item, &sim))
        return true; //se não passar no filtro, ignora e continua

    c->matched++;
    if (c->res) {
        item_atribuir_valor_numerico(item, "similarity", sim);
        double t0 = agora_ms();
        if (est_inserir(c->res, item, c->q)) //insere na estrutura de resultado
            c->inseridos++;
        c->insert_ms += agora_ms() - t0;
    }
    return true;
}

//Função de exportação de saída
typedef struct {
    FILE *f; //ponteiro para o ficheiro aberto de escrita
    int restante; //controla limite de registros que serão exportados 
    int escritos; //contador para o total de registros finalizados
} CTX_SAIDA;


//Função para gravar a saída em CSV
static bool visita_saida(ITEM *item, void *arg) {
    CTX_SAIDA *c = arg; //
    item_imprimir_linha_csv(item, c->f); //escreve a linha formatada
    c->escritos++;
    if (c->restante > 0) c->restante--;
    return c->restante != 0; //para quando atinge o limite do --top-k
}


//Main
int main(int argc, char *argv[]) {
    const char *reader_tipo = NULL, *input_path = NULL, *tipo_base = NULL;
    const char *tipo_resultado = NULL, *query_path = NULL, *output_path = NULL;
    int top_k = 0, limit = 0;
    int status = 1;
    ESTRUTURA *base = NULL, *res = NULL;
    QUERY *query = NULL;
    double t0 = 0.0, load_ms = 0.0, search_ms = 0.0, write_ms = 0.0;
    int input_records = 0, output_records = 0;
    CTX_BUSCA cb = { NULL, NULL, 0, 0, 0.0 };
    bool carga_ok = false;

    //Laço para leitura e interpretação dos comandos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--reader") == 0 && i + 1 < argc)      reader_tipo = argv[++i];
        else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc)  input_path = argv[++i];
        else if (strcmp(argv[i], "--base") == 0 && i + 1 < argc)   tipo_base = argv[++i];
        else if (strcmp(argv[i], "--result") == 0 && i + 1 < argc) tipo_resultado = argv[++i];
        else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc)  query_path = argv[++i];
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) output_path = argv[++i];
        else if (strcmp(argv[i], "--top-k") == 0 && i + 1 < argc)  top_k = atoi(argv[++i]);
        else if (strcmp(argv[i], "--limit") == 0 && i + 1 < argc)  limit = atoi(argv[++i]);
    }

    if (!reader_tipo || !input_path || !tipo_base) {
        fprintf(stderr, "Uso: %s --reader [nasa|chamados] --input arq.csv --base [estrutura] ...\n", argv[0]);
        goto fim; //usando "goto" para forçar o programa a pular para o fim caso dê algum erro
    }


    //Inicialização das estruturas de dados e consultas
    base = est_criar(tipo_base); //criando estrutura de bade (fila, lista_seq, etc)
    if (!base) {
        fprintf(stderr, "Estrutura base invalida: %s\n", tipo_base);
        goto fim;
    }
    if (tipo_resultado) {
        res = est_criar(tipo_resultado) //criando estrutura do resultado --result; 
        if (!res) {
            fprintf(stderr, "Estrutura de resultado invalida: %s\n", tipo_resultado);
            goto fim;
        }
    }
    if (query_path) {
        query = query_carregar(query_path); //carregando regras de busca do .txt
        if (!query) goto fim;
    }

    if (res) est_configurar_ordem(res, query_order_campo(query), query_order_desc(query));


    //Carregamento da base (Dataset) conforme o reader escolhido
    t0 = agora_ms();
    if (strcmp(reader_tipo, "nasa") == 0) {
        carga_ok = carregar_base_nasa(input_path, base, query, limit);
    } else if (strcmp(reader_tipo, "chamados") == 0) {
        carga_ok = carregar_base_chamados(input_path, base, query, limit);
    } else {
        fprintf(stderr, "Leitor '%s' nao suportado.\n", reader_tipo);
        goto fim;
    }

    if (!carga_ok) {
        fprintf(stderr, "Erro durante o carregamento dos dados.\n");
        goto fim;
    }
    load_ms = agora_ms() - t0;
    input_records = est_tamanho(base);

    //Busca
    cb.q = query;
    cb.res = res;
    t0 = agora_ms();
    est_percorrer(base, visita_busca, &cb); //percorre toda a base vendo os filtros e guardando os resultados
    search_ms = agora_ms() - t0 - cb.insert_ms;
    if (search_ms < 0.0) search_ms = 0.0;

    //Escrita do output
    t0 = agora_ms();
    if (res) {
        int total = est_tamanho(res);
        output_records = (top_k > 0 && top_k < total) ? top_k : total;
        if (output_path) {
            FILE *fout = fopen(output_path, "w");
            if (!fout) {
                perror("Erro ao abrir --output");
                goto fim;
            }
            const char *cabecalho = (strcmp(reader_tipo, "nasa") == 0) ? reader_cabecalho() : chamados_reader_cabecalho();
            fprintf(fout, "%s\n", cabecalho); //escrita do cabeçalho
            

            //Percorre a estrutura de resultados para exportar as linhas
            CTX_SAIDA cs = { fout, top_k > 0 ? top_k : -1, 0 };
            est_percorrer(res, visita_saida, &cs);
            output_records = cs.escritos;
            fclose(fout);
        }
    }
    write_ms = agora_ms() - t0;

    //Impressão das métricas
    printf("STATUS=OK\n");
    printf("INPUT_RECORDS=%d\n", input_records);
    printf("MATCHED_RECORDS=%d\n", cb.matched);
    printf("OUTPUT_RECORDS=%d\n", output_records);
    printf("LOAD_MS=%.3f\n", load_ms);
    printf("SEARCH_MS=%.3f\n", search_ms);
    printf("RESULT_INSERT_MS=%.3f\n", cb.insert_ms);
    printf("WRITE_MS=%.3f\n", write_ms);
    status = 0;

fim:
    //liberação de memória
    est_apagar(&res, false);
    est_apagar(&base, true);
    query_apagar(&query);
    if (status != 0) printf("STATUS=ERROR\n");
    return status;
}