#include <stdio.h>
#include "hash_inscricao.h"
#include "hash_nota.h"
/* #include "hash_municipio.h" */
#include <string.h>
#include "utils.h"

/* gcc main.c hash_inscricao.c hash_nota.c utils.c -o execute -ansi -pedantic */
/* ./execute */

void menuInterativo(FILE *file) {
    int opcao;
    char num_inscricao[20];
    char municipio[100];
    char uf[3];

    do {
        printf("\n--- MENU ---\n");
        printf("1 - Buscar inscricao por numero\n");
        printf("2 - Listar inscricoes com maior nota na redacao\n");
        printf("3 - Listar inscricoes por municipio e UF\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Digite o numero de inscricao: ");
                scanf("%19s", num_inscricao);
                buscar_por_inscricao_hash(file, num_inscricao);
                break;
            case 2:
                listar_maior_nota_com_hash(file);
                break;
            case 3:
            /*   printf("Digite o nome do municipio: "); */
            /*    scanf(" %99[^\n]", municipio); */
            /*    printf("Digite a UF: "); */
            /*   scanf(" %2s", uf); */
            /*    listar_inscricoes_por_municipio_uf(file, municipio, uf); */
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);
}

int main() {
    FILE *file = fopen("MICRODADOS_ENEM_2023.csv", "rb");
    if (!file) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    construir_hash_inscricao(file); 
    construir_hash_notas(file);   
    /*construir_hash_municipio_uf(file);*/

    menuInterativo(file);

    fclose(file);
    return 0;
}