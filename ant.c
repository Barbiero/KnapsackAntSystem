////////////////
//Definitions for an Ant
//
//Ant - walks through the system building a solution
//Adds pheremone for each item selected on its solution
//

#include <stdlib.h>
#include <stdio.h>
#include "ant.h"
#include "item.h"


void ant_init(struct Ant* ant)
{
    Knapsack_init(&ant->solution);
}

void ant_fin(struct Ant *ant __attribute__((unused)) )
{
    //destructor
    Knapsack_destroy(&ant->solution);
}


void ant_buildSolution(struct Ant* ant)
{

    while(true)
    {
        struct Neighbour *n = createNeighbour(&ant->solution);
        if(n->size == 0){
            deleteNeighbour(n);
            break;
        }

        ItemId itemid = Neighbour_randSelect(n);        

        Knapsack_addItem(&ant->solution, itemid);
        deleteNeighbour(n);
    }
}

inline Pher ant_getPherDelta(struct Ant* ant)
{
    return (Pher)(ant->solution.worth * 0.025);
}

/**
 * adds DeltaPher to the delta matrix, which shall be a pointer
 * to Pher with exactly NUM_ITEMS elements.
 *
 * In the first call to this function within a thread, delta shall be
 * pre allocated and zero-initalized.
 *
 * This function is THREAD UNSAFE, and delta shall not
 * be shared accross multiple threads.
 */
void ant_getDeltaPherMatrix(struct Ant *ant, Pher *delta[])
{
    Pher deltaPher = ant_getPherDelta(ant);

    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        if(ant->solution.has_item[i])
        {
            (*delta)[i] += deltaPher;
        }
    }
}

/**
 * Updates item pheromones with a matrix
 * THREAD SAFE: mutexes are in place to make sure no racing occurs
 */
void ant_updPheromonesMatrix(Pher *delta[])
{
    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        if( (*delta)[i] != 0)
        {
            Item_addPheromone(&universe[i], (*delta)[i]);
        }
    }
} 



/**
 * Updates pheromones based off current solution
 * trails are updated by Q*value
 */
void ant_updatePheromones(struct Ant* ant)
{
    Pher delta = ant_getPherDelta(ant);

    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        if(ant->solution.has_item[i])
        {
            //Critical region!
            Item_addPheromone(&universe[i], delta);
            //!Critical region
        }
    }
}

