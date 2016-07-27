//////////////////
//Defines communication schemes for MPI Send/Recv
//

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <mpi.h>

#include "const.h"
#include "ant.h"
#include "item.h"


// The system uses a star topology due to global pheromones
// Each iteration generates a new pheromone update matrix
// And on each iteration, every node sends this matrix to every other node

extern int32_t num_ants;
extern int32_t num_iterations;


/**
 * pair (itemid,delta) which will be broadcast
 */
struct ItemDeltaPher {
    Pher delta;
    ItemId itemid;
};

MPI_Datatype DATATYPE_ITEMPHER;
void registerDataType()
{
    int blocklen[] = {1, 1};
    MPI_Aint offsets[] = {
        offsetof(struct ItemDeltaPher, delta),
        offsetof(struct ItemDeltaPher, itemid)
    };
    MPI_Datatype types[] = {
        MPI_DOUBLE,
        MPI_INT
    };

    MPI_Type_create_struct(2, blocklen, offsets, types, &DATATYPE_ITEMPHER);
    MPI_Type_commit(&DATATYPE_ITEMPHER);
}

//Define the slice in which the node will work on
//Global variables available such as num_ants and num_threads
Cost processAnts(int32_t wrank, int32_t wsize)
{
    registerDataType();
    //on MPI, num_ants refers to number of ants per node
    //as opposed to number of ants in the whole system
    
    //1. Process the ant system as usual
    //2. Gather a delta_pher matrix from the solution
    //3. broadcast the delta_pher matrix + evap local pheromones
    //4. update the pheromones + update PdValues
   
    struct Ant *ants = malloc(sizeof(*ants) * num_ants);
    int32_t it = num_iterations;

    Cost best = 0;
    Pher *pheMatrix = malloc(sizeof(*pheMatrix) * NUM_ITEMS);
    while(it-- > 0) {
        memset(pheMatrix, 0, sizeof(*pheMatrix) * NUM_ITEMS);

        //process the ant system as usual & gather a pheMatrix
        for(int32_t i = 0; i < num_ants; i++) {
            ant_init(&ants[i]);
            ant_buildSolution(&ants[i]);
            if(ants[i].solution.worth > best)
                best = ants[i].solution.worth;

            ant_getDeltaPherMatrix(&ants[i], &pheMatrix);
            ant_fin(&ants[i]);
        }
        
        //transform the pheMatrix into a small list
        //Which will be broadcast to other systems

        //Create a big list but save the number of updates
        //Since we're traversing through NUM_ITEMS
        //might as well evaporate the pheromones here
        struct ItemDeltaPher idp[NUM_ITEMS];
        int numPherUpdates = 0;
        for(ItemId i = 0; i < NUM_ITEMS; i++) {
            if(pheMatrix[i] != 0) {
                idp[numPherUpdates] = 
                (struct ItemDeltaPher){
                    .delta = pheMatrix[i],
                    .itemid = i
                };

                numPherUpdates++;
            }

            Item_evapPheromone(&universe[i]);
        }
    
        //prepare to broadcast the update matrix
        //by sending the size of these matrixes first
        int updMatSize[wsize];
        updMatSize[wrank] = numPherUpdates;
        for(int32_t i = 0; i < wsize; i++) {
            MPI_Bcast(&updMatSize[i], 1, MPI_INT, i, MPI_COMM_WORLD);
        }
        

        //now allocate and broadcast the update lists
        for(int32_t i = 0; i < wsize; i++) {

            struct ItemDeltaPher updates[updMatSize[i]];
            if(i == wrank) {
                for(int j = 0; j < numPherUpdates; j++) {
                    updates[j] = idp[j];
                }
            }

            MPI_Bcast(&updates, updMatSize[i], DATATYPE_ITEMPHER, 
                    i, MPI_COMM_WORLD);

            //now update pheMatrix to include the newly received values
            for(int j = 0; j < updMatSize[i]; j++) {
                pheMatrix[updates[j].itemid] += updates[j].delta;
            }

            //now that the matrix was updated, we can free this memory
        }

        //NOW update the items with their new pheromone values
        ant_updPheromonesMatrix(&pheMatrix);

        //Finally update the PdValues
        for(ItemId i = 0; i < NUM_ITEMS; i++) {
            Item_updatePdValue(&universe[i]);
        }
    }
    free(pheMatrix);

    return best;
}

