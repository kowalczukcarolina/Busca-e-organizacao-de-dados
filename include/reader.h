#ifndef READER_H
#define READER_H

#include <stdbool.h>
#include "tads/item.h"

// Função responsável por carregar o dataset para qualquer estrutura escolhida (--base)
bool carregar_base_nasa(const char *caminho_arquivo, void *estrutura_base, const char *tipo_base, int limite);

#endif /* READER_H */