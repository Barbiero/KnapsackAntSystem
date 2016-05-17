////////////////////////////////////////////
//Definition and methods related to items and their restrictions

//Restrictions are defined at compile time,
//ie. every item has all restrictions

#include <stdlib.h>
#include <math.h>
#include <tgmath.h>

#include "item.h"
#include "util.h"

const int MIN_VALUE = 1;
const int MAX_VALUE = 100;

const double MIN_REST[] = {0.5, 100.0};
const double MAX_REST[] = {100.0, 2000.0};

const double PHE_INIT = 1.0;
const double PHE_MAX = 100.0;

const double PHE_EVAP = 0.05;

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
double get_desirability(int value, double rest[NUM_RESTRICTIONS])
{
    double d = (double)value;
    for(int i = 0; i < NUM_RESTRICTIONS; i++)
    {
        d = d / (rest[i] / (MAX_REST[i]-MIN_REST[i]) );
    }

    return d;
}

/**
 * Randomizes an item's attributes
 */
void Item_rand(struct Item* i)
{
    //Assuming seed was already initialized

    (*i).value = rand_int(MIN_VALUE, MAX_VALUE);
    for(int j = 0; j < NUM_RESTRICTIONS; j++)
    {
        (*i).restrictions[j] = rand_double(MIN_REST[j], MAX_REST[j]);
    }

    //calc desirability
    (*i).desirability = get_desirability((*i).value, (*i).restrictions);
}

void Item_addPheromone(struct Item *i, double delta)
{
    i->pheromone = fmax(i->pheromone + delta, PHE_MAX);
}

void Item_evapPheromone(struct Item *i)
{
    i->pheromone *= (1 - PHE_EVAP);
}


const double PHE_WEIGHT = 0.01;
const double DES_WEIGHT = 0.05;

double Item_getPherDesireValues(struct Item *i)
{
    double pherVal = pow(i->pheromone, PHE_WEIGHT);
    double desVal = pow(i->desirability, DES_WEIGHT);

    return pherVal * desVal;
}

int desire_order(const void* a, const void* b)
{
    const struct Item* iA = (struct Item*)a;
    const struct Item* iB = (struct Item*)b;

    return iB->desirability - iA->desirability;
}

//universe is the set of all items in the system

void create_universe()
{
    for(int i = 0; i < 300; i++)
    {
        Item_init(&universe[i]);
        Item_rand(&universe[i]);
    }

    qsort(universe, NUM_ITEMS, sizeof(struct Item), desire_order);
}

