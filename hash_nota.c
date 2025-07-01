#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_nota.h"
#include "utils.h"

#define TABLE_SIZE 100003

typedef struct OffsetNode {
    long file_offset;
    struct OffsetNode *next;
} OffsetNode;

typedef struct NotaEntry {
    int nota;
    OffsetNode *offsets;
    struct NotaEntry *next;
} NotaEntry;

static NotaEntry *hash_table[TABLE_SIZE];

static unsigned long hash_func_int(int key) {
    return ((unsigned long)key) % TABLE_SIZE;
}

static void insert_offset(NotaEntry *entry, long offset) {
    OffsetNode *new_node = (OffsetNode *)malloc(sizeof(OffsetNode));
    if (!new_node) {
        fprintf(stderr, "Erro de memoria\n");
        exit(1);
    }
    new_node->file_offset = offset;
    new_node->next = entry->offsets;
    entry->offsets = new_node;
}

static NotaEntry *find_nota_entry(int nota) {
    unsigned long index = hash_func_int(nota);
    NotaEntry *entry = hash_table[index];
    while (entry) {
        if (entry->nota == nota) return entry;
        entry = entry->next;
    }
    return NULL;
}

void hash_insert_notas(int nota, long offset) {
    unsigned long index = hash_func_int(nota);
    NotaEntry *entry = hash_table[index];

    while (entry) {
        if (entry->nota == nota) {
            insert_offset(entry, offset);
            return;
        }
        entry = entry->next;
    }

    entry = (NotaEntry *)malloc(sizeof(NotaEntry));
    if (!entry) {
        fprintf(stderr, "Erro de memoria\n");
        exit(1);
    }
    entry->nota = nota;
    entry->offsets = NULL;

    insert_offset(entry, offset);

    entry->next = hash_table[index];
    hash_table[index] = entry;
}

void construir_hash_notas(FILE *file) {
    char line[MAX_LINE_LENGTH];
    long offset;
    Inscricao inscricao;
    NotaEntry *entry;
    unsigned long index;

    ignorar_cabecalho(file);

    while (1) {
        offset = ftell(file);
        if (!fgets(line, sizeof(line), file)) break;
        remover_quebra_linha(line);
        if (!parse_csv_line(line, &inscricao)) continue;

        index = hash_func_int(inscricao.NU_NOTA_REDACAO);
        entry = hash_table[index];
        while (entry) {
            if (entry->nota == inscricao.NU_NOTA_REDACAO) break;
            entry = entry->next;
        }

        if (!entry) {
            entry = (NotaEntry *)malloc(sizeof(NotaEntry));
            if (!entry) {
                fprintf(stderr, "Erro de memoria\n");
                exit(1);
            }
            entry->nota = inscricao.NU_NOTA_REDACAO;
            entry->offsets = NULL;
            entry->next = hash_table[index];
            hash_table[index] = entry;
        }

        insert_offset(entry, offset);
    }
    fseek(file, 0, SEEK_SET);
}

void listar_maior_nota_com_hash(FILE *file) {
    char line[MAX_LINE_LENGTH];
    Inscricao inscricao;
    int maior_nota = -1;
    int i;
    NotaEntry *entry;
    OffsetNode *node;
    int cont = 0;

    /* Encontra a maior nota */
    for (i = 0; i < TABLE_SIZE; i++) {
        entry = hash_table[i];
        while (entry) {
            if (entry->nota > maior_nota) {
                maior_nota = entry->nota;
            }
            entry = entry->next;
        }
    }

    if (maior_nota == -1) {
        printf("Nao foi possivel encontrar a maior nota.\n");
        return;
    }

    printf("Maior nota de redacao: %d\n", maior_nota);

    entry = find_nota_entry(maior_nota);
    if (!entry) {
        printf("Nenhuma inscricao com a maior nota encontrada.\n");
        return;
    }

    node = entry->offsets;
    while (node) {
        fseek(file, node->file_offset, SEEK_SET);

        if (!fgets(line, sizeof(line), file)) {
            printf("Erro ao ler o arquivo.\n");
            return;
        }
        remover_quebra_linha(line);

        if (parse_csv_line(line, &inscricao)) {
            printf("Inscricao: %s, Municipio: %s, UF: %s, Nota: %d\n",
                   inscricao.NU_INSCRICAO,
                   inscricao.NO_MUNICIPIO_PROVA,
                   inscricao.SG_UF_PROVA,
                   inscricao.NU_NOTA_REDACAO);
                cont++;
        }
        node = node->next;
    }
    printf("\nTotal de inscricoes com nota %d: %d\n", maior_nota, cont);
}

void liberar_hash_nota() {
    int i;
    NotaEntry *entry;
    NotaEntry *temp_entry;
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