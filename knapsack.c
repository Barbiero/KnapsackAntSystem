/////////////
//Knapsack definitions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "knapsack.h"
#include "item.h"
#include "util.h"

//default values for a knapsack
struct Knapsack k_init = {
    .has_item = NULL, 
    .capacity = {3000.0, 10000.0},
    .worth = (Cost) 0,
    .num_items = (size_t)0
};

/**
 * Sets the memory of k to that of a default knapsack
 */
void Knapsack_init(struct Knapsack *k)
{
    assert(k != NULL);
    memcpy(k, &k_init, sizeof(*k));

    k->has_item = calloc(sizeof(bool), NUM_ITEMS);
}

void Knapsack_destroy(struct Knapsack *k)
{
    assert(k != NULL);
    free(k->has_item);
}

bool Knapsack_canAddItem(struct Knapsack *k, ItemId i)
{
    if(k->has_item[i]) return false;

    //struct Item *item = &universe[i];
    
    for(RestrId c = 0; c < NUM_RESTRICTIONS; c++)
    {
        if(k->capacity[c] < universe[i].restrictions[c]){
            return false;
        }
    }

   return true;
}

void Knapsack_addItem(struct Knapsack *k, ItemId itemid)
{
    assert(itemid < NUM_ITEMS);
    
    for(RestrId c = 0; c < NUM_RESTRICTIONS; c++)
    {
        k->capacity[c] -= universe[itemid].restrictions[c];
    }
    k->has_item[itemid] = true;

    k->worth += universe[itemid].value;
    k->num_items++;
}

struct Neighbour *createNeighbour(struct Knapsack *k)
{
    //iterate over all items and add to the neighbourhood
    //only the items that can be added to the knapsack k
    //allocate only as much space as necessary, multiplying by 2 each time


    struct Neighbour *n = malloc(sizeof(*n));

    n->items = malloc(sizeof(*(n->items)));
    n->size = 0;
    n->cap = 1; //neighbour array capacity

    size_t j = 0; //neighbour iterator

    PherDes sum = 0.0;

    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        if(!Knapsack_canAddItem(k, i)){
            continue;
        }

        if(j >= n->cap)
        {
            n->cap *= 2;
            n->items = realloc(n->items, sizeof(*(n->items)) * n->cap);
        }

        n->items[j] = (struct K_item_prob){
            .itemid = i,
            .prob = universe[i].pdValue
        };
        sum += universe[i].pdValue;

        n->size++;


        j++;
    }

    //normalize probabilities
    for(size_t i = 0; i < n->size; i++)
    {
        n->items[i].prob = (Prob)(n->items[i].prob/sum);
    }

    return n;
}

void deleteNeighbour(struct Neighbour *n)
{
    free(n->items);
    free(n);
}


int32_t Neighbour_search(struct Neighbour *n, double search_val)
{
    /* Sequential Search */
    
    for(size_t i = 0; i < n->size; i++)
    {
        search_val -= n->items[i].prob;
        if(search_val <= 0) return (int32_t)i;
    }

    return (int32_t)(n->size-1);
}




//Select a single item at random from a neighbour
//and return its ID
ItemId Neighbour_randSelect(struct Neighbour *n)
{
    double r = rand_double(0.0, 1.0);
    int32_t nid = Neighbour_search(n, r);

    return n->items[nid].itemid;
}

