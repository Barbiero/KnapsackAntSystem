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


Cost MIN_VALUE = 1;
Cost MAX_VALUE = 100;

Restr MIN_REST[] = {0.5, 100.0};
Restr MAX_REST[] = {100.0, 2000.0};

Pher PHE_INIT = 40.0;
Pher PHE_MAX = 100.0;

float64 PHE_EVAP = 0.05;

float64 PHE_WEIGHT = 0.1;
float64 DES_WEIGHT = 1.5;



void Item_init(struct Item* item)
{
    //An item initializes with all values as 0
    (*item) = (struct Item){0};

    //Except for its pheromone
    item->pheromone = PHE_INIT;
}
/**
 * Returns the desirability based on value and the restrictions
 * value / sum(restriction/restricition_range)
 */
Desirability get_desirability(Cost value, Restr rest[NUM_RESTRICTIONS])
{
    Desirability d = (Desirability)value;
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        d = d / (rest[i] / (MAX_REST[i]-MIN_REST[i]) );
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

void Item_addPheromone(struct Item *i, Pher delta)
{
    i->pheromone = (Pher)fmin(i->pheromone + delta, PHE_MAX);
}


void Item_evapPheromone(struct Item *i)
{
    i->pheromone *= (1 - PHE_EVAP);
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
    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        Item_init(&universe[i]);
        Item_rand(&universe[i]);
    }

    qsort(universe, NUM_ITEMS, sizeof(struct Item), desire_order);
}

