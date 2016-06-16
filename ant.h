#ifndef __ANT_H__
#define __ANT_H__

#include "knapsack.h"

struct Ant {
    struct Knapsack solution;
    //
};


void ant_init(struct Ant*);
void ant_fin(struct Ant*);
void ant_buildSolution(struct Ant*);

void ant_getDeltaPherMatrix(struct Ant*, Pher*[]);
void ant_updPheromonesMatrix(Pher*[]);

void ant_updatePheromones(struct Ant*);

#endif

