#ifndef __KNAPSACK_H__
#define __KANPSACK_H__

#include "item.h"
#include <stdbool.h>

struct Knapsack {
    /*
     * Marks wether or not the sack contains an item
     */
    bool has_item[NUM_ITEMS];

    /*
     * marks the remaining capacity the sack has
     */
    double capacity[NUM_RESTRICTIONS];
};

struct K_item_prob{
    int itemid;

    //cumulative distribution function
    double cdf;

    //fixes exactly 16 bytes
    int __padding;
};

struct Neighbour {
    int size; //number of items in the neighbourhood

    //dynamic structure of all items
    //cap is the allocated size, different from the used size above
    int cap;
    struct K_item_prob* items;
};



void Knapsack_init(struct Knapsack*);

struct Neighbour *createNeighbour(struct Knapsack*);
void deleteNeighbour(struct Neighbour*);

int Neighbour_binSearch(struct Neighbour*, double);

int Neighbour_randSelect(struct Neighbour*);

#endif
