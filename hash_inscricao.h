#ifndef HASH_INSCRICAO_H
#define HASH_INSCRICAO_H

#include <stdio.h>

void construir_hash_inscricao(FILE *file);
void buscar_por_inscricao_hash(FILE *file, const char *num_inscricao);
void liberar_hash_inscricao();

#endif
