//////////////////////////////////////
//Constant parameters used everywhere
//////////////////////////////////////

#ifndef __CONST_H__
#define __CONST_H__

#include <inttypes.h>

//number of bits for each FP type
typedef float float32;
typedef double float64;

//Type definitions of low level structures

//Used to iterate over arrays with NUM_ITEMS elements
typedef int32_t ItemId;
//Used to iterate over arrays with NUM_RESTRICTIONS elements
typedef int32_t RestrId;

//Used for cost/worth of items
typedef int32_t Cost;

//Used for restrictions(or weight) of items
typedef float64 Restr;

//Used for probability calculations
typedef float64 Prob;

//Used for Pheromone calculations
typedef float64 Pher;

//Used for desirability of an item
typedef float64 Desirability;

//Coeficient of Pher*Desire, must be compatible with both types
typedef float64 PherDes;


//Number of items in the universe
#define NUM_ITEMS 300
//Number of Restrictions for each item
#define NUM_RESTRICTIONS 2

//Min-Max value for each item
extern Cost MIN_VALUE;
extern Cost MAX_VALUE;

//Minimum values for restrictions
extern Restr MIN_REST[];
//Maximum values for restrictions
extern Restr MAX_REST[];

//How much pheromone items begin with
extern Pher PHE_INIT; 
//Maximum value of pheromone items can have
extern Pher PHE_MAX;

//How much pheromone is lost each time it evaporates
extern float64 PHE_EVAP;

//How strong pheromone is
//Recommended 0 < phe_weight < 1
extern float64 PHE_WEIGHT;

//How strong the desirability is for an item
//Recommended 1 < des_weight
extern float64 DES_WEIGHT;

//Default knapsack struct
extern struct Knapsack k_init;

#endif

