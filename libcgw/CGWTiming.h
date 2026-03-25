/**
 * @file CGWTiming.h
 * @brief Curses Gaming Window, Timing functions
 * 
 * Data type and functions for handling high-resolution timing
 * calculations.
 *
 */

#ifndef __CGWTIMING_H__
#define __CGWTIMING_H__

#include "CGWUtil.h"

#include <time.h>

/**
 * Number of microseconds
 * A type alias (for a double-precision float) that is used to
 * represent a count of microseconds of time.  This will primarily
 * be used to turn a (sec,nsec) timespec into an
 * easily-manipulated form:  simple conditional tests and addition
 * can be used in lieu of operations on the native integer fields
 * of the timespec struct.
 */
typedef double CGWMicroseconds;

/**
 * Generate a CGWMicroseconds from the current timestamp
 * Calculate the number of microseconds associated with the
 * current timestamp and return a CGWMicroseconds with that
 * value.
 * @return          the number of milliseconds represented
 *                  by \p t
 */
static inline
CGWMicroseconds
CGWMicrosecondsMakeNow()
{
    struct timespec     t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (CGWMicroseconds)(t.tv_sec * 1.0e6 + t.tv_nsec * 1e-3);
}

/**
 * Generate a CGWMicroseconds from a timespec
 * Calculate the number of microseconds associated with the
 * timespec \p t and return a CGWMicroseconds with that
 * value.
 * @param t         a timespec struct
 * @return          the number of milliseconds represented
 *                  by \p t
 */
static inline
CGWMicroseconds
CGWMicrosecondsMakeWithTimespec(
    const struct timespec   t
)
{
    return (CGWMicroseconds)(t.tv_sec * 1.0e6 + t.tv_nsec * 1e-3);
}

/**
 * Track a periodic interval
 * Data structure used to track game timing.  The clock has
 * a frequency (inverse of the cycle_time) that is deemed
 * the "tick" of that clock.
 * 
 * When time is started, the cycle time is used to compute
 * the next tick of the clock.
 *
 * Subsequent higher-frequency checks of the clock will yield
 * boolean true if the next tick has elapsed -- and the time
 * of the next tick will be calculated.
 *
 * When the clock is stopped, the total microseconds between
 * the start of the clock and the current time is added to
 * the accumulated time.  This allows the game to be paused,
 * for example, and a running game time to be calculated
 * regardless.
 */
typedef struct {
    CGWMicroseconds     start_time;         /*!< time when the clock started */
    CGWMicroseconds     cycle_time;         /*!< period of the clock tick */
    CGWMicroseconds     next_cycle;         /*!< next time a tick happens */
    CGWMicroseconds     accumulated_time;   /*!< elapsed time between all starts/stops of the clock */
} CGWTimingInterval;

/**
 * Generate a CGWTimingInterval with a cycle time
 * Initialize and return a CGWTimingInterval with the given
 * \p cycle_time and all other fields set to zero.  The
 * resulting clock can be started, checked, and stopped via
 * macros (without function call overhead).
 * @param cycle_time    the inverse of the clock frequency
 * @return              the initialized clock
 */
static inline
CGWTimingInterval
CGWTimingIntervalMake(
    CGWMicroseconds     cycle_time
)
{
    CGWTimingInterval   new_interval = {
        .start_time = 0.0,
        .cycle_time = cycle_time,
        .next_cycle = 0.0,
        .accumulated_time = 0.0
    };
    return new_interval;
}

/**
 * Generate a CGWTimingInterval with a frequency
 * Initialize and return a CGWTimingInterval with the given
 * \p freq_hz and all other fields set to zero.  The
 * resulting clock can be started, checked, and stopped via
 * macros (without function call overhead).
 * @param freq_hz       the frequency in Hz
 * @return              the initialized clock
 */
static inline
CGWTimingInterval
CGWTimingIntervalMakeWithFrequency(
    double          freq_hz
)
{
    CGWTimingInterval   new_interval = {
        .start_time = 0.0,
        .cycle_time = 1.0 / freq_hz * 1e6,  /* (1 / v) * (10^6 µs / s) */
        .next_cycle = 0.0,
        .accumulated_time = 0.0
    };
    return new_interval;
}

/**
 * Start the clock
 * Set the start time to the current timestamp and calculate the time of the
 * next tick from that time plus the cycle time.
 * @param TI            the clock to affect
 * @return              time of the next tick
 */
#define CGWTimingIntervalStart(TI) \
            ( (TI).next_cycle = ((TI).start_time = CGWMicrosecondsMakeNow()) + \
                                (TI).cycle_time )


/**
 * Check the clock
 * Check if the given timestamp, \p NOW, implies that the next tick
 * of \p TI has elapsed.  If so, calculate the time of the next tick
 * and return true.  Otherwise, return false.
 * @param TI            the clock to check
 * @param NOW           the timestamp to check the clock against; this
 *                      avoids referencing \ref CGWMicrosecondsMakeNow()
 *                      so that a single system clock call can be made
 *                      in the game loop
 * @return              true if the tick had elapsed, false otherwise
 */
#define CGWTimingIntervalCheck(TI, NOW) \
            ( ((TI).next_cycle > (NOW)) ? 0 : \
                    (((TI).next_cycle += (TI).cycle_time) > 0.0) )

/**
 * Stop the clock
 * The total elapsed time between the last start of the clock \p TI
 * and \p NOW is added to the its accumulated time.  A subsequent
 * \ref CGWTimingIntervalStart() will restart the clock.
 * @param TI            the clock to stop
 * @param NOW           the timestamp at which the clock is being
 *                      stopped; see \ref CGWTimingIntervalCheck()
 *                      for the rationale for not just calling 
 *                      \ref CGWMicrosecondsMakeNow() herein
 * @return              the total time accumulated by \p TI
 */
#define CGWTimingIntervalStop(TI, NOW) \
            ( (TI).accumulated_time += (NOW - (TI).start_time) )

/**
 * Stop the clock this moment
 * The total elapsed time between the last start of the clock \p TI
 * and this moment in time is added to the its accumulated time.  A
 * subsequent \ref CGWTimingIntervalStart() will restart the clock.
 * @param TI            the clock to stop
 * @return              the total time accumulated by \p TI
 */
#define CGWTimingIntervalStopNow(TI) \
            ( (TI).accumulated_time += (CGWMicrosecondsMakeNow() - (TI).start_time) )

#endif /* __CGWTIMING_H__ */