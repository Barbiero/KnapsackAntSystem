#ifndef __ITEM_H__
#define __ITEM_H__

#define NUM_RESTRICTIONS 2
#define NUM_ITEMS 300

struct Item
{
    /**
     * The value of the item
     */
    int value;

    /**
     * Each type of restriction, independent of each other
     * A Knapsack cannot go over any restriction
     */
    double restrictions[NUM_RESTRICTIONS];

    /**
     * How desirable is an item, calculated from value and restrictions
     */
    double desirability;

    double pheromone;
};

struct Item universe[NUM_ITEMS];

double Item_getPherDesireValues(struct Item*);

void create_universe();

#endif

