////
//Useful functions

#include <stdlib.h>

/**
 * Return a random int between a and b
 * code by theJPester at
 * http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
 */
unsigned int
rand_int(unsigned int min, unsigned int max)
{
    int r;
    
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do
    {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}

/**
 * Return a random double between a and b
 * Straight from the C FAQ http://c-faq.com/lib/fprand.html
 */
double
rand_double(double min, double max)
{
    return min + (rand() / (RAND_MAX /(max-min)));
}

