#include <stdio.h>
#include <stdlib.h>

#include "lp_lib.h"

// Macro para print quando executado em modo debug
#ifdef DEBUG
#define debug_print(...) do{ printf( __VA_ARGS__ ); } while(0)
#else
#define debug_print(...) do{ } while (0)
#endif

typedef struct par_ord_t {
  int a, b;
} par_ord_t;


void le_entrada(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int **pesos_itens, par_ord_t **pares);

int **parcial(int quant_items, int quant_pares_ord, int capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **solucao_parcial, int *viagens_utilizadas);


int main() {
  // Variavéis para leitura
  int quant_items, quant_pares_ord, capacidade_caminhao;
  int *pesos_itens;
  par_ord_t *pares;

  // Representacao das restricoes
  int **matriz_result;

  le_entrada(&quant_items, &quant_pares_ord, &capacidade_caminhao, &pesos_itens, &pares);

  // **REMOVER: prints para debug**
  debug_print("%d, %d, %d\n", quant_items, quant_pares_ord, capacidade_caminhao);
  for(int i = 0; i < quant_items; i++){
    debug_print("%d ", pesos_itens[i]);
  }
  debug_print("\n");
  for(int i = 0; i < quant_pares_ord; i++){
    debug_print("a = %d, ", pares[i].a);
    debug_print("b = %d\n", pares[i].b);
  }
  // ****

  // ********* Variaveis para representacao do problema *********
  // Cria matriz para associar v(i), ou seja,
  // solucao[i][j] = 1 se o item i vai na viagem j
  int **solucao = malloc ((quant_items+1) * sizeof (double*));
  if(solucao == NULL){
    fprintf(stderr, "Ocorreu um erro ao criar a matriz\n");
    return -1;
  }
  for (int i = 0; i < quant_items; i++)
    solucao[i] = malloc (quant_items * sizeof (double));

  // Cria vetor para representar se a viagem será ou não feita
  // viagen[i] = 1 se a viagen i sera feita ou 0 se não
  int *viagens = malloc (quant_items * sizeof (double*));
  if(viagens == NULL){
    fprintf(stderr, "Ocorreu um erro ao criar a matriz\n");
    return -1;
  }
  for (int i = 0; i < quant_items; i++)
    viagens[i] = 0;

  // ******************

  // ********* Primeira etapa *********
  //exemplo
  solucao[2][1] = 1;
  solucao[4][2] = 1;

  viagens[1] = 1;
  viagens[2] = 1;
  
  parcial(quant_items, quant_pares_ord, capacidade_caminhao, pesos_itens, pares, solucao, viagens);
  
  // ******************
  return 0;
}

void le_entrada(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int **pesos_itens, par_ord_t **pares){
  debug_print("Lendo entrada...\n");

  // Recebe a entrada do problema
  scanf("%d", quant_items);
  scanf("%d", quant_pares_ord);
  scanf("%d", capacidade_caminhao);

  // Inicia pesos
  *pesos_itens = malloc(sizeof(int) * (*quant_items));
  if(!*pesos_itens){
    perror("Não foi possível alocar espaço para os pesos !\n");
    return;
  }

  for(int i = 0; i < *quant_items; i++){
    scanf("%d", &((*pesos_itens)[i]));
  }

  // inicia pares ordenados
  *pares = malloc(sizeof(par_ord_t) * (*quant_pares_ord));
  if(! *pares){
    perror("Não foi possível alocar espaço para os pares ordenados !\n");
    return;
  }

  for(int i = 0; i < *quant_pares_ord; i++){
    scanf("%d", &((*pares)[i].a));
    scanf("%d", &((*pares)[i].b));
  }

  debug_print("Fim leitura entrada !\n");
}

int getPosicaoColuna (int linha, int coluna, int tamanho) {
  return linha * tamanho + coluna + 1; // O mais 1 é pq o lp solve indexa a partir de 1 aparentemente
}

// Cria restricao:
// sum(linha) = 1
void adiciona_restricao_linha (int index_linha, int quant_items, int numero_colunas, int **solucao_parcial, lprec *lp) {
  int *index_col = NULL;
  REAL *linha = NULL;

  index_col = (int *) malloc(numero_colunas * sizeof(*index_col)); // ????
  linha = (REAL *) malloc(numero_colunas * sizeof(*linha));
  int porcentegem_nao_enviada = 1;

  for(int i = 0; i < quant_items; i++) {
    if(solucao_parcial[index_linha][i] <= 0){
      index_col[i] = getPosicaoColuna(index_linha, i, quant_items);
      linha[i] = 1;

      if(i>0) debug_print(" + ");
      debug_print("%s [%d]", get_col_name(lp, getPosicaoColuna(index_linha, i, quant_items)), getPosicaoColuna(index_linha, i, quant_items));
    } else {
      porcentegem_nao_enviada -= solucao_parcial[index_linha][i];
      debug_print("           ");
      // index_col[i] = getPosicaoColuna(index_linha, i, quant_items);
      // linha[i] = 0; ???????
    }
  }

  debug_print(" <= %d\n", porcentegem_nao_enviada);
  if(!add_constraintex(lp, quant_items, linha, index_col, LE, porcentegem_nao_enviada)) {
    fprintf(stderr, "Não foi possível construir o modelo\n");
  }
}

void adiciona_restricao_coluna (int *pesos, int coluna, int quant_items, int numero_colunas, int capacidade_caminhao, int **solucao_parcial, int *viagens_utilizadas, lprec *lp) {
  int *index_col = NULL;
  REAL *linha = NULL;

  index_col = (int *) malloc(numero_colunas * sizeof(*index_col)); // ????
  linha = (REAL *) malloc(numero_colunas * sizeof(*linha));
  int peso_restante = capacidade_caminhao;

  for(int i = 0; i < quant_items; i++) {
    if(solucao_parcial[i][coluna] <= 0){
      index_col[i] = getPosicaoColuna(i, coluna, quant_items);
      linha[i] = pesos[i];

      if(i>0) debug_print(" + ");
      debug_print("%s [%d]", get_col_name(lp, getPosicaoColuna(i, coluna, quant_items)), getPosicaoColuna(i, coluna, quant_items));
    } else {
      debug_print("           ");
      peso_restante -= solucao_parcial[i][coluna] * pesos[i];
    }
  }

    debug_print(" <= %d\n", peso_restante);

  if(!add_constraintex(lp, quant_items, linha, index_col, LE, peso_restante)) {
    fprintf(stderr, "Não foi possível construir o modelo\n");
  }
}

void cria_funcao_objetivo (int linha, int **solucoes) {
  
}

int **parcial(int quant_items, int quant_pares_ord, int capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **solucao_parcial, int *viagens_utilizadas) {

  lprec *lp;
  int numero_colunas;
  int *index_col = NULL;
  REAL *linha = NULL;

  // Inicia variáveis
  numero_colunas = (quant_items*quant_items) + quant_items + 1; // Matriz + vetor com viagens
  lp = make_lp(0, numero_colunas);

  if(lp == NULL){
    fprintf(stderr, "Não foi possível construir o modelo\n");
    return NULL;
  }

  index_col = (int *) malloc(numero_colunas * sizeof(*index_col)); // ????
  linha = (REAL *) malloc(numero_colunas * sizeof(*linha));

  if(!index_col || !linha) {
    fprintf(stderr, "Não foi possível alocar espaço para restrições\n");
    return NULL;
  }
  debug_print("numero_colunas: %d, quant_items: %d \n", numero_colunas, quant_items);

  // Cria váriaveis no lp solve
  debug_print("Criando nomes para variáveis: \n");
  char nome_variavel[20];
  for(int i = 0; i < quant_items; i++){
    for(int j = 0; j < quant_items; j++) {
      if(solucao_parcial[i][j] <= 0){
        sprintf(nome_variavel, "M_%d_%d", i, j);
        debug_print("%s[%d] ", nome_variavel, getPosicaoColuna(i, j, quant_items));
        set_col_name(lp, getPosicaoColuna(i, j, quant_items), nome_variavel);
      } else {
        debug_print("***** ");
      }
    }
    debug_print("\n");
  }

  debug_print("\n");

  for(int i = (quant_items*quant_items) + 1; i < numero_colunas; i++){
    int index = i - (quant_items*quant_items) - 1;
    if(viagens_utilizadas[index] != 1){
      sprintf(nome_variavel, "V_%d", (i - (quant_items*quant_items) - 1));
      debug_print("%s ", nome_variavel);
      set_col_name(lp, i, nome_variavel);
    } else {
      debug_print("*** ");
    }
  }

  debug_print("\nNomes de variáveis criados !\n");


  // Cria restrições de linha
  set_add_rowmode(lp, TRUE);
  debug_print("Criando restrições...\n");
  debug_print("Criando restrição para linha da matriz: \n");
  for(int i = 0; i < quant_items; i++) {
    adiciona_restricao_linha(i, quant_items, numero_colunas, solucao_parcial, lp);
  }
  
  // Cria restrições de coluna
  debug_print("Criando restrição para coluna da matriz: \n");
  for(int i = 0; i < quant_items; i++) {
    adiciona_restricao_coluna(pesos_itens, i, quant_items, numero_colunas, capacidade_caminhao, solucao_parcial, viagens_utilizadas, lp);
  }

  // TODO: Cria restrição de valor

  set_add_rowmode(lp, FALSE);

  // Adiciona função objetivo

  return NULL;
}