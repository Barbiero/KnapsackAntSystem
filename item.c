////////////////////////////////////////////
//Definition and methods related to items and their restrictions

//Restrictions are defined at compile time,
//ie. every item has all restrictions

#include <stdlib.h>
#include <math.h>
#include <tgmath.h>

#include "assert.h"
#include "const.h"
#include "item.h"
#include "util.h"

void Item_init(struct Item* item)
{
    //An item initializes with all values as 0
    (*item) = (struct Item){0};

    //restrictions is an array and should be 0-initialized
    item->restrictions = calloc(sizeof(*item->restrictions), NUM_RESTRICTIONS);

    //Except for its pheromone
    item->pheromone = PHE_INIT;

#ifdef THREADED
    pthread_mutex_init(&item->itemLock, NULL);
#endif
}

void Item_fin(struct Item* item)
{
    free(item->restrictions);

#ifdef THREADED
    pthread_mutex_destroy(&item->itemLock);
#endif
}
/**
 * Returns the desirability based on value and the restrictions
 * value / prod(restriction/restricition_range)
 */
Desirability get_desirability(Cost value, Restr *rest)
{
    Desirability d = (Desirability)value;
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        d = d / (rest[i] / cap_init[i]);
    }

    return d;
}

void Item_updatePdValue(struct Item *i)
{
    i->pdValue = (PherDes)(
        (Desirability)pow(i->desirability, DES_WEIGHT) *
        (Pher)pow(i->pheromone, PHE_WEIGHT)
        );
}

/**
 * Randomizes an item's attributes
 */
void Item_rand(struct Item* i)
{
    //Assuming seed was already initialized

    i->value = (Cost)rand_int(MIN_VALUE, MAX_VALUE);
    for(RestrId j = 0; j < NUM_RESTRICTIONS; j++)
    {
        i->restrictions[j] = (Restr)rand_double(MIN_REST[j], MAX_REST[j]);
    }

    //calc desirability
    i->desirability = get_desirability(i->value, i->restrictions);

    Item_updatePdValue(i);
}


inline Pher phMin(Pher a, Pher b){
    return (a > b? b : a);
}

void Item_addPheromone(struct Item *i, Pher delta)
{
#ifdef THREADED
    pthread_mutex_lock(&i->itemLock);
#endif
    i->pheromone = phMin(i->pheromone + delta, PHE_MAX);
#ifdef THREADED
    pthread_mutex_unlock(&i->itemLock);
#endif
}


void evapPheromones()
{
    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        Item_evapPheromone(&universe[i]);
    }
}

int desire_order(const void* a, const void* b)
{
    const struct Item* iA = (struct Item*)a;
    const struct Item* iB = (struct Item*)b;

    Restr rA = 0.0, rB = 0.0;
    for(RestrId c = 0; c < NUM_RESTRICTIONS; c++)
    {
        rA += iA->restrictions[c];
        rB += iB->restrictions[c];
    }


    return rA - rB;
}

//universe is the set of all items in the system

void create_universe()
{
    universe = (struct Item*)malloc(sizeof(*universe) * NUM_ITEMS);

    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        Item_init(&universe[i]);
        Item_rand(&universe[i]);
    }

    qsort(universe, NUM_ITEMS, sizeof(struct Item), desire_order);
}

void delete_universe()
{
    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        Item_fin(&universe[i]);
    }
    free(universe);
}

