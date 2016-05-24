////
//Useful functions

#include <stdlib.h>
#include <stdint.h>
#include "const.h"

/**
 * Return a random int between a and b
 * code by theJPester at
 * http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
 */
int32_t
rand_int(int32_t min, int32_t max)
{
    int32_t r;
    
    const int32_t range = 1 + max - min;
    const int32_t buckets = RAND_MAX / range;
    const int32_t limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do
    {
        r = (int32_t)rand();
    } while (r >= limit);

    return min + (r / buckets);
}

/**
 * Return a random double between a and b
 * Straight from the C FAQ http://c-faq.com/lib/fprand.html
 */
float64
rand_double(float64 min, float64 max)
{
    return min + (rand() / (RAND_MAX /(max-min)));
}

