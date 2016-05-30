#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>

#include "ant.h"
#include "const.h"
#include "item.h"
#include "knapsack.h"
#include "util.h"

int32_t num_iterations = 200;
int32_t num_ants = 100;

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
            {"ants", required_argument, 0, 'n'},
            {"help", no_argument, 0, 'h'},
            {0,0,0,0}
        };

        int option_index = 0;

        c = getopt_long(argc, argv, "v:V:r:R:p:e:a:b:k:i:n:h",
                long_options, &option_index);

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
                    }
                }
                break;
            case 'V':
                {
                    if(optarg){
                        MAX_VALUE = atoi(optarg);
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
                    printf("\t--knap_cap '<value <value>': "
                            "set capacity size for knapsacks\n");
                    printf("\t--iterations <value>: "
                            "set number of iterations of the ant system\n");
                    printf("\t--ants <value>: "
                            "set number of ants of the ant system\n");
                    exit(0);
                }
                break;
        }

    }

    printf("Selected Values:\n");
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
    printf("\tIterations: %" PRIi32 "\n", num_iterations);
    printf("\tAnts: %" PRIi32 "\n", num_ants);
    printf("\n");


}

int main(int argc, char **argv)
{

    setbuf(stdout, NULL);

    //read CLI options
    process_cli(argc, argv);

    create_universe();

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    //millisecond value of system clock to seed the RNG
    srand((start.tv_sec)*1000 + (start.tv_usec)/1000);

    Cost best = 0;
    int32_t it = num_iterations;

    printf("Ant system starting\n");
    
    gettimeofday(&start, NULL);
    struct Ant ants[num_ants];
    while(it-- > 0){
        //struct Ant ants[num_ants];
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
    gettimeofday(&stop, NULL);

    printf("Ant system finished in %lis %lims\n",
            (stop.tv_sec - start.tv_sec),
            labs((stop.tv_usec - start.tv_usec)/1000) );

    printf("Best result found: %" PRIi32 "\n", best);


}


