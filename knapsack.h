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

    //dynamic structure of all items
    //cap is the allocated size, different from the used size above
    size_t cap;
    struct K_item_prob* items;

    _Alignas(8) char __padding;
};



void Knapsack_init(struct Knapsack*);
void Knapsack_destroy(struct Knapsack*);

bool Knapsack_canAddItem(struct Knapsack*, ItemId);
void Knapsack_addItem(struct Knapsack*, ItemId);

struct Neighbour *createNeighbour(struct Knapsack*);
void deleteNeighbour(struct Neighbour*);

ItemId Neighbour_randSelect(struct Neighbour*);

#endif
