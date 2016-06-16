#ifndef __KNAPSACK_H__
#define __KNAPSACK_H__

#include <stdbool.h>

#include "const.h"

struct Knapsack {
    /*
     * Marks wether or not the sack contains an item
     */
    bool *has_item;

    /*
     * marks the remaining capacity the sack has
     */
    Restr capacity[NUM_RESTRICTIONS];

    /*
     * worth of the knapsack, aka. the sum of all items worth
     */
    Cost worth;

    /*
     * number of items in the knapsack
     */
    size_t num_items;
};

struct K_item_prob{
    ItemId itemid;

    //probability to choose this item
    Prob prob;

    //padding to complete 16 bytes
    int32_t __padding;
};

struct Neighbour {
    size_t size; //number of items in the neighbourhood

    //dynamic structure of all items
    //cap is the allocated size, different from the used size above
    size_t cap;
    struct K_item_prob* items;
};



void Knapsack_init(struct Knapsack*);
void Knapsack_destroy(struct Knapsack*);

bool Knapsack_canAddItem(struct Knapsack*, ItemId);
void Knapsack_addItem(struct Knapsack*, ItemId);

struct Neighbour *createNeighbour(struct Knapsack*);
void deleteNeighbour(struct Neighbour*);

ItemId Neighbour_randSelect(struct Neighbour*);

#endif
