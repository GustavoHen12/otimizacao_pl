#include <stdio.h>
#include <stdlib.h>
#include "data_strucs.h"

int **new_matrix(int h, int w){
    int **m = malloc(h*sizeof(int*));
    if (!m) return NULL;

    m[0] = calloc(h*w, sizeof(int));
    for (int i = 1; i < h; ++i)
        m[i] = m[0] + i*w;
    return m;
}

void free_matrix(int **m){
    if (m) {
        if (m[0]) free(m[0]);
        free(m);
    }
}

void print_matrix(int **m, int h, int w){
    for (int i = 0; i < h; ++i){
        printf("%d", m[i][0]);
        for (int j = 1; j < w; ++j){
            printf(" %d", m[i][j]);
        }
        printf("\n");
    }
}

int *new_array(int n){
    int *a = calloc(n, sizeof(int));
    return a;
}

void free_array(int *a){
    if (a) free(a);
}

void print_array(int *a, int n){
    printf("%d", a[0]);
    for (int i = 1; i < n; ++i){
        printf(" %d", a[i]);
    }
    printf("\n");
}


int queue_size (queue_t *queue){
    if (!queue) 
        return 0;

    queue_t *ptr = queue->next;
    int size = 1;
    while (ptr != queue){
        ptr = ptr->next;
        size++;
    }
    
    return size;
}

void queue_print (char *name, queue_t *queue, void print_elem (void*) ){
    printf("%s [", name); // imprime inicio da fila

    if (!queue){ // fila vazia, imprime final e retorna.
        printf("]\n");
        return;
    }

    // imprime primeiro elemento sem espaço
    queue_t *it = queue;
    print_elem((void*) it);
    it = it->next;
    // imprime elementos restantes com espaço
    while(it != queue) {
        printf(" ");
        print_elem((void*) it);
        it = it->next;
    };
    // imprime final da fila
    printf("]\n");
}

/*
    -1: Elemento não existe
    -2: Elemento em outra fila
*/
int queue_append (queue_t **queue, queue_t *elem){
    if (!elem)
        return -1;

    if (elem->next || elem->prev)
        return -2;
    
    if (!(*queue)){
        elem->next = elem;
        elem->prev = elem;
        *queue = elem;
    } else {
        queue_t *last = (*queue)->prev;
        // ajusta ponteiros com ultimo elemento
        last->next = elem;
        elem->prev = last;
        // ajusta ponteiros com primeiro elemento
        elem->next = *queue;
        (*queue)->prev = elem;
    }

    return 0;
}

// -1: a fila esta vazia
// -2: o elemento não existe
// -3: o elemento não pertence a fila
// Retorno: 0 se sucesso, <0 se ocorreu algum erro
int queue_remove (queue_t **queue, queue_t *elem){
    if (!(*queue))
        return -1;

    if (!elem)
        return -2;

    // procura na fila começando do "segundo" elemento
    queue_t *it = (*queue)->next;
    while (it != elem && it != *queue){
        it = it->next;
    }
    // chegou no inicio novamente e elemento nao esta no inicio
    if (it == *queue && it != elem)
        return -3;
    
    // elemento é a cabeça da fila
    if (elem == *queue)
        *queue = (*queue)->next;

    // apenas um elemento
    if ((*queue)->next == (*queue))
        *queue = NULL;
    
    queue_t *prev = elem->prev;
    queue_t *next = elem->next;
    prev->next = next;
    next->prev = prev;

    elem->next = NULL;
    elem->prev = NULL;

    return 0;
}

