/**
 * @file CGWStats.h
 * @brief Curses Gaming Window, Statistical functions
 * 
 * Data types and functions for statistical stuff.
 */

#ifndef __CGWSTATS_H__
#define __CGWSTATS_H__

#include "CGWConfig.h"

/**
 * Statistical data for a series of n values
 * Rather than retain a growing sequence of values and repeatedly
 * sum over values, the average and variance are updated based upon
 * their previous value and the incoming data point.
 */
typedef struct {
    unsigned int        n;          /*!< the number of samples */
    double              average;    /*!< the average value of the samples */
    double              variance;   /*!< the variance among the samples; INFINITY when only
                                         one sample is in the sequence */
    double              min;        /*!< the minimum value in the sequence */
    double              max;        /*!< the maximum value in the sequence */
} CGWStats;

/**
 * Pointer to a stats struct
 * Type of a pointer to a \ref CGWStats.
 */
typedef CGWStats * CGWStatsPtr;

/**
 * Create an empty stats sequence
 * Returns a \ref CGWStats struct initialized to be an
 * empty sequence:  zero samples, etc.
 * @return      an empty \ref CGWStats struct
 */
static inline
CGWStats
CGWStatsMake()
{
    CGWStats    new_stats = {
                    .n = 0,
                    .average = 0.0,
                    .variance = 0.0,
                    .min = 0.0,
                    .max = 0.0
                };
    return new_stats;
}

/**
 * Add a sample to the sequence
 * Given the sample sequence represted in \p s, add a new sample
 * \p x_i.
 *
 * With the addition of the first sample, all fields except the
 * variance will be updated; the variance will be set to
 * INFINITY (since there is a division by zero in the formula).
 *
 * On each subsequent call, all fields will be updated.
 * @param s         pointer to the \ref CGWStats to which the sample
 *                  will be added
 * @param x_i       the sample to add
 * @return          the pointer to the \ref CGWStats -- can be used
 *                  to chain function calls together, for example
 */
CGWStatsPtr CGWStatsUpdate(CGWStatsPtr s, double x_i);

#endif /* __CGWSTATS_H__ */