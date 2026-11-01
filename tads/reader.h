#ifndef READER_H
#define READER_H

#include <stdbool.h>
#include "estrutura.h"
#include "query.h"

// Leitor da Parte 1 (NASA)
bool carregar_base_nasa(const char *caminho_arquivo, ESTRUTURA *base, const QUERY *q, int limite);
const char *reader_cabecalho(void);

// Leitor da Parte 2 (Chamados / Dados Abertos)
bool carregar_base_chamados(const char *caminho_arquivo, ESTRUTURA *base, const QUERY *q, int limite);
const char *chamados_reader_cabecalho(void);

#endif