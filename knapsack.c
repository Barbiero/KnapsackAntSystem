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

static void Knapsack_initCapacity(Restr *cap)
{
    memcpy(cap, cap_init, sizeof(*cap) * NUM_RESTRICTIONS);
}

/**
 * Sets the memory of k to that of a default knapsack
 */
void Knapsack_init(struct Knapsack *k)
{
    memcpy(k, &k_init, sizeof(*k));

    k->capacity = malloc(sizeof(*k->capacity) * NUM_RESTRICTIONS);
    Knapsack_initCapacity(k->capacity);
    k->has_item = calloc(sizeof(*k->has_item), NUM_ITEMS);
}

void Knapsack_destroy(struct Knapsack *k)
{
    free(k->has_item);
    free(k->capacity);
}

static inline bool Knapsack_canAddItem(struct Knapsack *k, ItemId i)
{
    if(k->has_item[i]) return false;

    Restr *restrictions = universe[i].restrictions;
    
//    for(RestrId c = 0; c < NUM_RESTRICTIONS; c++)
    for(RestrId c = NUM_RESTRICTIONS-1; c >= 0; --c)
    {
        if(k->capacity[c] < restrictions[c]){
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

void createNeighbour(struct Knapsack *k, struct Neighbour *n)
{
    //iterate over all items and add to the neighbourhood
    //only the items that can be added to the knapsack k

    assert(n->items != NULL);
    n->size = 0;

    size_t j = 0; //neighbour iterator

    PherDes sum = 0.0;

    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        if(!Knapsack_canAddItem(k, i)){
            continue;
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

}


static int32_t Neighbour_search(struct Neighbour *n, double search_val)
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
    float64 r = rand_double(0.0, 1.0);
    int32_t nid = Neighbour_search(n, r);

    return n->items[nid].itemid;
}

