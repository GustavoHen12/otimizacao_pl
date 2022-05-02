#include <stdio.h>
#include <stdlib.h>
#include "data_strucs.h"

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

int parcial(int n, int p, int c, int *w, int **mat_ordem, int **mat_parcial, int *V){
    int *Vaux = (int *) calloc(n, sizeof(int));
    int *cargas = (int *) calloc(n, sizeof(int));
    for (int i = 1; i < n; ++i){
        if (V[i]) cargas[V[i]] += w[i];
        Vaux[i] = V[i];
    } 

    for (int i = 1; i < n; ++i){
        // vou pro primeiro caminhão que da pra colocar
        // coloco o que da nesse e nos proximos
    }
}

void gera_matriz_ordem(queue_items_t *ordens, int **matriz_ordem, int n){
    for (int i = 1; i < n; ++i){
        for (int j = 1; j < n; ++j){
            if (i != j) matriz_ordem[i][j] = bfs(ordens, n, i, j);
        }
    }
}

int main(){
    int n, p, c;
    int **trips, **mat_ordem;
    int *w, *viagens;

    // Leitura 
    scanf("%d %d %d", &n, &p, &c);

    // Alocação
    trips = new_matrix(n+1, n+1);
    mat_ordem = new_matrix(n+1, n+1);
    w = new_array(n);
    viagens = new_array(n);
    queue_items_t **ordens = (queue_items_t **) malloc((n+1)*sizeof(queue_items_t *));
    for (int i = 0; i <= n; ++i){
        ordens[i] = NULL;
    }

    // Leitura
    for (int i = 0; i < n; ++i)
        scanf("%d", w+i);
    for (int i = 0; i < p; ++i)
        ler_ordem(ordens);
    gera_matriz_ordem(ordens, mat_ordem, n+1);

    #ifdef DEBUG
    for (int i = 1; i <= n; ++i){
        printf("Elemento %d\n", i);
        queue_print("Next", (queue_t *) ordens[i].depois, print_elem);
        printf("\n");
    }
    #endif

    

    for (int i = 0; i <= n; ++i)
        while( queue_size((queue_t *) ordens[i]) )
            queue_items_pop(&ordens[i]);
    free(ordens);
    free_array(w);
    free_matrix(trips);
}
