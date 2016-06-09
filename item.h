#ifndef __ITEM_H__
#define __ITEM_H__

#include "const.h"

struct Item
{
    /**
     * The value of the item
     */
    Cost value;

    /**
     * Each type of restriction, independent of each other
     * A Knapsack cannot go over any restriction
     */
    Restr restrictions[NUM_RESTRICTIONS];

    /**
     * How desirable is an item, calculated from value and restrictions
     */
    Desirability desirability;

    Pher pheromone;

    PherDes pdValue;
};

extern struct Item universe[NUM_ITEMS];

void Item_updatePdValue(struct Item*);

void Item_addPheromone(struct Item*, Pher);
void evapPheromones();

void create_universe();

#endif

