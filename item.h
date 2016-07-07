#ifndef __ITEM_H__
#define __ITEM_H__

#include "const.h"
#ifdef THREADED
#include <pthread.h>
#endif

struct Item
{

#ifdef THREADED
    /** the lock of a given item
     * for pheromone updating
     */

    pthread_mutex_t itemLock;
#endif


    /**
     * Each type of restriction, independent of each other
     * A Knapsack cannot go over any restriction
     */
    Restr *restrictions;

    PherDes pdValue;

    /**
     * How desirable is an item, calculated from value and restrictions
     */
    Desirability desirability;
 
    /**
     * Declaring the pheromone as _Atomic guarantees no race writes will occur
     */
    Pher pheromone;

    /**
     * The value of the item
     */
    Cost value;
};

extern struct Item *universe;

void Item_init(struct Item*);

void Item_updatePdValue(struct Item*);
Desirability get_desirability(Cost, Restr*);
void Item_addPheromone(struct Item*, Pher);

inline void Item_evapPheromone(struct Item *i)
{
    i->pheromone *= (1 - PHE_EVAP);
}

void evapPheromones();

void create_universe();
void delete_universe();

#endif

