/**
 * @file CGWMath.c
 * @brief Curses Gaming Window, Mathematical functions
 * 
 * Source code implementing the header.
 */

#include "CGWMath.h"

/**
 * Macro to declare an integer square root function
 * Rather than explicitly write an integer square root function
 * for the various types, a macro is used to generate the code
 * for a static inline function.
 *
 * The algorithm is from Guy, Martin (1985).  The optional
 * pointer, \p rem, can be used to receive the fractional
 * remainder (which is zero for an exact value).
 *
 * @param TYPE      the C type of the function argument and
 *                  return value
 * @param TYPE_NAME the function name component indicating
 *                  the type
 * @return          the largest integer <= the square root of
 *                  the number
 */
#define CGWDefineSqrtFn(TYPE, TYPE_NAME)                        \
    TYPE                                                        \
    CGWSqrt##TYPE_NAME (TYPE x, TYPE *rem) {                    \
        TYPE    result = 0, power_of_four = 1;                  \
                                                                \
        if ( x < 2 ) {                                          \
            if ( rem ) *rem = 0;                                \
            return x;                                           \
        }                                                       \
        while ( power_of_four <= x ) power_of_four <<= 2;       \
        power_of_four >>= 2;                                    \
                                                                \
        while ( power_of_four ) {                               \
            TYPE    dltasqr = result + power_of_four;           \
                                                                \
            if ( x >= dltasqr ) {                               \
                x -= dltasqr;                                   \
                result += (power_of_four << 1);                 \
            }                                                   \
            result >>= 1, power_of_four >>= 2;                  \
        }                                                       \
        if ( rem ) *rem = x;                                    \
        return result;                                          \
    }

/*
 * Per-type integer square root functions:
 */
CGWDefineSqrtFn(int, Int)
CGWDefineSqrtFn(unsigned int, UInt)
CGWDefineSqrtFn(int32_t, Int32)
CGWDefineSqrtFn(int64_t, Int64)
CGWDefineSqrtFn(uint32_t, UInt32)
CGWDefineSqrtFn(uint64_t, UInt64)


#undef CGWDefineSqrtFn