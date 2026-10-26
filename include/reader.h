#ifndef READER_H
#define READER_H

#include <stdbool.h>
#include "estrutura.h"
#include "query.h"

// Carrega o CSV da NASA para a estrutura --base. "q" (pode ser NULL) fornece GROUP/CROSS
// para lista_gen e lista_cruz. limite = 0 carrega tudo.
bool carregar_base_nasa(const char *caminho_arquivo, ESTRUTURA *base, const QUERY *q, int limite);

// Cabeçalho original do CSV de entrada (usado idêntico no --output).
const char *reader_cabecalho(void);

#endif /* READER_H */
