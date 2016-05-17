/////////////
//Knapsack definitions

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "knapsack.h"
#include "util.h"

//default values for a knapsack
struct Knapsack k_init = { {false}, {3000.0, 10000.0} };

/**
 * Sets the memory of k to that of a default knapsack
 */
void Knapsack_init(struct Knapsack *k)
{
    assert(k != NULL);
    memcpy(k, &k_init, sizeof(struct Knapsack));
}

bool Knapsack_canAddItem(struct Knapsack *k, int i)
{
    if(k->has_item[i]) return false;

    struct Item *item = &universe[i];
    
    for(int c = 0; c < NUM_RESTRICTIONS; c++)
    {
        if(k->capacity[c] < item->restrictions[c])
        {
            return false;
        }
    }

    return true;
}


struct Neighbour *createNeighbour(struct Knapsack *k)
{
    //iterate over all items and add to the neighbourhood
    //only the items that can be added to the knapsack k
    //allocate only as much space as necessary, multiplying by 2 each time


    struct Neighbour *n = malloc(sizeof(struct Neighbour));

    n->items = malloc(sizeof(struct K_item_prob));
    n->size = 0;
    n->cap = 1; //neighbour array capacity

    int j = 0; //neighbour iterator

    double cdf = 0.0;

    for(int i = 0; i < NUM_ITEMS; i++)
    {
        if(!Knapsack_canAddItem(k, i)){
            continue;
        }

        if(j >= n->cap)
        {
            n->cap *= 2;
            n->items = realloc(n->items, sizeof(struct K_item_prob) * n->cap);
        }

        double pherDes = Item_getPherDesireValues(&universe[i]);

        n->items[j] = (struct K_item_prob){
            .itemid = i,
            .cdf = cdf + pherDes
        };
        cdf += pherDes;

        n->size++;


        j++;
    }

    //normalize CDF
    for(int i = 0; i < n->size; i++)
    {
        n->items[i].cdf /= cdf;
    }

    return n;
}

void deleteNeighbour(struct Neighbour *n)
{
    free(n->items);
    free(n);
}

//Binary search an item using its cdf
int Neighbour_binSearch(struct Neighbour *n, double search_val)
{
    int first = 0; 
    int last = n->size - 1;
    int mid = ceil( (first+last)/2.0 );

    while(first <= last)
    {
        if(n->items[mid].cdf > search_val)
        {
            last = mid - 1;
        }
        else if(n->items[mid].cdf < search_val){
            first = mid + 1;
        }
        else{
            //mid == search_val, very unlikely
            return mid;
        }

        mid = ceil( (first+last) / 2.0 );
    }

    return mid;
}


//Select a single item at random from a neighbour
int Neighbour_randSelect(struct Neighbour *n)
{
    double r = rand_double(0.0, 1.0);
    return Neighbour_binSearch(n, r);
}

