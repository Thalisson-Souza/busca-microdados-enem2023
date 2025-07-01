#ifndef UTILS_H
#define UTILS_H

#define MAX_LINE_LENGTH 2048

typedef struct {
    char NU_INSCRICAO[20];
    char NO_MUNICIPIO_PROVA[100];
    char SG_UF_PROVA[3];
    int NU_NOTA_REDACAO;
} Inscricao;

int parse_csv_line(char *line, Inscricao *inscricao);
void remover_quebra_linha(char *line);
int ignorar_cabecalho(FILE *file);
unsigned long hash_func_string(const char *str);


#endif
