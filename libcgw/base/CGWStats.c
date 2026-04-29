/**
 * @file CGWStats.c
 * @brief Curses Gaming Window, Statistical functions
 * 
 * Source to accompany CGWStats.h.
 */

#include "CGWStats.h"

/* 

    avg = Sum(x_i, i=1…n) / n
    
    var = Sum((x_i - avg)^2, i=1…n)/(n - 1)
        = 1/(n - 1) Sum( (x_i - avg)^2, i=1…n )
        = 1/(n - 1) Sum(x_i^2 - 2 avg x_i + avg^2, i=1…n)
        = 1/(n - 1)( n avg^2 - 2 avg Sum(x_i, 1=1…n) + Sum(x_i^2, i=1…n) )
    => Sum(x_i, 1=1…n) = n avg
        = 1/(n - 1)( n avg^2 - 2 avg Sum(x_i, 1=1…n) + Sum(x_i^2, i=1…n) )
        = 1/(n - 1)( Sum(x_i^2, i=1…n) - n avg^2 )
    
    
    Additional data point: x_(i+1) with n' = n + 1
    
        avg' = Sum(x_i, i=1…n+1) / (n+1)
             = 1/(n+1) (Sum(x_i, i=1…n) + x_n+1)
        => Sum(x_i, i=1…n) = n avg
             = 1/(n+1) (n avg + x_n+1)
             = (n/(n+1)) avg + (1/(n+1)) x_n+1
    
    
        var' = Sum((x_i - avg')^2, i=1…n+1) / n
             = (1/n) Sum(x_i^2 - 2 x_i avg' + avg'^2, i=1…n+1)
             = (1/n)( (n+1) avg'^2 - 2 avg' Sum(x_i, i=1…n+1) + Sum(x_i^2, i=1…n+1) )
        => Sum(x_i, i=1…n+1) = (n + 1) avg'
             = (1/n)( (n+1) avg'^2 - 2(n+1) avg'^2 + Sum(x_i^2, i=1…n) + x_n+1^2 )
             = (1/n)( Sum(x_i^2, i=1…n) + x_n+1^2 - (n+1) avg'^2 )
        => Sum(x_i^2, i=1…n) = (n - 1) var + n avg^2
             = (1/n)( (n - 1) var + n avg^2 + x_n+1^2 - (n+1) avg'^2 )
             = ((n-1)/n) var + avg^2 + (1/n) x_n+1^2 - ((n+1)/n) avg'^2

 */

CGWStatsPtr
CGWStatsUpdate(
    CGWStatsPtr s,
    double      x_i
)
{
    switch ( s->n ) {
        case 0: {
            s->average = s->min = s->max = x_i;
            s->variance = INFINITY;
            break;
        }
        case 1: {
            /* Since there is just one sample, the average IS
             * the previous sample, so stash that in v2 for now:
             */
            double      v1, v2 = s->average;
            
            /* Compute the new average: */
            s->average = 0.5 * (s->average + x_i);
            /* Difference of sample 1 (using the value of sample 1 we
             * stashed in v2) and new average: */
            v1 = v2 - s->average;
            /* Difference of sample 2 and new average: */
            v2 = x_i - s->average;
            /* Variance is the sum of squares of difference divided by
             * (n - 2), but since n = 2, the divisor is just 1: */
            s->variance = v1 * v1 + v2 * v2;
            /* Update min/max: */
            if ( x_i < s->min ) s->min = x_i;
            else if ( x_i > s->max ) s->max = x_i;
            break;
        }
        default: {
            /* Precalculate the scaling factors we'll need: */
            double  inv_n_plus_one = 1.0 / (double)(s->n + 1);
            double  n_over_n_plus_one = (double)s->n * inv_n_plus_one;
            double  n_minus_one_over_n = (double)(s->n - 1) / (double)s->n;
            
            /* Calculate the portion of the variance update that does
             * not depend on the new average: */
            double  base_var = s->average * s->average + n_minus_one_over_n * s->variance + \
                                x_i * x_i / (double)s->n;
            /* Update the average: */
            s->average = n_over_n_plus_one * s->average + inv_n_plus_one * x_i;
            /* Update the variance: */
            s->variance = base_var - s->average * s->average / n_over_n_plus_one;
            /* Update min/max: */
            if ( x_i < s->min ) s->min = x_i;
            else if ( x_i > s->max ) s->max = x_i;
            break;
        }
    }
    /* We've added another sample: */
    s->n++;
    return s;
}