#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <sys/time.h>
#include <errno.h>

#include "ant.h"
#include "const.h"
#include "item.h"
#include "knapsack.h"
#include "util.h"
//#include "universe.h"


#ifdef THREADED
#include <pthread.h>
#include "barrier.h"

static int num_threads = 4;
#endif


static int32_t num_iterations = 200;
static int32_t num_ants = 100;
static bool use_predef_universe = true;

void process_cli(int argc, char **argv)
{
    int c;
    while(1)
    {
        static struct option long_options[] =
        {
            {"min_value", required_argument, 0, 'v'},
            {"max_value", required_argument, 0, 'V'},
            {"min_restrictions", required_argument, 0, 'r'},
            {"max_restrictions", required_argument, 0, 'R'},
            {"pheromone", required_argument, 0, 'p'},
            {"evaporate", required_argument, 0, 'e'},
            {"alpha", required_argument, 0, 'a'},
            {"beta", required_argument, 0, 'b'},
            {"knap-cap", required_argument, 0, 'k'},
            {"iterations", required_argument, 0, 'i'},
            {"items", required_argument, 0, 't'},
            {"ants", required_argument, 0, 'n'},
            {"help", no_argument, 0, 'h'},
#ifdef THREADED
            {"threads", required_argument, 0, 'T'},
#endif
            {0,0,0,0}
        };

        int option_index = 0;

        c = getopt_long(argc, argv, "v:V:r:R:p:e:a:b:k:i:n:t:h"
#ifdef THREADED
                "T:"
#endif
                , long_options, &option_index);

        if(c == -1)
            break;

        switch(c)
        {
            case 0:
                break;
            case 'v':
                {
                    if(optarg){
                        MIN_VALUE = atoi(optarg);
                        use_predef_universe = false;
                    }
                }
                break;
            case 'V':
                {
                    if(optarg){
                        MAX_VALUE = atoi(optarg);
                        use_predef_universe = false;
                    }
                }
                break;
            case 'r':
                {
                    char *end;
                    float64 f = 0.0;
                    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
                    {
                        f = strtod(optarg, &end);
                        if(optarg == end) break;

                        optarg = end;
                        MIN_REST[i] = f;
                    }
                    use_predef_universe = false; 
                }
                break;
            case 'R':
                {
                    char *end;
                    float64 f = 0.0;
                    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
                    {
                        f = strtod(optarg, &end);
                        if(optarg == end) break;
                        optarg = end;

                        MAX_REST[i] = f;
                    }
                    use_predef_universe = false;
                }
                break;
            case 'p':
                {
                    if(optarg){
                        char *end;
                        float64 p = strtod(optarg, &end);
                        if(optarg == end) break;
                        optarg = end;
                        PHE_INIT = p;

                        p = strtod(optarg, &end);
                        if(optarg == end) break;
                        optarg = end;
                        PHE_MAX = p;

                        use_predef_universe = false;
                    }
                }
                break;
            case 'e':
                {
                    if(optarg){
                        PHE_EVAP = atof(optarg);
                        use_predef_universe = false;
                    }
                }
                break;
            case 'a':
                {
                    if(optarg){
                        PHE_WEIGHT = atof(optarg);
                        use_predef_universe = false;
                    }
                }
                break;
            case 'b':
                {
                    if(optarg){
                        DES_WEIGHT = atof(optarg);
                        use_predef_universe = false;
                    }
                }
                break;
            case 't':
                {
                    if(optarg){
                        NUM_ITEMS = atoi(optarg);
                        use_predef_universe = false;
                    }
                }
                break;
            case 'k':
                {
                    char *end;
                    float64 f = 0.0;
                    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
                    {
                        f = strtod(optarg, &end);
                        if(optarg == end) break;
                        optarg = end;

                        k_init.capacity[i] = f;
                    }
                    use_predef_universe = false;
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
                    }
                }
                break;
#ifdef THREADED
      case 'T':
                {
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
                }
                break;
#endif
            case 'h':
            default:
                {
                    printf("Options:\n");
                    printf("\t--min_value <value>: Set minimum item worth\n");
                    printf("\t--max_value <value>: Set maximum item worth\n");
                    printf("\t--min_restrictions '<value> <value>': "
                            "minimum value of item restrictions\n");
                    printf("\t--max_restrictions '<value> <value>': "
                            "maximum value of item restrictions\n");
                    printf("\t--pheromone '<initial> <max>': "
                           "set values for pheromones\n");
                    printf("\t--evaporate <value>: "
                            "evaporation coeficient\n");
                    printf("\t--alpha <value>: "
                            "how strong pheromone is\n");
                    printf("\t--beta <value>: "
                            "how strong items desirability is\n");
                    printf("\t--knap-cap '<value <value>': "
                            "set capacity size for knapsacks\n");
                    printf("\t--items <value': "
                            "set number of items on the system\n");
                    printf("\t--iterations <value>: "
                            "set number of iterations of the ant system\n");
                    printf("\t--ants <value>: "
                            "set number of ants of the ant system\n");
#ifdef THREADED
                    printf("\t--threads <value>: "
                            "set number of threads of the system."
                            "\n\t"
                            "note: must be a power of two.");
#endif
                    exit(0);
                }
                break;
        }

    }

    printf("Selected Values:\n");
#ifdef THREADED
    printf("\tThreads: %"PRIi32"\n", num_threads);
#endif
    printf("\tWorth: [%" PRIi32 ", %" PRIi32 "]\n", MIN_VALUE, MAX_VALUE);
    printf("\tRestrictions: { ");
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        printf("[%.2f, %.2f],", MIN_REST[i], MAX_REST[i]);
    }
    printf(" }\n");
    printf("\tPheromone: initial: %.2f max: %.2f\n", PHE_INIT, PHE_MAX);
    printf("\tEvap coef: %.2f\n", PHE_EVAP);
    printf("\tAlpha: %.2f Beta: %.2f\n", PHE_WEIGHT, DES_WEIGHT);
    printf("\tKnapsack Capacities:\n");
    printf("\t\t{ ");
    for(RestrId i = 0; i < NUM_RESTRICTIONS; i++)
    {
        printf("%.2f,", k_init.capacity[i]);
    }
    printf(" }\n");
    printf("\tNumber of items: %" PRIi32 "\n", NUM_ITEMS);
    printf("\tIterations: %" PRIi32 "\n", num_iterations);
    printf("\tAnts: %" PRIi32 "\n", num_ants);
    printf("\n");


}

struct Ant (*ants);

#ifdef THREADED
pthread_mutex_t itemMut;
Barrier barrier;

struct t_args {
    int32_t threadid;
};

inline int32_t min(int32_t a, int32_t b)
{ return (a<b)?a:b; }

void*
thread_processAnts(void *args)
{
    struct t_args* antarg = (struct t_args*)args;
    int32_t tid = antarg->threadid;
    //free(antarg);

    int32_t tfactor = (int32_t)ceil((double)num_ants/(double)num_threads);

    int32_t ini = (tid) * tfactor;
    int32_t fin = min(ini + tfactor, num_ants);
    int32_t i;

    int32_t localit = num_iterations;

    Cost *localBest = malloc(sizeof(*localBest));
    *localBest = 0;

    while(localit--) {

        barrier_wait(&barrier, tid);

        for(i = ini; i < fin; i++)
        {
            ant_init(&ants[i]);
            ant_buildSolution(&ants[i]);
            if(ants[i].solution.worth > *localBest)
                *localBest = ants[i].solution.worth;
        }

        barrier_wait(&barrier, tid);
        if(tid == 0) {
            evapPheromones();
        }
       

        barrier_wait(&barrier, tid);
        Pher *pheMatrix = calloc(sizeof(*pheMatrix), NUM_ITEMS);
        for(i = ini; i < fin; i++)
        {
            ant_getDeltaPherMatrix(&ants[i], &pheMatrix);
            ant_fin(&ants[i]);

            //REGIAO CRITICA
           // ant_updatePheromones(&ants[i]);
           // ant_fin(&ants[i]);
        }

        ant_updPheromonesMatrix(&pheMatrix);

        free(pheMatrix);

        barrier_wait(&barrier, tid);

        if(tid == 0) {
            for(ItemId i = 0; i < NUM_ITEMS; i++) {
                Item_updatePdValue(&universe[i]);
            }
        }
    }


    return (void*)localBest;
}
#endif

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);

    //read CLI options
    process_cli(argc, argv);

#ifdef THREADED
    pthread_mutex_init(&itemMut, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    barrier_init(&barrier, num_threads);
    if(errno != 0){
        perror("");
        exit(errno);
    }

    pthread_t threads[num_threads];
#endif
    //if(!use_predef_universe)
        create_universe();

/*
    FILE *f = fopen("universe.txt", "w");

    fprintf(f, "universe[] = { \n");
    for(ItemId i = 0; i < NUM_ITEMS; i++)
    {
        fprintf(f, "{.value = %" PRIi32 ",", universe[i].value);
        fprintf(f, ".restrictions = {");
        for(RestrId j = 0; j < NUM_RESTRICTIONS; j++)
        {
            fprintf(f, "%lf", universe[i].restrictions[j]);
            if(j < NUM_RESTRICTIONS-1) fprintf(f, ",");
        }

        fprintf(f, "},");
        fprintf(f, ".desirability = %lf,", universe[i].desirability);
        fprintf(f, ".pheromone = %lf,", universe[i].pheromone);
        fprintf(f, ".pdValue = %lf}", universe[i].pdValue);
        if(i < NUM_ITEMS-1) fprintf(f, ",\n");
    }
    fprintf(f, "\n}\n");
    fclose(f);
*/

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    //millisecond value of system clock to seed the RNG
    srand((start.tv_sec)*1000 + (start.tv_usec)/1000);

    Cost best = 0;

    printf("Ant system starting\n");
    
    gettimeofday(&start, NULL);
    //struct Ant ants[num_ants];
    ants = malloc(sizeof(*ants) * num_ants);

#ifndef THREADED
    int32_t it = num_iterations;
    while(it-- > 0){
       
        for(int32_t x = 0; x < num_ants; x++){
            ant_init(&ants[x]);
            ant_buildSolution(&ants[x]);
            if(ants[x].solution.worth > best)
                best = ants[x].solution.worth;
        }
        

        evapPheromones();

        /*
        Pher *pheMatrix = calloc(NUM_ITEMS, sizeof(*pheMatrix));
        for(int32_t i = 0; i < num_ants; i++)
        {
            ant_getDeltaPherMatrix(&ants[i], &pheMatrix);
            ant_fin(&ants[i]);
        }
        ant_updPheromonesMatrix(&pheMatrix);

        free(pheMatrix);*/


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
#else
    int ret;
    struct t_args args[num_threads];
    for(int32_t i = 0; i < num_threads; i++) {
        args[i] = (struct t_args){i};
        ret = pthread_create(&threads[i], &attr, thread_processAnts, &args[i]);
        switch(ret) {
            case EAGAIN:
            case EINVAL:
            case EPERM:
                perror("");
                break;
            case 0:
                printf("\tThread %i criada com sucesso\n", i);
                break;
            default:
                break;
        }
    }

    for(int32_t i = 0; i < num_threads; i++) {
        Cost *localBest;
        ret = pthread_join(threads[i], (void**)(&localBest));
        switch(ret) {
            case EINVAL:
            case ESRCH:
            case EDEADLK:
                perror("");
                break;
            case 0:
                printf("\tThread %i unida com sucesso. localBest: %" PRIi32 "\n",
                        i, *localBest);
                break;
            default:
                break;
        }

        if(*localBest > best)
            best = *localBest;

        free(localBest);
    }

#endif
    gettimeofday(&stop, NULL);

    ldiv_t minsec = ldiv( (stop.tv_sec - start.tv_sec), 60);

    if(minsec.quot > 0){
        printf("Ant system finished in %lim %lis %lims\n",
                minsec.quot, minsec.rem, 
                labs((stop.tv_usec - start.tv_usec)/1000));
    }
    else{
        printf("Ant system finished in %lis %lims\n",
            minsec.rem,
            labs((stop.tv_usec - start.tv_usec)/1000) );
    }

    printf("Best result found: %" PRIi32 "\n", best);

#ifdef THREADED
    barrier_destroy(&barrier);
    pthread_mutex_destroy(&itemMut);
#endif

}


