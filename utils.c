#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int parse_csv_line(char *line, Inscricao *inscricao) {
    int field_pos = 0;
    char *start = line;
    char *end;
    int in_quotes = 0;

    while (*start) {
        end = start;
        if (*start == '"') {
            in_quotes = 1;
            start++;
            end++;
            while (*end && (*end != '"' || (*(end+1) != ';' && *(end+1) != '\0' && *(end+1) != '\n'))) {
                end++;
            }
            if (*end != '"') {
                return 0;
            }
        } else {
            while (*end && *end != ';' && *end != '\n') {
                end++;
            }
        }
        switch (field_pos) {
            case 0:
                strncpy(inscricao->NU_INSCRICAO, start, end - start);
                inscricao->NU_INSCRICAO[end - start] = '\0';
                break;
            case 20:
                strncpy(inscricao->NO_MUNICIPIO_PROVA, start, end - start);
                inscricao->NO_MUNICIPIO_PROVA[end - start] = '\0';
                break;
            case 22:
                strncpy(inscricao->SG_UF_PROVA, start, end - start);
                inscricao->SG_UF_PROVA[end - start] = '\0';
                break;
            case 50:
                if (end - start == 0) {
                    inscricao->NU_NOTA_REDACAO = 0;
                } else {
                    char nota_str[10];
                    strncpy(nota_str, start, end - start);
                    nota_str[end - start] = '\0';
                    inscricao->NU_NOTA_REDACAO = atoi(nota_str);
                }
                break;
        }
        if (*end == '"') end++;
        if (*end == ';') end++;
        start = end;
        field_pos++;
    }
    return 1;
}

void remover_quebra_linha(char *line) {
    line[strcspn(line, "\r\n")] = '\0';
}

int ignorar_cabecalho(FILE *file) {
    char line[MAX_LINE_LENGTH];
    return fgets(line, sizeof(line), file) != NULL;
}
