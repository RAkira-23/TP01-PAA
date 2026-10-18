#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "../headers/mapa.h"
#ifdef _WIN32
    #include <windows.h>
    #define sleep_ms(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
#endif

//Define struct simples para montagem dos mapas
typedef struct pos{
    int x;
    int y;
} pos;

// Imprime o mapa de forma interativa
void imprime_mapa2(int** mapa, int altura, int largura){
    if (!mapa || altura <= 0 || largura <= 0) return;
    printf("|");
    for (int k = 0; k < largura; k++) printf("-");
    printf("|\n");
    for (int i = 0; i < altura; i++) {
        printf("|");
        for (int j = 0; j < largura; j++){
            if (mapa[i][j] == 0){
                printf(" ");
            } else if (mapa[i][j] == 1){
                printf("#");
            } else if (mapa[i][j] == 2){
                printf("A");
            } else if (mapa[i][j] == 3){
                printf("C");
            } else if (mapa[i][j] == 4){
                printf("X");
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("|");
    for (int k = 0; k < largura; k++) printf("-");
    printf("|\n");
}

//Imprime o mapa de forma direta
void imprime_mapa(int** mapa, int altura, int largura){
        for (int i = 0; i < altura; i++) {
            for (int j = 0; j < largura; j++){
                    printf("%d", mapa[i][j]);
        }
        putchar('\n');
    }
}

void imprime_com_tempo(int** mapa, int altura, int largura, int** caminho, int linha_caminho){
    if (!mapa || !caminho || altura <= 0 || largura <= 0 || linha_caminho <= 0) return;
    int **copia = malloc((size_t)altura * sizeof(int*));
    if (!copia) return;
    for (int i = 0; i < altura; i++) {
        copia[i] = malloc((size_t)largura * sizeof(int));
        if (!copia[i]) {
            limpar_mapa(copia, i, largura);
            return;
        }
        for (int j = 0; j < largura; j++) {
            copia[i][j] = mapa[i][j];
        }
    }
    int delay_ms = 500;
    for (int passo = 0; passo < linha_caminho; passo++){
        if (passo > 0) {
            //a posicao anterior fica livre e a nova vira o aventureiro
            int ant_l = caminho[passo - 1][0];
            int ant_c = caminho[passo - 1][1];
            copia[ant_l][ant_c] = 0;
            int atual_l = caminho[passo][0];
            int atual_c = caminho[passo][1];
            copia[atual_l][atual_c] = 2;
        }
        //Limpa a tela antes da proxima impressao
        system("cls");
        imprime_mapa2(copia, altura, largura);
        //aplica o intervalo de tempo
        sleep_ms(delay_ms);
        //aceleracao dinamica do tempo
    }
    //Pausa antes de mostrar o menu
    sleep_ms(1000);
    //limpa matriz copia
    limpar_mapa(copia, altura, largura);
}

//retorna a distancia de 2 pontos
int calcular_distancia(pos p1, pos p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

//Verificacao se ponto ja esta na matriz
int ponto_existe(pos* localiza, int total, int x, int y){
    for (int i = 0; i < total; i++) {
        if (localiza[i].x == x && localiza[i].y == y){
            return 1;
        }
    }
    return 0;
}

//Libera a memoria de uma matriz indesejada
void limpar_mapa(int **mapa, int linhas, int colunas){
    (void)colunas;
    if (!mapa) return;
    for (int i = 0; i < linhas; i++) free(mapa[i]);
    free(mapa);
}

//retorna a matriz de acordo com a leitura do arquivo
int **ler_arquivo(int *linhas, int *colunas, int *qtd_chaves){
    FILE *arq = fopen("arquivo.txt", "r");
    if (!arq) arq = fopen("../arquivo.txt", "r");
    if (!arq) { puts("Nao foi possivel ler arquivo.txt."); return NULL; }
    int l, c, q;
    if (fscanf(arq, "%d %d %d", &l, &c, &q) != 3 || l <= 0 || c <= 0 || q < 0) {
        puts("Cabecalho invalido."); fclose(arq); return NULL;
    }
    int **mapa = calloc((size_t)l, sizeof(*mapa));
    if (!mapa) { fclose(arq); return NULL; }
    int inicios = 0, baus = 0, chaves = 0;
    for (int i = 0; i < l; i++) {
        mapa[i] = calloc((size_t)c, sizeof(**mapa));
        if (!mapa[i]) goto erro;
        for (int j = 0; j < c; j++){
            char token[16];
            if (fscanf(arq, "%15s", token) != 1 || token[1] != '\0') goto erro;
            switch (token[0]) {
            case '0': mapa[i][j] = 0; break;
            case '1': mapa[i][j] = 1; break;
            case 'A': mapa[i][j] = 2; inicios++; break;
            case 'C': mapa[i][j] = 3; chaves++; break;
            case 'X': mapa[i][j] = 4; baus++; break;
            default: goto erro;
            }
        }
    }
    char extra;
    if (fscanf(arq, " %c", &extra) == 1 || inicios != 1 || baus != 1 || chaves != q) goto erro;
    fclose(arq);
    *linhas = l; *colunas = c; *qtd_chaves = q;
    return mapa;
erro:
    puts("Mapa invalido ou memoria insuficiente.");
    limpar_mapa(mapa, l, c); fclose(arq); return NULL;
}

//Funcao que busca um caminho evitando casos em que se encontra com outros elementos do mapa ou caminhos que podem ser criados futuramente
static bool conectar_bfs(int **mapa, int altura, int largura, int x1, int y1, int x2, int y2, bool apenas_paredes){
    if (x1 == x2 && y1 == y2) return true;
    //inicializacao das variaveis necessarias, sendo uma fila, a pai que sera utilizada para verificar de onde veio cada celula e visitado para evitar que seja visitado mais de uma vez
    size_t total = (size_t)altura * largura;
    pos *fila = malloc(total * sizeof(pos));
    pos *pai = malloc(total * sizeof(pos));
    bool *visitado = calloc(total, sizeof(bool));
    if (!fila || !pai || !visitado) {
        free(fila); free(pai); free(visitado);
        return false;
    }
    //define o inicio de um ponto e define o visitado de modo linear
    int inicio = 0, fim = 0;
    fila[fim++] = (pos){x1, y1};
    visitado[(size_t)x1 * largura + y1] = true;
    bool achou = false;
    static const int dl[] = {-1, 1, 0, 0};
    static const int dc[] = {0, 0, -1, 1};

    while (inicio < fim) {
        pos atual = fila[inicio++];
        if (atual.x == x2 && atual.y == y2){
            achou = true;
            break;
        }
        //variabilidade aleatoria de direcoes para caminhos
        int ordem[4] = {0, 1, 2, 3};
        for (int i = 3; i > 0; i--){
            int r = rand() % (i + 1);
            int tmp = ordem[i];
            ordem[i] = ordem[r];
            ordem[r] = tmp;
        }
        for (int i = 0; i < 4; i++){
            int d = ordem[i];
            int nl = atual.x + dl[d];
            int nc = atual.y + dc[d];
            if (nl >= 0 && nl < altura && nc >= 0 && nc < largura){
                size_t idx = (size_t)nl * largura + nc;
                if (!visitado[idx]){
                    bool valido = false;
                    //se for o destino final do segmento, e sempre valido
                    if (nl == x2 && nc == y2) {
                        valido = true;
                    } else if (mapa[nl][nc] == 1){
                        //parede disponivel para abrir passagem
                        valido = true;
                    } else if (!apenas_paredes && mapa[nl][nc] == 0){
                        //caminho ja aberto
                        valido = true;
                    }
                    if (valido) {
                        visitado[idx] = true;
                        pai[idx] = atual;
                        fila[fim++] = (pos){nl, nc};
                    }
                }
            }
        }
    }

    //gera a rota encontrada conectando os pontos
    if (achou) {
        pos curr = (pos){x2, y2};
        while (curr.x != x1 || curr.y != y1){
            if (mapa[curr.x][curr.y] == 1) {
                mapa[curr.x][curr.y] = 0;
            }
            size_t idx = (size_t)curr.x * largura + curr.y;
            curr = pai[idx];
        }
    }
    free(fila);
    free(pai);
    free(visitado);
    return achou;
}

void gerar_caminho(int** mapa, int altura, int largura, int x1, int y1, int x2, int y2, unsigned int seed){
    (void)seed;
    //tenta somente com paredes
    if (conectar_bfs(mapa, altura, largura, x1, y1, x2, y2, true)) return;
    //caso necessario faz por caminhos ja gerados
    if (conectar_bfs(mapa, altura, largura, x1, y1, x2, y2, false)) return;
    //logica que vai gerando o caminho um caminho para os dois pontos
    int x = x1, y = y1;
    while (x != x2 || y != y2) {
        if (x != x2 && (y == y2 || rand() % 2)){
            x += (x < x2) ? 1 : -1;
        } else if (y != y2){
            y += (y < y2) ? 1 : -1;
        }
        if (mapa[x][y] == 1){
            mapa[x][y] = 0;
        }
    }
}

int** gerar_mapa(int altura, int largura, int chaves, unsigned int seed){
    if (altura <= 0 || largura <= 0 || chaves < 0 || chaves + 2 > altura * largura) {
        puts("Dimensoes ou quantidade de chaves invalidas para geracao do mapa.");
        return NULL;
    }

    //gera uma seed aleatoria para garantir mapas novos a cada execucao
    srand(seed);
    int **mapa = malloc(altura * sizeof(int*));
    if (!mapa) return NULL;
    for (int i = 0; i < altura; i++) {
        mapa[i] = malloc(largura * sizeof(int));
        if (!mapa[i]) {
            for (int k = 0; k < i; k++) free(mapa[k]);
            free(mapa);
            return NULL;
        }
    }
    //inicializa o mapa so com paredes
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            mapa[i][j] = 1;
        }
    }
    //gera posicoes aleatorias para x chaves, 1 aventureiro e 1 bau
    int total_pontos = chaves + 2;
    pos* localiza = malloc(total_pontos * sizeof(pos));
    if (!localiza) {
        limpar_mapa(mapa, altura, largura);
        return NULL;
    }

    int verificacao = 0;
    while (verificacao < total_pontos) {
        int x = rand() % altura;
        int y = rand() % largura;
        if (!ponto_existe(localiza, verificacao, x, y)){
            //Logica para evitar que bau caia proximo ao inicio
            if (verificacao == total_pontos - 1 && chaves > 0) {
                pos p_inicio = localiza[0];
                pos p_bau = {x, y};
                if (calcular_distancia(p_inicio, p_bau) <= 1 && total_pontos < altura * largura){
                    continue;
                }
            }
            localiza[verificacao] = (pos){x, y};
            verificacao++;
        }
    }
    //Posiciona os elementos no mapa na ordem sorteada ex: aventureiro e o localiza[0] 
    //mapa[localiza[0].x][lovaliza[0].y] = 2 2 sendo o equivalente para aventureiro
    mapa[localiza[0].x][localiza[0].y] = 2;
    for (int i = 1; i <= chaves; i++) {
        mapa[localiza[i].x][localiza[i].y] = 3;
    }
    mapa[localiza[total_pontos - 1].x][localiza[total_pontos - 1].y] = 4;
    //Conecta os pontos gerados aleatoriamente
    for (int i = 1; i < total_pontos; i++) {
        gerar_caminho(mapa, altura, largura, localiza[i - 1].x, localiza[i - 1].y, localiza[i].x, localiza[i].y, seed + (unsigned int)i);
    }
    //Adiciona caminhos extras
    int caminhos_extras = (altura * largura) / 5;
    for (int i = 0; i < caminhos_extras; i++) {
        int cx = rand() % altura;
        int cy = rand() % largura;
        //Abre paredes validas que nao estejam ao redor do bau
        if (mapa[cx][cy] == 1) {
            pos p_extra = {cx, cy};
            if (calcular_distancia(p_extra, localiza[total_pontos - 1]) > 1 || chaves == 0) {
                mapa[cx][cy] = 0;
            }
        }
    }
    imprime_mapa(mapa, altura, largura);
    free(localiza);
    return mapa;
}