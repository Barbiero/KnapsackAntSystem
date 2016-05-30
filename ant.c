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
    //I have a feeling I will need this in the future
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

/**
 * Updates pheromones based off current solution
 * trails are updated by Q*value
 */
void ant_updatePheromones(struct Ant* ant)
{
    Pher delta = (Pher)(ant->solution.worth * 0.025);

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


