#include "Source/mapa.c"

int main(){
    int linhas, colunas, qtd_chaves;
    int x = 1;
    while (0<x<6){
        printf("Escolha a operacao que deseja realizar:\n");
        printf("1) Receber o mapa através de um arquivo\n");
        printf("2) Realizar a busca por um caminho válido\n");
        printf("3) Criador de um mapa\n");
        printf("4) Guarda e mostra caminhos possiveis\n");
        printf("5) Saida de outra forma\n");
        printf("6) Limpar lista\n");
        printf("7) Sair do programa\n");
        scanf("%d",&x);
        switch (x){
        case 1:
            int **matriz = ler_arquivo(&linhas, &colunas, &qtd_chaves);
            if (matriz != NULL) {
                printf("Matriz Convertida:\n");
                for (int i = 0; i < linhas; i++) {
                    for (int j = 0; j < colunas; j++) {
                        printf("%d ", matriz[i][j]);
                    }
                    printf("\n");
                }
            }
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            limpar_mapa(matriz, linhas, colunas);
            break;
        default:
            printf("Valor invalido, favor verificar");
            break;
        }
    }
    return 0;
}