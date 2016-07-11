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

void ant_fin(struct Ant *ant)
{
    Knapsack_destroy(&ant->solution);
}

void ant_buildSolution(struct Ant* ant)
{
    struct Neighbour n;
    n.items = malloc(sizeof(*n.items) * NUM_ITEMS);
    while(true)
    {
        createNeighbour(&ant->solution, &n);
        if(n.size == 0){
            break;
        }

        ItemId itemid = Neighbour_randSelect(&n);        

        Knapsack_addItem(&ant->solution, itemid);
    }

    if(n.items != NULL) {
        free(n.items);
    }
}

static inline Pher ant_getPherDelta(struct Ant* ant)
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
void ant_getDeltaPherMatrix(struct Ant *ant, Pher *(delta[]))
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
 */
void ant_updPheromonesMatrix(Pher *(delta[]))
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
            Item_addPheromone(&universe[i], delta);
        }
    }
}

