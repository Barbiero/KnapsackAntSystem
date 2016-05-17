#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "item.h"
#include "knapsack.h"
#include "util.h"

void main()
{
    srand(time(NULL));
    create_universe();

    /*
    for(int i = 0; i < 300; i++)
    {
        printf("<%d,%.2f>:[", 
                universe[i].value,
                universe[i].desirability);

        for(int j = 0; j < NUM_RESTRICTIONS; j++){
            printf("%.2f,", universe[i].restrictions[j]);
        }
        printf("]\n");
    }*/


    struct Knapsack k;
    Knapsack_init(&k);


    while(1){
        struct Neighbour *n = createNeighbour(&k);
        if(n->size == 0){
            deleteNeighbour(n);
            break;
        }

        int itemid = Neighbour_randSelect(n);

        k.has_item[itemid] = true;
        for(int c = 0; c < NUM_RESTRICTIONS; c++)
        {
            k.capacity[c] -= universe[itemid].restrictions[c];
        }
        deleteNeighbour(n);
    }

    printf("Complete\n");
    printf("number of items: ");
    int n = 0;
    int sum = 0;
    for(int i = 0; i < NUM_ITEMS; i++){
        if(k.has_item[i]){
            n++;
            sum += universe[i].value;
        }
    }
    printf("Value: %d\n", sum);

    printf("%d\n", n);

    printf("\n");

}


