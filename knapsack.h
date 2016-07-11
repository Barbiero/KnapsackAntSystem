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
    Restr *capacity;

    /*
     * number of items in the knapsack
     */
    size_t num_items;

    /*
     * worth of the knapsack, aka. the sum of all items worth
     */
    _Alignas(8) Cost worth;


};

struct K_item_prob{
    //probability to choose this item
    Prob prob;

    _Alignas(8) ItemId itemid;
};

struct Neighbour {
    size_t size; //number of items in the neighbourhood

    struct K_item_prob* items;
};



void Knapsack_init(struct Knapsack*);
void Knapsack_destroy(struct Knapsack*);

void Knapsack_addItem(struct Knapsack*, ItemId);

void createNeighbour(struct Knapsack*, struct Neighbour*);

ItemId Neighbour_randSelect(struct Neighbour*);

#endif
