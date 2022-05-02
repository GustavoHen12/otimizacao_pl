#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "data_strucs.h"

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

// Fila de itens
// Será usada como lista de adjacência do grafo que representa as ordens.
typedef struct queue_items {
    struct queue_items *prev; 
    struct queue_items *next;
    int item;
} queue_items_t;

// Cria novo elemento da fila
queue_items_t *new_element(int val){
    queue_items_t *elem = (queue_items_t *) malloc(sizeof(queue_items_t));
    if (!elem) return NULL;

    elem->item = val;
    elem->next = NULL;
    elem->prev = NULL;
    return elem;
}

// Retira primeiro elemento da fila de itens e retorna seu valor
int queue_items_pop(queue_items_t **qi){
    queue_items_t *cur =  *qi;
    int ret = cur->item;
    queue_remove((queue_t **) qi, (queue_t *) cur);
    if (cur) free(cur);
    return ret;
}

// Aloca e insere elemento no final da fila.
int queue_items_push(queue_items_t **qi, int i){
    queue_items_t *elem = new_element(i);
    if (!elem) return 0;
    queue_append((queue_t **) qi, (queue_t*) elem);
    return 1;
}

// Imprime elemento da fila (para debug)
void print_elem(void *elem){
    queue_items_t *_elem = elem;
    printf("%d", _elem->item);
}

// Lê dois inteiros e insere no grafo de ordens
void ler_ordem(queue_items_t **ordens){
    int a, b;
    scanf("%d %d", &a, &b);
    queue_items_t *elem_b = new_element(b);
    queue_append((queue_t **) &(ordens[a]), (queue_t *) elem_b);
}

// Parte do elemento 'a' e procura o elemento 'b' por busca em largura no grafo ordem
// Retorna 0 se não achou e 1 se achou
int bfs(queue_items_t *ordem, int n, int a, int b){
    int ret = 0;
    queue_items_t *items = NULL;
    int *visited = new_array(n);
    queue_items_push(&items, a);

    while(queue_size((queue_t *) items)){
        #ifdef DEBUG
        queue_print("Queue BFS", (queue_t *) items, print_elem);
        #endif 
        int cur = queue_items_pop(&items);
        if (cur == b) ret = 1;

        queue_items_t *it = &(ordem[cur]);
        if (!it) continue;
        do {
            if (visited[it->item] == 0){
                queue_items_push(&items, it->item);
                visited[it->item] = 1;
            }
            it = it->next;
        } while (it != &(ordem[cur]));
    }
    return ret;
}

// viagens é o vetor que diz em qual viagem está cada item
// EX: viagens[3] == 2 quer dizer que o item 3 vai na viagem 2
// A função viavel testa se o item 'item' está posicionado de
// forma viável em relação aos outros
int viavel(int *viagens, int n, int item, int **mat_ordem){
    // TODO: testar cargas do caminhão
    int ret = 1;
    for (int i = 1; i <= n; ++i){
        if (!viagens[i] || i == item) continue;
        if (viagens[i] <= viagens[item] && mat_ordem[item][i] || 
            viagens[i] >= viagens[item] && mat_ordem[i][item]    )
            ret = 0;
    }
    return ret;
}

void gera_matriz_ordem(queue_items_t *ordens, int **matriz_ordem, int n){
    for (int i = 1; i < n; ++i){
        for (int j = 1; j < n; ++j){
            if (i != j) matriz_ordem[i][j] = bfs(ordens, n, i, j);
        }
    }
}

// Variaveis globais para resolução
int menor_numero_viagens;
int **solucao_minima;

void le_entrada(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int **pesos_itens, queue_items_t ***ordens);

int branchBound(int item_para_alocar, int quant_items, int capacidade_caminhao, int *pesos_itens, int **mat_ordem, int **solucao_parcial, int *viagens_utilizadas);

int main() {
  // Variavéis para leitura
  int quant_items, quant_pares_ord, capacidade_caminhao;
  int *pesos_itens;
  queue_items_t **ordens;

  // Le entrada do problema
  le_entrada(&quant_items, &quant_pares_ord, &capacidade_caminhao, &pesos_itens, &ordens);

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

  int **mat_ordem = new_matrix(quant_items+1, quant_items+1);
  gera_matriz_ordem(ordens, mat_ordem, quant_items);

  // ********* Encontra solução *********
  branchBound(0, quant_items, capacidade_caminhao, pesos_itens, mat_ordem, solucao, viagens);

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

void le_entrada(int *quant_items, int *quant_pares_ord, int *capacidade_caminhao, int **pesos_itens, queue_items_t ***ordens){
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
  *ordens = (queue_items_t **) malloc((quant_items+1) * sizeof(queue_items_t *));
  if(! *ordens){
    fprintf(stderr, "Não foi possível alocar espaço para os pares ordenados !\n");
    return;
  }

  for (int i = 0; i < quant_pares_ord; ++i)
      ler_ordem(ordens);

  debug_print("Fim leitura entrada !\n");
}

float parcial(int quant_items, int capacidade_caminhao, int *pesos_itens, int **mat_ordem, int **solucao_parcial, int *viagens_utilizadas) {
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
int branchBound(int item_para_alocar, int quant_items, int capacidade_caminhao, int *pesos_itens, int **mat_ordem, int **solucao_parcial, int *viagens_utilizadas) {
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
  int valor_parcial = parcial(quant_items, capacidade_caminhao, pesos_itens, mat_ordem, solucao_parcial, viagens_utilizadas);

  // Se pode continuar no ramo
  if(valor_parcial < menor_numero_viagens){
    // Tenta chamar adicionar o item em cada viagem e chama recursivamente
    for(int i = 0; i < quant_items; i++) {
      solucao_parcial[item_para_alocar][i] = 1;
      viagens_utilizadas[i] = 1;

      if(verificaViabilidade(i, quant_items, capacidade_caminhao, pesos_itens, solucao_parcial)){
        branchBound(item_para_alocar+1, quant_items, capacidade_caminhao, pesos_itens, mat_ordem, solucao_parcial, viagens_utilizadas);
      }
 
      solucao_parcial[item_para_alocar][i] = 0;
      if(!viagemUtilizada(i, solucao_parcial, quant_items)){
        viagens_utilizadas[i] = 0;
      }
    }
  }

  return 0;
}