#include <stdio.h>
#include "Source/mapa.c"
#include "Source/busca.c"

int main(void) {
    int linhas = 0, colunas = 0, chaves = 0, opcao = 0;
    int **mapa = NULL;
    while (opcao != 7) {
        puts("1) Carregar mapa\n2) Buscar caminho\n3) Criar mapa\n4) Todos os caminhos\n5) Saida de outra forma\n6) Limpar mapa\n7) Sair");
        printf("Opcao: ");
        if (scanf("%d", &opcao) != 1) break;
        switch (opcao) {
        case 1: {
            int l, c, q;
            int **novo = ler_arquivo(&l, &c, &q);
            if (novo) {
                limpar_mapa(mapa, linhas, colunas);
                mapa = novo; linhas = l; colunas = c; chaves = q;
                puts("Mapa carregado:");
                for (int i = 0; i < linhas; i++) {
                    for (int j = 0; j < colunas; j++) printf("%d ", mapa[i][j]);
                    putchar('\n');
                }
            }
            break;
        }
        case 2:
            if (!mapa) puts("Carregue o mapa primeiro.\n");
            else encontrar_caminho(mapa, linhas, colunas, chaves);
            break;
        case 4:
            if (!mapa) puts("Carregue o mapa primeiro.\n");
            else salvar_todos_caminhos(mapa, linhas, colunas, chaves, "caminhos.txt");
            break;
        case 3:
            if (!mapa){
                printf("Informe o valor de linhas: ");
                scanf("%d",&linhas);
                printf("Informe o valor de colunas: ");
                scanf("%d",&colunas);
                printf("Informe o valor de chaves: ");
                scanf("%d",&chaves);
                mapa = gerar_mapa(linhas,colunas, chaves, (unsigned int)time(NULL));
            }else{printf("Mapa já existente, favor limpar ele primeiro");}
            break;
        case 5: puts("Opcao ainda nao implementada.\n"); break;
        case 6:
            limpar_mapa(mapa, linhas, colunas);
            mapa = NULL; linhas = colunas = chaves = 0;
            puts("Mapa liberado.\n"); break;
        case 7: break;
        default: puts("Opcao invalida.\n");
        }
    }
    limpar_mapa(mapa, linhas, colunas);
    return 0;
}
