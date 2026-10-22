#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/tads/item.h"
#include "../include/tads/pilha.h"
#include "../include/tads/fila.h"
#include "../include/tads/lista_seq.h"
#include "../include/tads/lista_enc.h"
#include "../include/tads/lista_cab.h"
#include "../include/tads/lista_gen.h"
#include "../include/tads/lista_cruz.h"
#include "../include/reader.h"

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

    // Validação de parâmetros obrigatórios mínimos
    if (!reader_tipo || !input_path || !tipo_base) {
        printf("Uso incorreto. Exemplo:\n");
        printf("%s --reader nasa --input dados/asteroids.csv --base lista_enc --result lista_ord --query consultas/q1.txt --output resultado.csv --top-k 100 --limit 10000\n", argv[0]);
        return 1;
    }

    // Alocação da estrutura base que armazenará todo o dataset carregado
    void *estrutura_base = criar_estrutura(tipo_base);
    if (!estrutura_base) {
        printf("Erro: Estrutura base '%s' desconhecida ou falha ao alocar.\n", tipo_base);
        return 1;
    }

    // Alocação opcional/recomendada da estrutura de resultado, caso informada
    void *estrutura_resultado = NULL;
    if (tipo_resultado) {
        estrutura_resultado = criar_estrutura(tipo_resultado);
        if (!estrutura_resultado) {
            printf("Erro: Estrutura de resultado '%s' desconhecida ou falha ao alocar.\n", tipo_resultado);
            // Liberar estrutura_base antes de sair se necessário
            return 1;
        }
    }

    bool sucesso = false;

    // Carregamento da base de dados através do leitor especificado
    if (strcmp(reader_tipo, "nasa") == 0) {
        sucesso = carregar_base_nasa(input_path, estrutura_base, tipo_base, limit);
    } else {
        printf("Erro: Leitor '%s' não suportado.\n", reader_tipo);
        return 1;
    }

    if (!sucesso) {
        printf("Erro durante o carregamento dos dados da base.\n");
        return 1;
    }

    // Impressão obrigatória exigida pela especificação do trabalho
    printf("STATUS-OK\n");

    // TODO: Chamar o módulo de processamento de consultas usando `query_path`, `estrutura_base`, 
    // `estrutura_resultado`, `top_k` e gerando a saída em `output_path`.

    // Exemplo de encerramento limpo (a ser implementado conforme as funções de destruir de cada TAD)
    // Nota: Lembre-se de destruir estrutura_base e estrutura_resultado ao fim da execução.

    (void)estrutura_resultado;
    (void)query_path;
    (void)output_path;
    (void)top_k;

    return 0;
}