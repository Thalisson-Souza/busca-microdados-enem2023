#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int parse_csv_line(char *line, Inscricao *inscricao) {
    int field_pos = 0;
    char *start = line;
    char *end;

    while (*start) {
        if (*start == '"') { 
            start++;
            end = strchr(start, '"');
            if (!end) return 0; 
        } else { 
            end = strchr(start, ';');
        }

        if (!end) end = start + strlen(start); 

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
                inscricao->NU_NOTA_REDACAO = (end - start > 0) ? atoi(start) : 0;
                break;
        }

        start = (*end == '"') ? end + 2 : end + 1;
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
