#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_inscricao.h"
#include "utils.h"

#define TABLE_SIZE 100003


typedef struct HashEntry {
    char inscricao[20];
    long file_offset;
    struct HashEntry *next;
} HashEntry;

static HashEntry *hash_table[TABLE_SIZE]; 

static unsigned long hash_func(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TABLE_SIZE;
}

void hash_insert(const char *inscricao, long offset) {
    unsigned long index = hash_func(inscricao);
    HashEntry *new_entry = (HashEntry *)malloc(sizeof(HashEntry));
    if (!new_entry) {
        fprintf(stderr, "Erro de memoria\n");
        exit(1);
    }
    strcpy(new_entry->inscricao, inscricao);
    new_entry->file_offset = offset;
    new_entry->next = hash_table[index];
    hash_table[index] = new_entry;
}

long hash_search(const char *inscricao) {
    unsigned long index = hash_func(inscricao);
    HashEntry *entry = hash_table[index];
    while (entry) {
        if (strcmp(entry->inscricao, inscricao) == 0) {
            return entry->file_offset;
        }
        entry = entry->next;
    }
    return -1;
}

void construir_hash_inscricao(FILE *file) {
    char line[MAX_LINE_LENGTH];
    long offset;
    char inscr[20];
    char *p;
    int len;

    ignorar_cabecalho(file);

    while (1) {
        offset = ftell(file);
        if (!fgets(line, sizeof(line), file)) break;

        p = strchr(line, ';');
        if (!p) continue;

        len = p - line;
        if (len >= 20) len = 19;

        strncpy(inscr, line, len);
        inscr[len] = '\0';

        hash_insert(inscr, offset);
    }
    fseek(file, 0, SEEK_SET);
}

void buscar_por_inscricao_hash(FILE *file, const char *num_inscricao) {
    long offset = hash_search(num_inscricao);
    char line[MAX_LINE_LENGTH];
    Inscricao inscricao;

    if (offset == -1) {
        printf("Inscricao nao encontrada.\n");
        return;
    }

    fseek(file, offset, SEEK_SET);

    if (!fgets(line, sizeof(line), file)) {
        printf("Erro ao ler o arquivo.\n");
        return;
    }
    remover_quebra_linha(line);

    if (!parse_csv_line(line, &inscricao)) {
        printf("Erro ao analisar linha.\n");
        return;
    }

    if (strcmp(inscricao.NU_INSCRICAO, num_inscricao) == 0) {
        printf("\nInscricao encontrada:\n");
        printf("Numero: %s\n", inscricao.NU_INSCRICAO);
        printf("Municipio: %s\n", inscricao.NO_MUNICIPIO_PROVA);
        printf("UF: %s\n", inscricao.SG_UF_PROVA);
        printf("Nota Redacao: %d\n", inscricao.NU_NOTA_REDACAO);
    } else {
        printf("Inscricao nao encontrada.\n");
    }
}
