#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../include/tads/item.h"
#include "../include/tads/pilha.h"
#include "../include/tads/fila.h"
#include "../include/tads/lista_seq.h"
#include "../include/tads/lista_enc.h"
#include "../include/tads/lista_cab.h"
#include "../include/tads/lista_gen.h"
#include "../include/tads/lista_cruz.h"
#include "../include/reader.h"
#include "../include/query.h"
//TESTAR APARENTEMENTE JA ESTA TUDO FUNCIONANDO
// Função auxiliar para instanciar dinamicamente qualquer estrutura escolhida (--base ou --result)
static void *criar_estrutura(const char *tipo) {
    if (!tipo) return NULL;

    if (strcmp(tipo, "pilha") == 0) {
        return pilha_criar();
    } else if (strcmp(tipo, "fila") == 0) {
        return fila_criar();
    } else if (strcmp(tipo, "lista_seq") == 0) {
        return lista_criar_seq(false);
    } else if (strcmp(tipo, "lista_ord") == 0) {
        return lista_criar_seq(true);
    } else if (strcmp(tipo, "lista_enc") == 0) {
        return lista_criar_enc(false);
    } else if (strcmp(tipo, "lista_cabeca") == 0 || strcmp(tipo, "lista_cab") == 0) {
        return lista_criar_cab();
    } else if (strcmp(tipo, "lista_cruz") == 0) {
        return lcruz_criar();
    } else if (strcmp(tipo, "lista_gen") == 0) {
        return lgen_criar();
    }
    return NULL;
}

// Retorna o tamanho atual de qualquer estrutura base ou de resultado
static int obter_tamanho_estrutura(const char *tipo, const void *est) {
    if (!est || !tipo) return 0;
    if (strcmp(tipo, "pilha") == 0) return pilha_tamanho((PILHA *)est);
    if (strcmp(tipo, "fila") == 0) return fila_tamanho((FILA *)est);
    if (strcmp(tipo, "lista_seq") == 0 || strcmp(tipo, "lista_ord") == 0) return lista_tamanho_seq((LISTA_SEQ *)est);
    if (strcmp(tipo, "lista_enc") == 0) return lista_tamanho_enc((LISTA_ENC *)est);
    if (strcmp(tipo, "lista_cabeca") == 0 || strcmp(tipo, "lista_cab") == 0) return lista_tamanho_cab((LISTA_CAB *)est);
    if (strcmp(tipo, "lista_cruz") == 0) return lcruz_tamanho((LISTA_CRUZADA *)est);
    if (strcmp(tipo, "lista_gen") == 0) return lgen_tamanho((LISTA_GEN *)est);
    return 0;
}

// Obtém o item em um índice genérico de qualquer estrutura
static ITEM *obter_item_estrutura(const char *tipo, void *est, int index) {
    if (!est || !tipo || index < 0) return NULL;
    if (strcmp(tipo, "pilha") == 0) return pilha_obter_item((PILHA *)est, index);
    if (strcmp(tipo, "fila") == 0) return fila_obter_item((FILA *)est, index);
    if (strcmp(tipo, "lista_seq") == 0 || strcmp(tipo, "lista_ord") == 0) return lista_obter_item_seq((LISTA_SEQ *)est, index);
    if (strcmp(tipo, "lista_enc") == 0) return lista_obter_item_enc((LISTA_ENC *)est, index);
    if (strcmp(tipo, "lista_cabeca") == 0 || strcmp(tipo, "lista_cab") == 0) return lista_obter_item_cab((LISTA_CAB *)est, index);
    if (strcmp(tipo, "lista_cruz") == 0) return lcruz_obter_item((LISTA_CRUZADA *)est, index);
    if (strcmp(tipo, "lista_gen") == 0) return lgen_obter_item((LISTA_GEN *)est, index);
    return NULL;
}

// Insere um item aceito na estrutura de resultado conforme o tipo especificado
static void inserir_resultado(const char *tipo_res, void *est_res, ITEM *item, const QUERY *q) {
    if (!tipo_res || !est_res || !item) return;

    if (strcmp(tipo_res, "pilha") == 0) {
        pilha_empilhar((PILHA *)est_res, item);
    } else if (strcmp(tipo_res, "fila") == 0) {
        fila_inserir((FILA *)est_res, item);
    } else if (strcmp(tipo_res, "lista_seq") == 0 || strcmp(tipo_res, "lista_ord") == 0) {
        lista_inserir_seq((LISTA_SEQ *)est_res, item);
    } else if (strcmp(tipo_res, "lista_enc") == 0) {
        lista_inserir_enc((LISTA_ENC *)est_res, item);
    } else if (strcmp(tipo_res, "lista_cabeca") == 0 || strcmp(tipo_res, "lista_cab") == 0) {
        lista_inserir_cab((LISTA_CAB *)est_res, item);
    } else if (strcmp(tipo_res, "lista_cruz") == 0) {
        // Se houver comando CROSS na query, extrai os campos correspondentes
        const char *c_linha = (query_cross_tamanho(q) > 0) ? item_obter_valor_string(item, query_cross_campo(q, 0)) : "geral";
        const char *c_coluna = (query_cross_tamanho(q) > 1) ? item_obter_valor_string(item, query_cross_campo(q, 1)) : "padrao";
        lcruz_inserir((LISTA_CRUZADA *)est_res, item, c_linha ? c_linha : "desconhecido", c_coluna ? c_coluna : "desconhecido");
    } else if (strcmp(tipo_res, "lista_gen") == 0) {
        // Inserção na lista generalizada (átomo)
        lgen_inserir_atomo((LISTA_GEN *)est_res, item);
    }
}

int main(int argc, char *argv[]) {
    char *reader_tipo = NULL;
    char *input_path = NULL;
    char *tipo_base = NULL;
    char *tipo_resultado = NULL;
    char *query_path = NULL;
    char *output_path = NULL;
    int top_k = 0;
    int limit = 0;

    // Parsing robusto dos argumentos de linha de comando
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--reader") == 0 && i + 1 < argc) {
            reader_tipo = argv[++i];
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_path = argv[++i];
        } else if (strcmp(argv[i], "--base") == 0 && i + 1 < argc) {
            tipo_base = argv[++i];
        } else if (strcmp(argv[i], "--result") == 0 && i + 1 < argc) {
            tipo_resultado = argv[++i];
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query_path = argv[++i];
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (strcmp(argv[i], "--top-k") == 0 && i + 1 < argc) {
            top_k = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--limit") == 0 && i + 1 < argc) {
            limit = atoi(argv[++i]);
        }
    }

    if (!reader_tipo || !input_path || !tipo_base) {
        printf("Uso incorreto. Exemplo:\n");
        printf("%s --reader nasa --input dados/asteroid_dataset_20251019.csv --base lista_enc --result lista_ord --query consultas/q1.txt --output resultado.csv --top-k 100 --limit 1000\n", argv[0]);
        return 1;
    }

    // 1. Alocações
    void *estrutura_base = criar_estrutura(tipo_base);
    if (!estrutura_base) {
        printf("Erro ao alocar estrutura base '%s'.\n", tipo_base);
        return 1;
    }

    void *estrutura_resultado = NULL;
    if (tipo_resultado) {
        estrutura_resultado = criar_estrutura(tipo_resultado);
        if (!estrutura_resultado) {
            printf("Erro ao alocar estrutura de resultado '%s'.\n", tipo_resultado);
            return 1;
        }
    }

    QUERY *query = NULL;
    if (query_path) {
        query = query_carregar(query_path);
        if (!query) {
            printf("Erro ao carregar o arquivo de consulta: %s\n", query_path);
            return 1;
        }
    }

    // 2. Carregamento do dataset com medição de tempo
    clock_t inicio_carga = clock();
    bool sucesso = false;
    if (strcmp(reader_tipo, "nasa") == 0) {
        sucesso = carregar_base_nasa(input_path, estrutura_base, tipo_base, limit);
    } else {
        printf("Erro: Leitor '%s' não suportado.\n", reader_tipo);
        if (query) query_apagar(&query);
        return 1;
    }
    clock_t fim_carga = clock();
    double load_ms = (double)(fim_carga - inicio_carga) * 1000.0 / CLOCKS_PER_SEC;

    if (!sucesso) {
        printf("Erro durante o carregamento dos dados.\n");
        if (query) query_apagar(&query);
        return 1;
    }

    int input_records = obter_tamanho_estrutura(tipo_base, estrutura_base);

    // 3. Execução da Busca e Filtragem com medição de tempo
    clock_t inicio_busca = clock();
    int matched_records = 0;
    int output_records = 0;

    clock_t inicio_ins_res = clock();
    double insert_res_ms = 0.0;

    if (query && estrutura_base) {
        int total_base = obter_tamanho_estrutura(tipo_base, estrutura_base);
        for (int i = 0; i < total_base; i++) {
            ITEM *item = obter_item_estrutura(tipo_base, estrutura_base, i);
            if (!item) continue;

            double similaridade = 1.0;
            if (query_avaliar(query, item, &similaridade)) {
                matched_records++;

                if (estrutura_resultado) {
                    clock_t t_ini_ins = clock();
                    // Associa a similaridade ao item caso necessário para ordenação
                    item_atribuir_valor_numerico(item, "similarity", similaridade);
                    inserir_resultado(tipo_resultado, estrutura_resultado, item, query);
                    clock_t t_fim_ins = clock();
                    insert_res_ms += (double)(t_fim_ins - t_ini_ins) * 1000.0 / CLOCKS_PER_SEC;
                    output_records++;
                }
            }
        }
    } else if (estrutura_base) {
        // Caso não haja arquivo de query, todos os registros entram por padrão
        int total_base = obter_tamanho_estrutura(tipo_base, estrutura_base);
        matched_records = total_base;
        output_records = total_base;
        if (estrutura_resultado) {
            for (int i = 0; i < total_base; i++) {
                ITEM *item = obter_item_estrutura(tipo_base, estrutura_base, i);
                if (item) inserir_resultado(tipo_resultado, estrutura_resultado, item, query);
            }
        }
    }
    clock_t fim_busca = clock();
    double search_ms = (double)(fim_busca - inicio_busca) * 1000.0 / CLOCKS_PER_SEC;
    (void)inicio_ins_res;

    // 4. Escrita do Arquivo de Saída (--output) respeitando o --top-k
    clock_t inicio_write = clock();
    if (output_path && estrutura_resultado) {
        FILE *fout = fopen(output_path, "w");
        if (fout) {
            // Escreve o cabeçalho padrão idêntico ao do dataset original
            fprintf(fout, "asteroid_designation;asteroid_fullname;close_approach_date;year;month;distance_au;velocity_km_s;absolute_magnitude;days_until_approach;is_past_event;is_future_event;risk_score;panic_level;threat_category;panic_verdict;on_sentry_list;sentry_impact_prob;sentry_torino_scale;sentry_palermo_scale;sentry_diameter_km\n");

            int total_res = obter_tamanho_estrutura(tipo_resultado, estrutura_resultado);
            int limite_export = (top_k > 0 && top_k < total_res) ? top_k : total_res;

            for (int i = 0; i < limite_export; i++) {
                ITEM *it = obter_item_estrutura(tipo_resultado, estrutura_resultado, i);
                if (it) {
                    item_imprimir_linha_csv(it, fout);
                }
            }
            fclose(fout);
        }
    }
    clock_t fim_write = clock();
    double write_ms = (double)(fim_write - inicio_write) * 1000.0 / CLOCKS_PER_SEC;

    // 5. Impressão das métricas exigidas na saída padrão pelo trabalho
    printf("STATUS-OK\n");
    printf("INPUT RECORDS - %d\n", input_records);
    printf("MATCHED RECORDS S = %d\n", matched_records);
    printf("OUTPUT RECORDS i = %d\n", output_records);
    printf("LOAD NS = %.2f\n", load_ms);
    printf("SEARCH MS = %.3f\n", search_ms);
    printf("RESULT INSERT MS = %.3f\n", insert_res_ms);
    printf("WRITE MS = %.3f\n", write_ms);

    // 6. Limpeza de Memória
    if (query) query_query_apagar_ptr: { query_apagar(&query); }

    // Liberação da base e resultado (conforme o tipo escolhido)
    // Nota: Como os itens são compartilhados (ponteiros), limpa-se as estruturas na ordem correta
    if (estrutura_resultado) {
        if (strcmp(tipo_resultado, "lista_enc") == 0) lista_apagar_enc((LISTA_ENC **)&estrutura_resultado);
        else if (strcmp(tipo_resultado, "lista_seq") == 0 || strcmp(tipo_resultado, "lista_ord") == 0) lista_apagar_seq((LISTA_SEQ **)&estrutura_resultado);
        else if (strcmp(tipo_resultado, "lista_cabeca") == 0 || strcmp(tipo_resultado, "lista_cab") == 0) lista_apagar_cab((LISTA_CAB **)&estrutura_resultado);
        else if (strcmp(tipo_resultado, "pilha") == 0) pilha_apagar((PILHA **)&estrutura_resultado);
        else if (strcmp(tipo_resultado, "fila") == 0) fila_apagar((FILA **)&estrutura_resultado);
        else if (strcmp(tipo_resultado, "lista_cruz") == 0) lcruz_destruir((LISTA_CRUZADA **)&estrutura_resultado);
        else if (strcmp(tipo_resultado, "lista_gen") == 0) lgen_destruir((LISTA_GEN **)&estrutura_resultado);
    }

    if (estrutura_base) {
        if (strcmp(tipo_base, "lista_enc") == 0) lista_apagar_enc((LISTA_ENC **)&estrutura_base);
        else if (strcmp(tipo_base, "lista_seq") == 0 || strcmp(tipo_base, "lista_ord") == 0) lista_apagar_seq((LISTA_SEQ **)&estrutura_base);
        else if (strcmp(tipo_base, "lista_cabeca") == 0 || strcmp(tipo_base, "lista_cab") == 0) lista_apagar_cab((LISTA_CAB **)&estrutura_base);
        else if (strcmp(tipo_base, "pilha") == 0) pilha_apagar((PILHA **)&estrutura_base);
        else if (strcmp(tipo_base, "fila") == 0) fila_apagar((FILA **)&estrutura_base);
        else if (strcmp(tipo_base, "lista_cruz") == 0) lcruz_destruir((LISTA_CRUZADA **)&estrutura_base);
        else if (strcmp(tipo_base, "lista_gen") == 0) lgen_destruir((LISTA_GEN **)&estrutura_base);
    }

    return 0;
}