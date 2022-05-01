#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

void imprimeVetor(int *viagens_utilizadas, int tamanho){
  for(int i = 0; i < tamanho; i++)
    printf(" %d ", viagens_utilizadas[i]);
  printf("\n");
}

void imprimeSolucaoParcial(int **solucao, int tamanho){
  printf("\n");

  for(int i = 0; i < tamanho; i++){
    printf(" [%d] => ", i);
    imprimeVetor(solucao[i], tamanho);
  }
  printf("\n");

}

// Macro para print quando executado em modo debug
#ifdef DEBUG
#define debug_print(...) do{ printf( __VA_ARGS__ ); } while(0)
#else
#define debug_print(...) do{ } while (0)
#endif

typedef struct par_ord_t { 
  int a, b;
} par_ord_t;

#define inf 0xffffffff

// Variaveis globais para resolução
int menor_numero_viagens;
int **solucao_minima;

void le_entrada(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int **pesos_itens, par_ord_t **pares);

int branchBound(int item_para_alocar, int quant_items, int quant_pares_ord, int capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **solucao_parcial, int *viagens_utilizadas);

int main() {
  // Variavéis para leitura
  int quant_items, quant_pares_ord, capacidade_caminhao;
  int *pesos_itens;
  par_ord_t *pares;

  // Le entrada do problema
  le_entrada(&quant_items, &quant_pares_ord, &capacidade_caminhao, &pesos_itens, &pares);

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

  // ********* Inicia variaveis globais *********
  solucao_minima = malloc ((quant_items+1) * sizeof (int*));
  if(solucao_minima == NULL){
    fprintf(stderr, "Ocorreu um erro ao criar a matriz\n");
    return -1;
  }
  for (int i = 0; i < quant_items; i++)
    solucao_minima[i] = malloc (quant_items * sizeof (int));

  menor_numero_viagens = INT_MAX;

  // ********* Encontra solução *********
  branchBound(0, quant_items, quant_pares_ord, capacidade_caminhao, pesos_itens, pares, solucao, viagens);

  printf("%d\n", menor_numero_viagens);
  for(int i = 0; i < quant_items; i++){
    printf("Item: %d ", i);

    for(int j = 0; j < quant_items; j++){
      if(solucao_minima[i][j] == 1){
        printf("Viagem: %d ", j);
      }
    }

    printf("\n");
  }

  return 0;
}

void le_entrada(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int **pesos_itens, par_ord_t **pares){
  debug_print("Lendo entrada...\n");

  // Recebe a entrada do problema
  scanf("%d", quant_items);
  scanf("%d", quant_pares_ord);
  scanf("%d", capacidade_caminhao);

  // Le pesos dos itens
  *pesos_itens = malloc(sizeof(int) * (*quant_items));
  if(!*pesos_itens){
    fprintf(stderr,"Não foi possível alocar espaço para os pesos !\n");
    return;
  }

  for(int i = 0; i < *quant_items; i++){
    scanf("%d", &((*pesos_itens)[i]));
  }

  // Le pares ordenados
  *pares = malloc(sizeof(par_ord_t) * (*quant_pares_ord));
  if(! *pares){
    fprintf(stderr, "Não foi possível alocar espaço para os pares ordenados !\n");
    return;
  }

  for(int i = 0; i < *quant_pares_ord; i++){
    scanf("%d", &((*pares)[i].a));
    scanf("%d", &((*pares)[i].b));
  }

  debug_print("Fim leitura entrada !\n");
}

float parcial(int quant_items, int quant_pares_ord, int capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **solucao_parcial, int *viagens_utilizadas) {
  return 1;
}

int verificaViabilidade(int viagem, int quant_items, int capacidade_caminhao, int *pesos_itens, int **solucao_parcial) {
  int peso_total = 0;
  for(int i = 0; i < quant_items; i++){
    if(solucao_parcial[i][viagem] == 1){
      peso_total += pesos_itens[i];
    }
  }

  if(peso_total <= capacidade_caminhao) {
    return 1;
  }
  
  return 0;
}

int numeroViagensUtilizadas(int *viagens_utilizadas, int tamanho){
  int tamanho_vet = 0, i = 0;
  while(i < tamanho){
    tamanho_vet = viagens_utilizadas[i] == 1 ? tamanho_vet + 1 : tamanho_vet;
    i++;
  }
  return tamanho_vet;
}

int viagemUtilizada(int viagem, int **solucao, int tamanho){
  for(int i = 0; i < tamanho; i++){
    if(solucao[i][viagem] == 1)
      return 1;
  }

  return 0;
}

// ********* FUNCAO DE BRANCH AND BOUND *********
int branchBound(int item_para_alocar, int quant_items, int quant_pares_ord, int capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **solucao_parcial, int *viagens_utilizadas) {
  // Base da recursao, chegou em uma folha
  if(item_para_alocar == quant_items){
    int quant_viagens = numeroViagensUtilizadas(viagens_utilizadas, quant_items);
    if(quant_viagens < menor_numero_viagens){
      menor_numero_viagens = quant_viagens;
      for(int i = 0; i < quant_items; i++){
        for(int j = 0; j < quant_items; j++){
          solucao_minima[i][j] = solucao_parcial[i][j];
        }
      }
    }

    return 1;
  }

  // Calcula parcial
  int valor_parcial = parcial(quant_items, quant_pares_ord, capacidade_caminhao, pesos_itens, pares, solucao_parcial, viagens_utilizadas);

  // Se pode continuar no ramo
  if(valor_parcial < menor_numero_viagens){
    // Tenta chamar adicionar o item em cada viagem e chama recursivamente
    for(int i = 0; i < quant_items; i++) {
      solucao_parcial[item_para_alocar][i] = 1;
      viagens_utilizadas[i] = 1;

      if(verificaViabilidade(i, quant_items, capacidade_caminhao, pesos_itens, solucao_parcial)){
        branchBound(item_para_alocar+1, quant_items, quant_pares_ord, capacidade_caminhao, pesos_itens, pares, solucao_parcial, viagens_utilizadas);
      }
 
      solucao_parcial[item_para_alocar][i] = 0;
      if(!viagemUtilizada(i, solucao_parcial, quant_items)){
        viagens_utilizadas[i] = 0;
      }
    }
  }

  return 0;
}