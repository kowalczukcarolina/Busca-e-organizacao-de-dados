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

int main(int argc, char *argv[]) {
    char *reader_tipo = NULL;
    char *input_path = NULL;
    char *tipo_base = NULL;
    int limit = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--reader") == 0 && i + 1 < argc) {
            reader_tipo = argv[i + 1];
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_path = argv[i + 1];
        } else if (strcmp(argv[i], "--base") == 0 && i + 1 < argc) {
            tipo_base = argv[i + 1];
        } else if (strcmp(argv[i], "--limit") == 0 && i + 1 < argc) {
            limit = atoi(argv[i + 1]);
        }
    }

    if (!reader_tipo || !input_path || !tipo_base) {
        printf("Uso incorreto. Exemplo:\n");
        printf("%s --reader nasa --input dados/asteroids.csv --base lista_enc --limit 10000\n", argv[0]);
        return 1;
    }

    void *estrutura_base = NULL;

    if (strcmp(tipo_base, "pilha") == 0) {
        estrutura_base = pilha_criar();
    } else if (strcmp(tipo_base, "fila") == 0) {
        estrutura_base = fila_criar();
    } else if (strcmp(tipo_base, "lista_seq") == 0) {
        estrutura_base = lista_criar_seq(false);
    } else if (strcmp(tipo_base, "lista_ord") == 0) {
        estrutura_base = lista_criar_seq(true);
    } else if (strcmp(tipo_base, "lista_enc") == 0) {
        estrutura_base = lista_criar_enc(false);
    } else if (strcmp(tipo_base, "lista_cabeca") == 0) {
        estrutura_base = lista_criar_cab();
    } else if (strcmp(tipo_base, "lista_cruz") == 0) {
        estrutura_base = lcruz_criar();
    } else if (strcmp(tipo_base, "lista_gen") == 0) {
        estrutura_base = lgen_criar();
    } else {
        printf("Erro: Estrutura base '%s' desconhecida.\n", tipo_base);
        return 1;
    }

    if (!estrutura_base) {
        printf("Erro ao alocar memória para a estrutura base.\n");
        return 1;
    }

    bool sucesso = false;

    if (strcmp(reader_tipo, "nasa") == 0) {
        sucesso = carregar_base_nasa(input_path, estrutura_base, tipo_base, limit);
    } else {
        printf("Erro: Leitor '%s' não suportado.\n", reader_tipo);
        return 1;
    }

    if (sucesso) {
        printf("STATUS-OK\n");
    } else {
        printf("Erro durante o carregamento dos dados.\n");
    }

    return 0;
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

int main(int argc, char *argv[]) {
    char *reader_tipo = NULL;
    char *input_path = NULL;
    char *tipo_base = NULL;
    int limit = 0; // 0 significa toda a base

    // Análise dos argumentos passados via terminal
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--reader") == 0 && i + 1 < argc) {
            reader_tipo = argv[i + 1];
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_path = argv[i + 1];
        } else if (strcmp(argv[i], "--base") == 0 && i + 1 < argc) {
            tipo_base = argv[i + 1];
        } else if (strcmp(argv[i], "--limit") == 0 && i + 1 < argc) {
            limit = atoi(argv[i + 1]);
        }
    }

    // Validação dos parâmetros obrigatórios
    if (!reader_tipo || !input_path || !tipo_base) {
        printf("Uso incorreto. Exemplo:\n");
        printf("%s --reader nasa --input dados/asteroids.csv --base lista_enc --limit 10000\n", argv[0]);
        return 1;
    }

    void *estrutura_base = NULL;

    // Instanciação dinâmica da estrutura escolhida em --base
    if (strcmp(tipo_base, "pilha") == 0) {
        estrutura_base = pilha_criar();
    } else if (strcmp(tipo_base, "fila") == 0) {
        estrutura_base = fila_criar();
    } else if (strcmp(tipo_base, "lista_seq") == 0) {
        estrutura_base = lista_criar_seq(false); // Não ordenada
    } else if (strcmp(tipo_base, "lista_ord") == 0) {
        estrutura_base = lista_criar_seq(true);  // Ordenada
    } else if (strcmp(tipo_base, "lista_enc") == 0) {
        estrutura_base = lista_criar_enc(false);
    } else if (strcmp(tipo_base, "lista_cabeca") == 0) {
        estrutura_base = lista_criar_cab();
    } else if (strcmp(tipo_base, "lista_cruz") == 0) {
        estrutura_base = lcruz_criar();
    } else if (strcmp(tipo_base, "lista_gen") == 0) {
        estrutura_base = lgen_criar();
    } else {
        printf("Erro: Estrutura base '%s' desconhecida.\n", tipo_base);
        return 1;
    }

    if (!estrutura_base) {
        printf("Erro ao alocar memória para a estrutura base.\n");
        return 1;
    }

    bool sucesso = false;

    // Direcionamento para o leitor correto
    if (strcmp(reader_tipo, "nasa") == 0) {
        sucesso = carregar_base_nasa(input_path, estrutura_base, tipo_base, limit);
    } else {
        printf("Erro: Leitor '%s' não suportado.\n", reader_tipo);
        return 1;
    }

    if (sucesso) {
        printf("STATUS-OK\n");
    } else {
        printf("Erro durante o carregamento dos dados.\n");
    }

    return 0;
}*/