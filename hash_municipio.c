#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_municipio.h"
#include "utils.h"

#define TABLE_SIZE 100003

typedef struct OffsetNode {
    long file_offset;
    struct OffsetNode *next;
} OffsetNode;

typedef struct MunicipioEntry {
    char chave[150]; 
    long file_offset;
    OffsetNode *offsets; 
    struct MunicipioEntry *next;
} MunicipioEntry;

static MunicipioEntry *hash_table[TABLE_SIZE]; 

static unsigned long hash_func(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TABLE_SIZE;
}

void trim(char *str) {
    char *start = str;
    char *end;

    while (*start == ' ') start++;

    if (*start == 0) {
        *str = 0; 
        return;
    }

    if (start != str) {
        char *dst = str;
        while (*start) {
            *dst++ = *start++;
        }
        *dst = 0;
    }

    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') {
        *end = 0;
        end--;
    }
}


void criar_chave(const char *municipio, const char *uf, char *chave) {
    char municipio_trim[100], uf_trim[10];
    
    strcpy(municipio_trim, municipio);
    strcpy(uf_trim, uf);

    trim(municipio_trim);
    trim(uf_trim);

    sprintf(chave, "%s/%s", municipio_trim, uf_trim);
}

void hash_insert_municipio(const char *municipio, const char *uf, long offset) {
    char chave[150];
    unsigned long index;
    MunicipioEntry *entry, *new_entry;
    OffsetNode *new_offset;

    criar_chave(municipio, uf, chave);

    index = hash_func(chave);

    entry = hash_table[index];
    while (entry) {
        if (strcmp(entry->chave, chave) == 0) {
            new_offset = (OffsetNode *)malloc(sizeof(OffsetNode));
            if (!new_offset) {
                fprintf(stderr, "Erro de memória ao alocar OffsetNode\n");
                exit(EXIT_FAILURE);
            }
            new_offset->file_offset = offset;
            new_offset->next = entry->offsets;
            entry->offsets = new_offset;
            return;
        }
        entry = entry->next;
    }

    new_entry = (MunicipioEntry *)malloc(sizeof(MunicipioEntry));
    if (!new_entry) {
        fprintf(stderr, "Erro de memória ao alocar MunicipioEntry\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_entry->chave, chave);
    new_entry->offsets = NULL;
    new_entry->next = NULL;

    new_offset = (OffsetNode *)malloc(sizeof(OffsetNode));
    if (!new_offset) {
        fprintf(stderr, "Erro de memória ao alocar OffsetNode\n");
        free(new_entry); 
        exit(EXIT_FAILURE);
    }
    new_offset->file_offset = offset;
    new_offset->next = NULL;
    new_entry->offsets = new_offset;
    new_entry->next = hash_table[index];
    hash_table[index] = new_entry;
}

long hash_search_municipio(const char *municipio, const char *uf) {
    char chave[150];
    unsigned long index;
    MunicipioEntry *entry;
    criar_chave(municipio, uf, chave);

    index = hash_func(chave);
    entry = hash_table[index];
    while (entry) {
        if (strcmp(entry->chave, chave) == 0) {
            return entry->file_offset;
        }
        entry = entry->next;
    }
    return -1;
}

void construir_hash_municipio(FILE *file) {
    char line[MAX_LINE_LENGTH];
    long offset;
    Inscricao inscricao;

    ignorar_cabecalho(file);

    while (1) {
        offset = ftell(file);
        if (!fgets(line, sizeof(line), file)) break;

        remover_quebra_linha(line);
        if (!parse_csv_line(line, &inscricao)) continue;

        hash_insert_municipio(inscricao.NO_MUNICIPIO_PROVA, inscricao.SG_UF_PROVA, offset);
    }
    fseek(file, 0, SEEK_SET);
}


void listar_inscricoes_por_municipio(FILE *file, const char *municipio, const char *uf) {
    char chave[150];
    unsigned long index;
    MunicipioEntry *entry;
    OffsetNode *node;
    char line[MAX_LINE_LENGTH];
    Inscricao inscricao;

    criar_chave(municipio, uf, chave);
    printf("Buscando pela chave: '%s'\n", chave);
    index = hash_func(chave);

    entry = hash_table[index];
    while (entry && strcmp(entry->chave, chave) != 0) {
        entry = entry->next;
    }

    if (!entry) {
        printf("Nenhuma inscricao encontrada para %s/%s.\n", municipio, uf);
        return;
    }

    printf("Inscricoes para %s/%s:\n", municipio, uf);

    node = entry->offsets;
    while (node) {
        fseek(file, node->file_offset, SEEK_SET);
        if (fgets(line, sizeof(line), file)) {
            remover_quebra_linha(line);
            if (parse_csv_line(line, &inscricao)) {
                printf("Inscricao: %s, Nota Redacao: %d\n",
                    inscricao.NU_INSCRICAO, inscricao.NU_NOTA_REDACAO);
            }
        }
        node = node->next;
    }
}

void liberar_hash_municipio() {
    int i;
    MunicipioEntry *entry;
    MunicipioEntry *temp_entry;
    OffsetNode *offset;
    OffsetNode *temp_offset;

    for (i = 0; i < TABLE_SIZE; i++) {
        entry = hash_table[i];
        while (entry) {
            temp_entry = entry;
            entry = entry->next;

            offset = temp_entry->offsets;
            while (offset) {
                temp_offset = offset;
                offset = offset->next;
                free(temp_offset);
            }

            free(temp_entry);
        }
        hash_table[i] = NULL;
    }
}