#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <errno.h>

#ifndef USE_C11_TLOCAL
#include <pthread.h>
#endif

#include "barrier.h"

//Número de filhos que cada nó tem
#define RADIX 2 
//num de threads = pow(RADIX,DEPTH)

#define USE_C11_TLOCAL


#ifdef USE_C11_TLOCAL
//booleana local a thread, inicializada como true
_Thread_local bool mySense = true;
#else
pthread_key_t mySenseKey;
#endif

void nodeWait(Node *node)
{
#ifndef USE_C11_TLOCAL
    bool mySense = (bool)pthread_getspecific(mySenseKey);
#endif

    //critical region
    int pos = node->count--;
    if(pos == 1){
        if(node->parent != NULL){
            nodeWait(node->parent);
        }

        //critical region
        node->count = RADIX;
        node->nodeSense = mySense;
    }
    else if(pos <= 0) {
        node->count = 0;
        node->nodeSense = mySense;
    }
    else{
        while(node->nodeSense != mySense) {}
    }
}

//Construtor de nó
void buildNode(Barrier* b, Node* parent, int depth)
{
    if(depth <= 0){
        b->leaf[b->leaves] = parent;
        b->leaves++;
    }
    else{
        for(int i = 0; i < RADIX; i++){
            Node* child = malloc(sizeof(*child));
            (*child) = (Node) {
                .count = RADIX,
                .parent = parent,
                .nodeSense = false};

            buildNode(b, child, depth-1);
        }
    }
}

int barrier_init(Barrier *b, int n)
{
    //garantir que n é 2^d
    if( !( ((n != 0) && ((n & (~n + 1)) == n)) ) )
    {
        errno = EINVAL;
        return errno;
    }

#ifndef USE_C11_TLOCAL
    pthread_key_create(&mySenseKey, NULL);
#endif

    b->size = n;
    b->leaf = calloc(sizeof(*b->leaf), n);
    int height = 0;
    while(n > 1){
        height++;
        n = n/RADIX;
    }

    b->leaves = 0;

    Node* root = malloc(sizeof(*root));
    (*root) = (Node){
        .count = RADIX,
        .parent = NULL,
        .nodeSense = false};
    if(height == 0)
        root->count = 0;

    buildNode(b, root, height - 1);

    return 0;
}

void barrier_destroy(Barrier *b)
{
    for(size_t i = 0; i < b->size; i++)
    {
        if(b->leaf[i] != NULL)
        {
            free(b->leaf[i]);
        }
    }
    free(b->leaf);
}


void barrier_wait(Barrier *b, int threadid)
{
    int i = threadid/RADIX;
#ifndef USE_C11_TLOCAL
    bool initSense = !b->leaf[i]->nodeSense;
    pthread_setspecific(mySenseKey, (void*)initSense);
#endif
    nodeWait(b->leaf[i]);
#ifdef USE_C11_TLOCAL
    mySense = !mySense;
#else
    bool myNewSense = !((bool)pthread_getspecific(mySenseKey));
    pthread_setspecific(mySenseKey, (void*)(myNewSense));
#endif
}

