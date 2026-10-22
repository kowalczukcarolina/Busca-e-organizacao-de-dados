#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/query.h"

#define MAX_FILTROS 32
#define MAX_SIMS    32
#define MAX_EIXOS   8
#define TAM_CAMPO   64
#define TAM_VALOR   128
#define TAM_LINHA   512
#define MAX_TOKENS  16
#define EPS         1e-9

typedef enum { OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE } OPERADOR;

typedef struct {
    char campo[TAM_CAMPO];
    OPERADOR op;
    bool numerico;           /* true: valor é número ou true/false */
    double valor_num;
    char valor_txt[TAM_VALOR];
} FILTRO;

typedef struct {
    char campo[TAM_CAMPO];
    double alvo;
    double tolerancia;
    double peso;
} CRITERIO_SIM;

struct query {
    FILTRO filtros[MAX_FILTROS];
    int n_filtros;

    CRITERIO_SIM sims[MAX_SIMS];
    int n_sims;

    bool tem_accept;
    double accept;

    char order_campo[TAM_CAMPO];
    bool order_desc;

    char group[MAX_EIXOS][TAM_CAMPO];
    int n_group;

    char cross[MAX_EIXOS][TAM_CAMPO];
    int n_cross;
};

/* ------------------------------------------------------------------ */
/* Utilitários de texto                                               */
/* ------------------------------------------------------------------ */

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    char *fim = s + strlen(s);
    while (fim > s && isspace((unsigned char)fim[-1])) fim--;
    *fim = '\0';
    return s;
}

static bool igual_ci(const char *a, const char *b) {
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) return false;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

/* Copia src para dest se couber e não for vazio. */
static bool copiar(char *dest, size_t tam, const char *src) {
    size_t n = strlen(src);
    if (n == 0 || n >= tam) return false;
    memcpy(dest, src, n + 1);
    return true;
}

/* Divide a linha por '|' preservando campos vazios. Retorna nº de campos
 * ou -1 se exceder max. Modifica a linha (troca '|' por '\0'). */
static int dividir(char *linha, char *campos[], int max) {
    int n = 0;
    campos[n++] = linha;
    for (char *p = linha; *p; p++) {
        if (*p == '|') {
            *p = '\0';
            if (n >= max) return -1;
            campos[n++] = p + 1;
        }
    }
    return n;
}

static bool parse_double(const char *s, double *out) {
    char *fim;
    if (*s == '\0') return false;
    *out = strtod(s, &fim);
    return *fim == '\0';
}

/* Número ou booleano (true/false -> 1/0). */
static bool parse_numero_ou_bool(const char *s, double *out) {
    if (igual_ci(s, "true"))  { *out = 1.0; return true; }
    if (igual_ci(s, "false")) { *out = 0.0; return true; }
    return parse_double(s, out);
}

static bool parse_operador(const char *s, OPERADOR *op) {
    if (strcmp(s, "==") == 0)      *op = OP_EQ;
    else if (strcmp(s, "!=") == 0) *op = OP_NE;
    else if (strcmp(s, "<") == 0)  *op = OP_LT;
    else if (strcmp(s, "<=") == 0) *op = OP_LE;
    else if (strcmp(s, ">") == 0)  *op = OP_GT;
    else if (strcmp(s, ">=") == 0) *op = OP_GE;
    else return false;
    return true;
}

static bool erro(int linha, const char *msg) {
    fprintf(stderr, "Consulta, linha %d: %s\n", linha, msg);
    return false;
}

/* ------------------------------------------------------------------ */
/* Tratamento de cada comando                                         */
/* ------------------------------------------------------------------ */

static bool cmd_filter(QUERY *q, char *tok[], int n, int linha) {
    if (n != 4) return erro(linha, "uso: FILTER|campo|op|valor");
    if (q->n_filtros >= MAX_FILTROS) return erro(linha, "filtros demais");

    FILTRO *f = &q->filtros[q->n_filtros];
    if (!copiar(f->campo, TAM_CAMPO, tok[1])) return erro(linha, "campo vazio ou muito longo");
    if (!parse_operador(tok[2], &f->op))      return erro(linha, "operador invalido (use == != < <= > >=)");
    if (!copiar(f->valor_txt, TAM_VALOR, tok[3])) return erro(linha, "valor vazio ou muito longo");

    f->numerico = parse_numero_ou_bool(tok[3], &f->valor_num);
    q->n_filtros++;
    return true;
}

static bool cmd_sim(QUERY *q, char *tok[], int n, int linha) {
    if (n != 5) return erro(linha, "uso: SIM|campo|alvo|tolerancia|peso");
    if (q->n_sims >= MAX_SIMS) return erro(linha, "criterios SIM demais");

    CRITERIO_SIM *s = &q->sims[q->n_sims];
    if (!copiar(s->campo, TAM_CAMPO, tok[1])) return erro(linha, "campo vazio ou muito longo");
    if (!parse_double(tok[2], &s->alvo))       return erro(linha, "alvo nao numerico");
    if (!parse_double(tok[3], &s->tolerancia)) return erro(linha, "tolerancia nao numerica");
    if (!parse_double(tok[4], &s->peso))       return erro(linha, "peso nao numerico");
    if (s->tolerancia <= 0.0) return erro(linha, "tolerancia deve ser > 0");
    if (s->peso <= 0.0)       return erro(linha, "peso deve ser > 0");

    q->n_sims++;
    return true;
}

static bool cmd_accept(QUERY *q, char *tok[], int n, int linha) {
    if (n != 2) return erro(linha, "uso: ACCEPT|limiar");
    if (!parse_double(tok[1], &q->accept)) return erro(linha, "limiar nao numerico");
    if (q->accept < 0.0 || q->accept > 1.0) return erro(linha, "limiar deve estar em [0,1]");
    q->tem_accept = true;
    return true;
}

static bool cmd_order(QUERY *q, char *tok[], int n, int linha) {
    if (n != 2 && n != 3) return erro(linha, "uso: ORDER|campo|ASC ou DESC");
    if (!copiar(q->order_campo, TAM_CAMPO, tok[1])) return erro(linha, "campo vazio ou muito longo");

    q->order_desc = false;                       /* padrão quando a direção é omitida */
    if (n == 3) {
        if (igual_ci(tok[2], "DESC"))     q->order_desc = true;
        else if (!igual_ci(tok[2], "ASC")) return erro(linha, "direcao deve ser ASC ou DESC");
    }
    return true;
}

static bool cmd_eixos(char dest[][TAM_CAMPO], int *qtd, char *tok[], int n, int linha) {
    if (n < 2) return erro(linha, "informe ao menos um campo");
    if (n - 1 > MAX_EIXOS) return erro(linha, "campos demais");

    *qtd = 0;
    for (int i = 1; i < n; i++) {
        if (!copiar(dest[*qtd], TAM_CAMPO, tok[i])) return erro(linha, "campo vazio ou muito longo");
        (*qtd)++;
    }
    return true;
}

static bool processar_comando(QUERY *q, char *tok[], int n, int linha) {
    const char *cmd = tok[0];

    if (igual_ci(cmd, "FILTER")) return cmd_filter(q, tok, n, linha);
    if (igual_ci(cmd, "SIM"))    return cmd_sim(q, tok, n, linha);
    if (igual_ci(cmd, "ACCEPT")) return cmd_accept(q, tok, n, linha);
    if (igual_ci(cmd, "ORDER"))  return cmd_order(q, tok, n, linha);
    if (igual_ci(cmd, "GROUP"))  return cmd_eixos(q->group, &q->n_group, tok, n, linha);
    if (igual_ci(cmd, "CROSS"))  return cmd_eixos(q->cross, &q->n_cross, tok, n, linha);

    fprintf(stderr, "Consulta, linha %d: comando desconhecido '%s'\n", linha, cmd);
    return false;
}

/* ------------------------------------------------------------------ */
/* API pública                                                        */
/* ------------------------------------------------------------------ */

QUERY *query_carregar(const char *caminho) {
    if (!caminho) return NULL;

    FILE *f = fopen(caminho, "r");
    if (!f) {
        perror("Erro ao abrir arquivo de consulta");
        return NULL;
    }

    QUERY *q = calloc(1, sizeof(QUERY));
    if (!q) {
        fclose(f);
        return NULL;
    }
    strcpy(q->order_campo, "similarity");
    q->order_desc = true;

    char linha[TAM_LINHA];
    int num = 0;
    bool ok = true;

    while (ok && fgets(linha, sizeof(linha), f) != NULL) {
        num++;

        if (strchr(linha, '\n') == NULL && !feof(f)) {
            ok = erro(num, "linha longa demais");
            break;
        }

        char *s = linha;
        /* ignora BOM UTF-8 no início do arquivo */
        if (num == 1 && (unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBB && (unsigned char)s[2] == 0xBF)
            s += 3;

        s = trim(s);
        if (*s == '\0' || *s == '#') continue;

        char *tok[MAX_TOKENS];
        int n = dividir(s, tok, MAX_TOKENS);
        if (n < 0) {
            ok = erro(num, "campos demais na linha");
            break;
        }
        for (int i = 0; i < n; i++) tok[i] = trim(tok[i]);

        ok = processar_comando(q, tok, n, num);
    }

    fclose(f);
    if (!ok) {
        free(q);
        return NULL;
    }
    return q;
}

void query_apagar(QUERY **q) {
    if (!q || !*q) return;
    free(*q);
    *q = NULL;
}

static bool aplicar_operador(OPERADOR op, int cmp) {
    switch (op) {
        case OP_EQ: return cmp == 0;
        case OP_NE: return cmp != 0;
        case OP_LT: return cmp < 0;
        case OP_LE: return cmp <= 0;
        case OP_GT: return cmp > 0;
        case OP_GE: return cmp >= 0;
    }
    return false;
}

bool query_avaliar(const QUERY *q, const ITEM *item, double *similaridade) {
    if (!q || !item) return false;

    /* 1) FILTER: todos precisam ser satisfeitos */
    for (int i = 0; i < q->n_filtros; i++) {
        const FILTRO *f = &q->filtros[i];
        int cmp;

        if (f->numerico) {
            double x = item_obter_valor_numerico(item, f->campo);
            cmp = (x < f->valor_num) ? -1 : (x > f->valor_num ? 1 : 0);
        } else {
            cmp = strcmp(item_obter_valor_string(item, f->campo), f->valor_txt);
        }

        if (!aplicar_operador(f->op, cmp)) return false;
    }

    /* 2) SIM: média ponderada de s(x,q,t) = max(0, 1 - |x-q|/t) */
    double sim = 1.0;
    if (q->n_sims > 0) {
        double soma = 0.0, soma_pesos = 0.0;
        for (int i = 0; i < q->n_sims; i++) {
            const CRITERIO_SIM *s = &q->sims[i];
            double d = item_obter_valor_numerico(item, s->campo) - s->alvo;
            if (d < 0.0) d = -d;

            double si = 1.0 - d / s->tolerancia;
            if (si < 0.0) si = 0.0;

            soma += s->peso * si;
            soma_pesos += s->peso;
        }
        sim = soma / soma_pesos;
    }

    /* 3) ACCEPT: limiar mínimo da similaridade total */
    if (q->tem_accept && sim + EPS < q->accept) return false;

    if (similaridade) *similaridade = sim;
    return true;
}

const char *query_order_campo(const QUERY *q) { return q ? q->order_campo : "similarity"; }
bool query_order_desc(const QUERY *q)         { return q ? q->order_desc : true; }

int query_group_tamanho(const QUERY *q) { return q ? q->n_group : 0; }
const char *query_group_campo(const QUERY *q, int i) {
    return (q && i >= 0 && i < q->n_group) ? q->group[i] : NULL;
}

int query_cross_tamanho(const QUERY *q) { return q ? q->n_cross : 0; }
const char *query_cross_campo(const QUERY *q, int i) {
    return (q && i >= 0 && i < q->n_cross) ? q->cross[i] : NULL;
}

void query_imprimir(const QUERY *q) {
    static const char *nomes_op[] = { "==", "!=", "<", "<=", ">", ">=" };
    if (!q) return;

    for (int i = 0; i < q->n_filtros; i++)
        printf("FILTER %s %s %s\n", q->filtros[i].campo, nomes_op[q->filtros[i].op], q->filtros[i].valor_txt);
    for (int i = 0; i < q->n_sims; i++)
        printf("SIM %s alvo=%g tol=%g peso=%g\n", q->sims[i].campo, q->sims[i].alvo,
               q->sims[i].tolerancia, q->sims[i].peso);
    if (q->tem_accept) printf("ACCEPT %g\n", q->accept);
    printf("ORDER %s %s\n", q->order_campo, q->order_desc ? "DESC" : "ASC");
    if (q->n_group > 0) {
        printf("GROUP");
        for (int i = 0; i < q->n_group; i++) printf(" %s", q->group[i]);
        printf("\n");
    }
    if (q->n_cross > 0) {
        printf("CROSS");
        for (int i = 0; i < q->n_cross; i++) printf(" %s", q->cross[i]);
        printf("\n");
    }
}
