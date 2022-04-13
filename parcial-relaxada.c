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

float parcial(int quant_items, int quant_pares_ord, int capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **solucao_parcial, int *viagens_utilizadas);

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

  // ********* Le soluções parciais *********
  int numero_solucoes_parciais;
  scanf("%d", &numero_solucoes_parciais);
  for(int i = 0; i < numero_solucoes_parciais; i++) {
    int item, viagem;
    scanf("%d", &item);
    scanf("%d", &viagem);

    solucao[item - 1][viagem - 1] = 1;
    viagens[viagem-1] = 1;
  }
  
  // ********* Encontra solução parcial *********
  // Executa funcao parcial que ira calcular o resultado da funcao objetiva
  float resultado = parcial(quant_items, quant_pares_ord, capacidade_caminhao, pesos_itens, pares, solucao, viagens);

  printf("%.2f\n", resultado);  
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

/************ FUNCOES AUXILIARES ************/

// Recebe a posição da matriz e retorna o índice da variável correpondente no lp solve
int getPosicaoVariaveisMatriz (int linha, int coluna, int tamanho) {
  return linha * tamanho + coluna + 1; // O +1 é pq o lp solve indexa as variaveis a partir de 1
}

// Recebe a posição do vetor e retorna o índice da variável correpondente no lp solve
int getPosicaoVariaveisVetor (int index, int tamanho) {
  return index + (tamanho*tamanho) + 1 ; // O +1 é pq o lp solve indexa as variaveis a partir de 1
}

// Adiciona a restrição: Os itens devem ser enviados completamente
// Portanto a soma da porcentagem enviada por item deve ser igual a 1
void adiciona_restricao_linha (int index_linha, int quant_itens, int quant_variaveis, lprec *lp) {
  int *index_col = NULL;
  REAL *linha = NULL;
  index_col = (int *) malloc(quant_variaveis * sizeof(*index_col)); // Indice das variaveis
  linha = (REAL *) malloc(quant_variaveis * sizeof(*linha)); // Multiplicador das variaveis
  
  // Percorre a linha da matriz (total de viagens para um item)
  for(int i = 0; i < quant_itens; i++) {
    // Pega o indice da variavel correspondente a posicao da matriz
    index_col[i] = getPosicaoVariaveisMatriz(index_linha, i, quant_itens);
    linha[i] = 1; // 1*M_[index_linha]_[i]
  }

  // Adiciona restricao no lp_solve
  if(!add_constraintex(lp, quant_itens, linha, index_col, EQ, 1)) {
    fprintf(stderr, "Não foi possível adicionar a restrição!\n");
  }
}

// Adiciona a restrição: A soma dos pesos da carga em uma viagem deve ser menor que o
// o peso máximo que o caminhão suporta
void adiciona_restricao_coluna (int *pesos, int index_coluna, int quant_itens, int quant_variaveis, int capacidade_caminhao, lprec *lp) {
  int *index_col = NULL;
  REAL *linha = NULL;

  index_col = (int *) malloc(quant_variaveis * sizeof(*index_col));
  linha = (REAL *) malloc(quant_variaveis * sizeof(*linha));

  // Percorre a coluna da matriz (total de itens em uma viagem)
  for(int i = 0; i < quant_itens; i++) {
    index_col[i] = getPosicaoVariaveisMatriz(i, index_coluna, quant_itens);
    linha[i] = pesos[i];
  }

  // Adiciona a subtracao da constante do peso da carga na restricao
  index_col[quant_itens] = getPosicaoVariaveisVetor(index_coluna, quant_itens);
  linha[quant_itens] = -1 * capacidade_caminhao;

  // Adiciona restricao no lp_solve
  if(!add_constraintex(lp, quant_itens + 1, linha, index_col, LE, 0)) {
    fprintf(stderr, "Não foi possível adicionar a restrição!\n");
  }
}

// Cria restricao com uma variavel
// Portanto cria restricao: [index_var] [op] [limite] 
// Exemplo: V_0 = 1
void adiciona_valor_restricao (int index_var, int limite, int op,  lprec *lp) {
  int *index_col = NULL;
  REAL *linha = NULL;

  index_col = (int *) malloc(2 * sizeof(*index_col));
  linha = (REAL *) malloc(2 * sizeof(*linha));

  index_col[0] = index_var;
  linha[0] = 1;

  if(!add_constraintex(lp, 1, linha, index_col, op, limite)) {
    fprintf(stderr, "Não foi possível adicionar a restrição!\n");
  }
}

void adiciona_restricao_par_ordenada(int a, int b, int quant_itens, int quant_variaveis, lprec *lp) {
  int index_a = a-1;
  int index_b = b-1;
  debug_print ("a: %d b: %d\n", a, b);
  int *index_col = NULL;
  REAL *linha = NULL;

  int *index_col_rest_sum = NULL;
  REAL *linha_rest_sum = NULL;

  index_col = (int *) malloc(quant_variaveis * sizeof(*index_col));
  linha = (REAL *) malloc(quant_variaveis * sizeof(*linha));
  int tamanho = 0;

  // Adiciona restrição (3)
  // Garante que, para um par ordenado (a, b), v(a) <= v(b)
  for(int p = 0; p < quant_itens; p++) {
    index_col = (int *) malloc(quant_variaveis * sizeof(*index_col));
    linha = (REAL *) malloc(quant_variaveis * sizeof(*linha));
    tamanho = 0;

    for(int j = 0; j < p; j++) {
      index_col[tamanho] = getPosicaoVariaveisMatriz(index_a, j, quant_itens);
      linha[tamanho] = -1;
      tamanho++;

      index_col[tamanho] = getPosicaoVariaveisMatriz(index_b, j, quant_itens);
      linha[tamanho] = +1;
      tamanho++;
    }

    // Adiciona restricao no lp_solve
    if(!add_constraintex(lp, tamanho, linha, index_col, LE, 0)) {
      fprintf(stderr, "Não foi possível adicionar a restrição!\n");
    }
  }

  // Adiciona restrição (4)
  for(int i = 0; i < quant_itens; i++){
    index_col_rest_sum = (int *) malloc(2 * sizeof(*index_col_rest_sum));
    linha_rest_sum = (REAL *) malloc(2 * sizeof(*linha_rest_sum));

    index_col_rest_sum[0] = getPosicaoVariaveisMatriz(index_a, i, quant_itens);
    linha_rest_sum[0] = 1;
    index_col_rest_sum[1] = getPosicaoVariaveisMatriz(index_b, i, quant_itens);
    linha_rest_sum[1] = 1;

    if(!add_constraintex(lp, 2, linha_rest_sum, index_col_rest_sum, LE, 1)) {
      fprintf(stderr, "Não foi possível adicionar a restrição!\n");
    }
  }
}

// Cria funcao objetivo do modelo
void cria_funcao_objetivo (int quant_itens, int numero_colunas, lprec *lp) {
  int *index_col = NULL;
  REAL *linha = NULL;

  index_col = (int *) malloc(numero_colunas * sizeof(*index_col));
  linha = (REAL *) malloc(numero_colunas * sizeof(*linha));

  for(int i = 0; i < quant_itens; i++){
    index_col[i] = getPosicaoVariaveisVetor(i, quant_itens);
    linha[i] = 1;
  }

  if(!set_obj_fnex(lp, quant_itens, linha, index_col)) {
    fprintf(stderr, "Não foi possível construir o modelo\n");
  }
}

// ********* FUNCAO PARCIAL *********

float parcial(int quant_items, int quant_pares_ord, int capacidade_caminhao, int *pesos_itens, par_ord_t *pares, int **solucao_parcial, int *viagens_utilizadas) {
  // Cria variaveis para criar modelo do lp solve
  lprec *lp;
  int numero_colunas;
  int *index_col = NULL;
  REAL *linha = NULL;

  // ********* Inicia variaveis *********
  // Calcula o total de variaveis necessárias
  numero_colunas = (quant_items*quant_items) + quant_items; // |Matriz| + |vetor com viagens|
  
  // Cria modelo do lp_solve para o total de variaveis
  lp = make_lp(0, numero_colunas);
  if(lp == NULL){
    fprintf(stderr, "Não foi possível construir o modelo\n");
    return -1;
  }

  // Variaveis para representar linha da restricao
  index_col = (int *) malloc(numero_colunas * sizeof(*index_col));
  linha = (REAL *) malloc(numero_colunas * sizeof(*linha));
  if(!index_col || !linha) {
    fprintf(stderr, "Não foi possível alocar espaço para restrições\n");
    return -1;
  }


  // ********* Cria váriaveis no lp solve *********
  char nome_variavel[20];

  // Cria variaveis para representar a matriz de solucoes
  for(int i = 0; i < quant_items; i++){
    for(int j = 0; j < quant_items; j++) {
        sprintf(nome_variavel, "M_%d_%d", i+1, j+1);
        set_col_name(lp, getPosicaoVariaveisMatriz(i, j, quant_items), nome_variavel);
    }
  }

  // Cria variaveis para representar o vetor de viagens
  for(int i = (quant_items*quant_items) + 1; i <= numero_colunas; i++){
    // int index = i - (quant_items*quant_items) - 1;
    sprintf(nome_variavel, "Y_%d", (i - (quant_items*quant_items)));
    set_col_name(lp, i, nome_variavel);
  }

  debug_print("\nNomes de variáveis criados !\n");

  // ********* Cria restricoes *********

  // Coloca o lp solve no modo de retricoes
  set_add_rowmode(lp, TRUE);

  for(int i = 0; i < quant_items; i++) {
    // Cria restrições de linha da matriz
    adiciona_restricao_linha(i, quant_items, numero_colunas, lp);
    
    // Cria restrições de coluna da matriz
    adiciona_restricao_coluna(pesos_itens, i, quant_items, numero_colunas, capacidade_caminhao, lp);
  }

  // Adiciona restricao para pares ordenados
  for(int i = 0; i < quant_pares_ord; i++) {
    adiciona_restricao_par_ordenada(pares[i].a, pares[i].b, quant_items, numero_colunas, lp);
  }

  // Adiciona restricao de valor
  for(int i = 0; i < quant_items; i++){
    // verifica vetor de viagens
    if(viagens_utilizadas[i] != 0) {
      // variavel = viagens_utilizadas[i]
      adiciona_valor_restricao(getPosicaoVariaveisVetor(i, quant_items), viagens_utilizadas[i], EQ, lp);
    } else {
      // 0 <= variaval <= 1
      adiciona_valor_restricao(getPosicaoVariaveisVetor(i, quant_items), 0, GE, lp);
      adiciona_valor_restricao(getPosicaoVariaveisVetor(i, quant_items), 1, LE, lp);
    }

    // Verifica matriz de solucoes parciais
    for(int j = 0; j < quant_items; j++){
      if(solucao_parcial[i][j] != 0) {
        adiciona_valor_restricao(getPosicaoVariaveisMatriz(i, j, quant_items), solucao_parcial[i][j], EQ, lp);
      } else {
        adiciona_valor_restricao(getPosicaoVariaveisMatriz(i, j, quant_items), 0, GE, lp);
        adiciona_valor_restricao(getPosicaoVariaveisMatriz(i, j, quant_items), 1, LE, lp);
      }
    }
  }
  // Sai do modo de adicionar restricoes
  set_add_rowmode(lp, FALSE);

  // ********* Cria função objetivo *********

  cria_funcao_objetivo(quant_items, numero_colunas, lp);
  // Configura função objetivo como minizacao
  set_minim(lp);
  
  #ifdef DEBUG
  printf("\n\n"); 
  write_LP(lp, stdout);
  #endif
  
  /* Calcula solução */
  // Exibir apenas as mensagens importantes durante a resoluca
  set_verbose(lp, IMPORTANT);
  // Resolve
  solve(lp);

  // Recebe valor da funcao objetivo
  float resultado = get_objective(lp);

  // No modo debug imprime os valores das variaveis
  debug_print("\nResultado: %f\n", resultado);
  get_variables(lp, linha);
  for(int i = 0; i < numero_colunas; i++)
    debug_print("%s: %f\n", get_col_name(lp, i + 1), linha[i]);
  
  return resultado;
}