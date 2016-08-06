#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
//#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "ant.h"
#include "const.h"
#include "item.h"
#include "knapsack.h"
#include "util.h"

#ifdef THREADED
#include <pthread.h>

static int num_threads = 4;
#endif

#include <mpi.h>
int wrank;
int wsize;

ItemId NUM_ITEMS;
//Number of Restrictions for each item
RestrId NUM_RESTRICTIONS;

//Min-Max value for each item
Cost MIN_VALUE;
Cost MAX_VALUE;

//Minimum values for restrictions
Restr *MIN_REST;
//Maximum values for restrictions
Restr *MAX_REST;

//How much pheromone items begin with
Pher PHE_INIT; 
//Maximum value of pheromone items can have
Pher PHE_MAX;

//How much pheromone is lost each time it evaporates
float64 PHE_EVAP;

//How strong pheromone is
//Recommended 0 < phe_weight < 1
float64 PHE_WEIGHT;

//How strong the desirability is for an item
//Recommended 1 < des_weight
float64 DES_WEIGHT;

//Default knapsack struct
struct Knapsack k_init;
Restr *cap_init;

struct Item *universe;

int32_t num_iterations = 200;
int32_t num_ants = 50;

static char filename[255];

void process_cli(int argc, char **argv)
{
    int c;
    while(1)
    {
        static struct option long_options[] =
        {
            {"file", required_argument, 0, 'f'},
            {"pheromone", required_argument, 0, 'p'},
            {"evaporate", required_argument, 0, 'e'},
            {"alpha", required_argument, 0, 'a'},
            {"beta", required_argument, 0, 'b'},
            {"iterations", required_argument, 0, 'i'},
            {"ants", required_argument, 0, 'n'},
            {"help", no_argument, 0, 'h'},
#ifdef THREADED
            {"threads", required_argument, 0, 'T'},
#endif
            {0,0,0,0}
        };

        int option_index = 0;

        c = getopt_long(argc, argv, "f:p:e:a:b:i:n:T:h",
                long_options, &option_index);

        if(c == -1)
            break;

        switch(c)
        {
            case 0:
                break;
            case 'p':
                {
                    if(optarg){
                        char *end;
                        float64 p = strtod(optarg, &end);
                        if(optarg == end) break;
                        optarg = end;
                        PHE_INIT = (p>0? p : 0.01) ;

                        p = strtod(optarg, &end);
                        if(optarg == end) break;
                        optarg = end;
                        PHE_MAX = p;

                    }
                }
                break;
            case 'e':
                {
                    if(optarg){
                        PHE_EVAP = atof(optarg);
                    }
                }
                break;
            case 'a':
                {
                    if(optarg){
                        PHE_WEIGHT = atof(optarg);
                    }
                }
                break;
            case 'b':
                {
                    if(optarg){
                        DES_WEIGHT = atof(optarg);
                    }
                }
                break;

            case 'i':
                {
                    if(optarg){
                        num_iterations = atoi(optarg);
                    }
                }
                break;
            case 'n':
                {
                    if(optarg){
                        num_ants = atoi(optarg);
                        num_ants /= wsize;
                    }
                }
                break;
            case 'f':
                {
                    if(optarg){
                        strcpy(filename, optarg);
                    }
                }
                break;
          case 'T':
                {
#ifdef THREADED
                    if(optarg){
                        num_threads = atoi(optarg);
                        if( !( ((num_threads != 0) &&
                           ((num_threads & (~num_threads + 1)) == num_threads))) ){
                            //not power of 2
                            fprintf(stderr, "Warning: selected %"PRIi32" threads, "
                                    "but only power of two allowed. Choosing default 4",
                                    num_threads);
                            num_threads = 4;
                        }
                    }
#endif
                }
                break;
            case 'h':
            default:
                {
                    if(wrank==0){
                    printf("Options:\n");
/*                    printf("\t--min_value <value>: Set minimum item worth\n");
                    printf("\t--max_value <value>: Set maximum item worth\n");
                    printf("\t--min_restrictions '<value> <value>': "
                            "minimum value of item restrictions\n");
                    printf("\t--max_restrictions '<value> <value>': "
                            "maximum value of item restrictions\n");*/
                    printf("\t--pheromone '<initial> <max>': "
                           "set values for pheromones\n");
                    printf("\t--evaporate <value>: "
                            "evaporation coeficient\n");
                    printf("\t--alpha <value>: "
                            "how strong pheromone is\n");
                    printf("\t--beta <value>: "
                            "how strong items desirability is\n");
                    /*
                    printf("\t--knap-cap '<value <value>': "
                            "set capacity size for knapsacks\n");*
                    printf("\t--items <value': "
                            "set number of items on the system\n");*/
                    printf("\t--iterations <value>: "
                            "set number of iterations of the ant system\n");
                    printf("\t--ants <value>: "
                            "set number of ants of the ant system\n");
                    printf("\t--file name: "
                        "set the file from which the system will take info\n");
#ifdef THREADED
                    printf("\t--threads <value>: "
                            "set number of threads of the system."
                            "\n\t"
                            "note: must be a power of two.\n");
#endif
                    }
                    exit(0);
                }
                break;
        }

    }



    if(wrank == 0) {
    printf("Selected Values:\n");
#ifdef THREADED
    printf("\tThreads: %"PRIi32"\n", num_threads);
#endif
    /*
    printf("\tWorth: [%" PRIi32 ", %" PRIi32 "]\n", MIN_VALUE, MAX_VALUE);
    printf("\tRestrictions: { ");
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        printf("[%.2f, %.2f],", MIN_REST[i], MAX_REST[i]);
    }
    printf(" }\n");
    printf("\tKnapsack Capacities:\n");
    printf("\t\t{ ");
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        printf("%.2f,", cap_init[i]);
    }
    printf(" }\n");
    printf("\tNumber of items: %" PRIi32 "\n", NUM_ITEMS);
        */


    printf("\tPheromone: initial: %.2f max: %.2f\n", PHE_INIT, PHE_MAX);
    printf("\tEvap coef: %.2f\n", PHE_EVAP);
    printf("\tAlpha: %.2f Beta: %.2f\n", PHE_WEIGHT, DES_WEIGHT);
    printf("\tIterations: %" PRIi32 "\n", num_iterations);
    printf("\tAnts: %" PRIi32 " per node\n", num_ants);
    printf("\n");
    }


}

//comm_protocol.c
extern Cost processAnts(int wrank, int wsize);
//

struct Ant (*ants);


#ifdef THREADED
static pthread_barrier_t barrier;

struct t_args {
    int32_t threadid;
};

static int32_t min(int32_t a, int32_t b)
{ return (a<b)?a:b; }

void*
thread_processAnts(void *args)
{
    struct t_args* antarg = (struct t_args*)args;
    int32_t tid = antarg->threadid;

    int32_t tfactor = (int32_t)ceil((double)num_ants/(double)num_threads);

    int32_t ini = (tid) * tfactor;
    int32_t fin = min(ini + tfactor, num_ants);
    int32_t i;

    int32_t itfactor = (int32_t)ceil((double)NUM_ITEMS/(double)num_threads);
    int32_t ini_items = (tid) * itfactor;
    int32_t fin_items = min(ini_items + itfactor, NUM_ITEMS);

    int32_t localit = num_iterations;

    Cost *localBest = malloc(sizeof(*localBest));
    *localBest = 0;

    Pher *pheMatrix = malloc(sizeof(*pheMatrix) * NUM_ITEMS);

    while(localit--) {
        memset(pheMatrix, 0, sizeof(*pheMatrix) * NUM_ITEMS);

        pthread_barrier_wait(&barrier);

        for(i = ini; i < fin; i++)
        {
            ant_init(&ants[i]);
            ant_buildSolution(&ants[i]);
            if(ants[i].solution.worth > *localBest)
                *localBest = ants[i].solution.worth;

            ant_getDeltaPherMatrix(&ants[i], &pheMatrix);
            ant_fin(&ants[i]);
        }

        pthread_barrier_wait(&barrier);
       

        for(i = ini_items; i < fin_items; i++)
        {
            Item_evapPheromone(&universe[i]);
        }

        pthread_barrier_wait(&barrier);


        ant_updPheromonesMatrix(&pheMatrix);

        pthread_barrier_wait(&barrier);
        
        for(i = ini_items; i < fin_items; i++) 
        {
            Item_updatePdValue(&universe[i]);
        }
    }
    free(pheMatrix);

    return (void*)localBest;
}
#endif

void initDefaults()
{
    strcpy(filename, "mkuniverse.txt");

    MIN_VALUE = 1;
    MAX_VALUE = 100;

    PHE_INIT = 50.0;
    PHE_MAX = 1000.0;

    PHE_EVAP = 0.05;

    PHE_WEIGHT = 0.1;
    DES_WEIGHT = 1.5;

    NUM_ITEMS = 15000;
    universe = NULL;

    NUM_RESTRICTIONS = 2;

    cap_init = malloc(sizeof(*cap_init) * NUM_RESTRICTIONS);
    cap_init[0] = 10000.0;
    cap_init[1] = 20000.0;

    MIN_REST = malloc(sizeof(*MAX_REST) * NUM_RESTRICTIONS);
    MIN_REST[0] = 0.5;
    MIN_REST[1] = 10.0;
    
    MAX_REST = malloc(sizeof(*MAX_REST) * NUM_RESTRICTIONS);
    MAX_REST[0] = 100.0;
    MAX_REST[1] = 50000.0;


    k_init = (struct Knapsack){
        .has_item = NULL, 
        .capacity = NULL,
        .worth = (Cost) 0,
        .num_items = (size_t)0
    };



}

void readmknap(FILE* fd)
{
    //read a file like http://people.brunel.ac.uk/~mastjjb/jeb/orlib/mknapinfo.html

    //assumes fd is already open

    /*
     * number of vars, number of constrains, solution(or zero)
     * p(j), j = 1 ... n
     * r(i,j), i = 1...m, j = 1...n
     * b(i), b = 1...m
     */

    Cost s;
    if(fscanf(fd, " %i %i %i ", &NUM_ITEMS, &NUM_RESTRICTIONS, &s) != 3){
        fprintf(stderr, "Invalid file: cant read num_items and num_restrictions"
                "\n");
        exit(-1);
    }

    //read universe
    universe = (struct Item*)malloc(sizeof(*universe) * NUM_ITEMS);
    
    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {

        Item_init(&universe[i]);

        if(fscanf(fd, " %i ", &universe[i].value) != 1){
            fprintf(stderr, "Invalid file. Can't read cost from Item ID %i\n"
                    , i);
            exit(-1);
        }
    }

    //read constraints
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        for(ItemId j = 0; j < NUM_ITEMS; j++)
        {
            if(fscanf(fd, " %lf ", &universe[j].restrictions[i]) != 1){
                fprintf(stderr, "Invalid file. Can't read restriction"
                        " %i from item %i\n", i, j);
                exit(-1);
            }
        }
    }

    //set desirability
    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        universe[i].desirability = get_desirability(universe[i].value, 
                universe[i].restrictions);

        Item_updatePdValue(&universe[i]);
    }

    //read knapsack limits
    cap_init = (Restr*)malloc(sizeof(*cap_init) * NUM_RESTRICTIONS);
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        if(fscanf(fd, " %lf ", &cap_init[i]) != 1) {
            fprintf(stderr, "Invalid file. Can't read cap %i\n", i);
            exit(-1);
        }
    }

}

void createmknap(FILE *fd)
{
    //creates a mknap file

    Cost sol = 0;
    fprintf(fd, "%i %i %i\n", NUM_ITEMS, NUM_RESTRICTIONS, sol);

    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        fprintf(fd, "%i ", universe[i].value);
        if(i%20 == 19) fprintf(fd, "\n");
    }

    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        for(ItemId j = 0; j < NUM_ITEMS; j++)
        {
            fprintf(fd, "%lf ", universe[j].restrictions[i]);
            if(j%20 == 19) fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
    }

    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        fprintf(fd, "%lf ", cap_init[i]);
    }
    fprintf(fd, "\n");
}

int main(int argc, char **argv)
{

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    setbuf(stdout, NULL);

    initDefaults();

    //read CLI options
    process_cli(argc, argv);

#ifdef THREADED
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int s = pthread_barrier_init(&barrier, NULL, num_threads);
    if(s != 0){
        perror("pthread_barrier_init");
        exit(s);
    }

    pthread_t threads[num_threads];
#endif

/*
    create_universe();

    FILE* fd = fopen(filename, "w");
    createmknap(fd);
    fclose(fd);
*/ 

 
    FILE *fd = fopen(filename, "r");
    if(fd == NULL) {
        perror("");
        exit(errno);
    }

    readmknap(fd);
    fclose(fd);


    clock_t start, end;
/*
    struct timeval start, stop;
    gettimeofday(&start, NULL);
*/
    //millisecond value of system clock to seed the RNG
    srand( time(NULL) + wrank );

    Cost best = 0;

    if(wrank==0){
        printf("Ant system starting\n");
    }
    
    //gettimeofday(&start, NULL);
    start = clock();
    //struct Ant ants[num_ants];
    //ants = malloc(sizeof(*ants) * num_ants);

#ifndef THREADED
    /*
    int32_t it = num_iterations;
    while(it-- > 0){
       
        for(int32_t x = 0; x < num_ants; x++){
            ant_init(&ants[x]);
            ant_buildSolution(&ants[x]);
            if(ants[x].solution.worth > best)
                best = ants[x].solution.worth;
        }

        evapPheromones();

        for(int32_t x = 0; x < num_ants; x++){
            ant_updatePheromones(&ants[x]);
            ant_fin(&ants[x]);
        }

        //Update Pheromone * Desirability values all around
        //Only need to do this after all pheromones were updated
        for(ItemId i = 0; i < NUM_ITEMS; i++){
            Item_updatePdValue(&universe[i]);
        }
    }
    */

    Cost localBest = processAnts(wrank, wsize);
    Cost bestBcast;
    for(int i = 0; i < wsize; i++) {
        if(wrank == i) {
            bestBcast = localBest;
        }

        MPI_Bcast(&bestBcast, 1, MPI_INT, i, MPI_COMM_WORLD);
        if(bestBcast > best) {
            best = bestBcast;
        }

        if(wrank == 0) {
            printf("[%i] Local Best: %i\n", i, bestBcast);
        }
    }

#else
    struct t_args args[num_threads];
    for(int32_t i = 0; i < num_threads; i++) {
        args[i] = (struct t_args){i};
        if(pthread_create(&threads[i], &attr, thread_processAnts, &args[i]) 
                == 0) {
            printf("\tThread %i created succesfully\n", i);
        } else {
            perror("");
        }
    }

    for(int32_t i = 0; i < num_threads; i++) {
        Cost *localBest;
        if(pthread_join(threads[i], (void**)(&localBest)) == 0) {
            printf("\tThread %i joined succesfully. localBest: %" PRIi32 "\n",
                    i, *localBest);
        } else {
            perror("");
        }

        if(*localBest > best)
            best = *localBest;

        free(localBest);
    }

#endif
    if(wrank == 0) {
        end = clock();
        double time = (double) (end - start) / CLOCKS_PER_SEC * 1000.0;

        ldiv_t sec_ms = ldiv(time, 1000.0);
        ldiv_t minsec = ldiv(sec_ms.quot, 60);

        if(minsec.quot > 0){
            printf("Ant system finished in %lim %lis %lims\n",
                    minsec.quot, minsec.rem, 
                    sec_ms.rem);
        }
        else{
            printf("Ant system finished in %lis %lims\n",
                minsec.rem,
                sec_ms.rem );
        }

        printf("Best result found: %" PRIi32 "\n", best);
    }

    delete_universe();

#ifdef THREADED
    pthread_barrier_destroy(&barrier);
#endif

    MPI_Finalize();
}


