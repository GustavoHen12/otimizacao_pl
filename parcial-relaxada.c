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

int **parcial(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **parcial);

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
  int **solucao = malloc (quant_items * sizeof (double*));
  if(solucao == NULL){
    fprintf(stderr, "Ocorreu um erro ao criar a matriz\n");
    return -1;
  }
  for (int i = 0; i < quant_items; i++)
    solucao[i] = malloc (quant_items * sizeof (double));

  // Cria vetor para representar se a viagem será ou não feita
  // viagen[i] = 1 se a viagen i sera feita ou 0 se não
  int *viagens = malloc (quant_items * sizeof (double*));
  if(solucao == NULL){
    fprintf(stderr, "Ocorreu um erro ao criar a matriz\n");
    return -1;
  }
  // ******************

  // ********* Primeira etapa *********
  //exemplo
  solucao[2][3] = 1;
  solucao[5][3] = 1;

  int k = parcial(quant_items, quant_pares_ord, capacidade_caminhao, pesos_itens, pares, solucao);
  
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

// Cria restricao:
// sum(linha) = 1
void adiciona_restricao_linha (int linha, int **solucoes) {

}

void adiciona_restricao_coluna (int linha, int **solucoes) {
  
}

void cria_funcao_objetivo (int linha, int **solucoes) {
  
}

int **parcial(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **parcial) {

}