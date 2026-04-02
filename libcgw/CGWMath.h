/**
 * @file CGWMath.h
 * @brief Curses Gaming Window, Mathematical functions
 * 
 * Data types and functions for mathematical stuff.
 */

#ifndef __CGWMATH_H__
#define __CGWMATH_H__

#include "CGWConfig.h"

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
#define CGWDeclareSqrtFn(TYPE, TYPE_NAME) \
    TYPE CGWSqrt##TYPE_NAME (TYPE x, TYPE *rem)

/*
 * Per-type integer square root functions:
 */
CGWDeclareSqrtFn(int, Int);
CGWDeclareSqrtFn(unsigned int, UInt);
CGWDeclareSqrtFn(int32_t, Int32);
CGWDeclareSqrtFn(int64_t, Int64);
CGWDeclareSqrtFn(uint32_t, UInt32);
CGWDeclareSqrtFn(uint64_t, UInt64);

#undef CGWDeclareSqrtFn

#endif /* __CGWMATH_H__ */