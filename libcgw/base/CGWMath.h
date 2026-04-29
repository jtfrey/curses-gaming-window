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

/**
 * The infamous Quake III inverse square root
 * Approximates the inverse square root of \p x using
 * a (very good) guess and Newton refinement.
 * @param x         the radicand
 * @param n_iter    the number of Newton
 *                  iterations to perform
 * @return          an approximation of the inverse
 *                  square root of \p x
 */
float CGWQInvSqrtF(float x, unsigned int n_iter);

/**
 * Quake III method for the square root
 * Approximates the square root of \p x using a (pretty
 * good) guess and Newton refinement.
 * @param x         the radicand
 * @param n_iter    the number of Newton
 *                  iterations to perform
 * @return          an approximation of the square
 *                  root of \p x
 */
float CGWQSqrtF(float x, unsigned int n_iter);

#if defined(__APPLE__) && defined(__aarch64__)

/**
 * Hand-coded Apple Silicon assembly for CGWQSqrtF
 * The \ref CGWQSqrtF() function written explicitly in
 * Apple Silicon assembly.
 * @param x         the radicand
 * @param n_iter    the number of Newton
 *                  iterations to perform
 * @return          an approximation of the square
 *                  root of \p x
 */
extern float CGWQSqrtFAsm(float x, int n_iter);

#endif

/**
 * The value two time pi
 * The standard math headers are missing the value of
 * 2π, so it is included here.
 */
#define M_2xPI      6.283185307179586232

/**
 * Round a number toward infinity
 * A negative number if rounded toward –∞ and a positive
 * (or zero) number is rounted toward +∞.
 * @param x         the number to round; 
 * @return          the rounded value
 */
static inline
double
CGWRoundInf(
    double  x
)
{
    return ( x < 0 ) ? floor(x) : ceil(x);
}

#endif /* __CGWMATH_H__ */