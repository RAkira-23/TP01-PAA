#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "Source/mapa.c"
#include "Source/busca.c"
#include "Source/globo.c"

int main(void){
    system("chcp 65001 > nul");
    int linhas = 0, colunas = 0, chaves = 0, opcao = 0;
    int **mapa = NULL;
    int **caminho = NULL;
    int linhas_caminho = 0;
    bool analise = false;
    
    // Atualizado para sair apenas na opcao 8
    while (opcao != 8) {
        puts("1) Carregar mapa\n2) Buscar caminho\n3) Criar mapa\n4) Todos os caminhos\n5) Saida com tempo\n6) Limpar mapa\n7) Movimentar no Globo\n8) Sair\n9) Alternar modo de analise");
        printf("Analise: %s\nOpcao: ", analise ? "ativada" : "desativada");
        
        if (scanf("%d", &opcao) != 1) break;
        
        switch (opcao) {
        case 1: {
            int l, c, q;
            int **novo = ler_arquivo(&l, &c, &q);
            if (novo) {
                limpar_mapa(mapa, linhas, colunas);
                limpar_mapa(caminho, linhas_caminho, 2);
                caminho = NULL;
                linhas_caminho = 0;

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
            if (!mapa) puts("Carregue o mapa primeiro.");
            else {
                limpar_mapa(caminho, linhas_caminho, 2);
                caminho = encontrar_caminho(mapa, linhas, colunas, chaves, analise, &linhas_caminho);
            }
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
            }else{
                printf("Mapa já existente, favor limpar ele primeiro\n");
            }
            break;
        case 4:
            if (!mapa) puts("Carregue o mapa primeiro.");
            else salvar_todos_caminhos(mapa, linhas, colunas, chaves, analise, "caminhos.txt");
            break;
        case 5: 
            if (!mapa || !caminho){
                printf("Carregue o mapa primeiro, ou execute a busca do caminho\n");
            }else{
                imprime_com_tempo(mapa, linhas, colunas, caminho, linhas_caminho);
            }
            break;
        case 6:
            limpar_mapa(mapa, linhas, colunas);
            mapa = NULL; linhas = colunas = chaves = 0;
            limpar_mapa(caminho, linhas_caminho, 2);
            caminho = NULL; linhas_caminho = 0;
            puts("Mapa liberado.\n"); 
            break;
        case 7:
            mover_no_globo(mapa, linhas, colunas);
            break;
        case 8:
            // Sair do programa
            break;
        case 9: 
            analise = !analise; 
            break;
        default: 
            puts("Opcao invalida.");
        }
        printf("\n");
    }
    
    limpar_mapa(mapa, linhas, colunas);
    limpar_mapa(caminho, linhas_caminho, 2);
    caminho = NULL;
    return 0;
}