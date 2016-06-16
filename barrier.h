#if !defined(__BARRIER_H__) && defined(THREADED)
#define __BARRIER_H__

#include <stdbool.h>
#include <stdatomic.h>

typedef struct node {
    struct node *parent;
    atomic_int count;
    volatile bool nodeSense;
} Node;

//Todos os nós da barreira
typedef struct barrier {
    Node *(*leaf);
    size_t size;
    int leaves;
} Barrier;

int barrier_init(Barrier *b, int n);
void barrier_destroy(Barrier *b);
void barrier_wait(Barrier *b, int threadid);

#endif

