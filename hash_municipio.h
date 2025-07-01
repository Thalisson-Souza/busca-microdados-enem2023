#ifndef HASH_MUNICIPIO_H
#define HASH_MUNICIPIO_H

#include <stdio.h>
#include "utils.h"  

void construir_hash_municipio(FILE *file);
void listar_inscricoes_por_municipio(FILE *file, const char *municipio, const char *uf);
void liberar_hash_municipio();

#endif
