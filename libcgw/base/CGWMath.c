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

//

/**
 * The "magic constant" for the 1/sqrt(x) function
 * The bitwise value of the constant term in the log2 approximation
 * equation for 1/sqrt(x).
 *
 * As a float the value is approx. 1.32118362e+19
 */
#define CGW_INV_SQRT_LOG_CONST_TERM 0x5f3759df

float
CGWQInvSqrtF(
    float           x,
    unsigned int    n_iter
)
{
    int32_t         x_bits;
    float           half_x, y;

    half_x = x * 0.5f;          /* The Newton iteration will repeatedly
                                   use (1/2)x so we will pre-calculate it
                                   once */
                                   
    x_bits = *(int32_t*)&x;     /* Type-cast that allows us to work with
                                   the bitwise form of the float and not
                                   do floating-point arithmetic on it */
                                   
    /*
     * For the mantissa M and exponent E:
     *     x = (1 + 2^(-23)•M) 2^(E-127)
     *     x_bits = M + 2^23•E
     *
     * So:
     *     log2(x) = log2(1 + 2^(-23)•M) + E - 127
     *
     * For x∈[0,1], log2(1+x) ≈ x + µ, so:
     *
     *     log2(x) ≈ 2^(-23)•M + µ + E - 127
     *             ≈ 2^(-23)[M + 2^23E] + µ - 127
     *             ≈ 2^(-23)x_bits + µ - 127
     *
     * Given the function in question:
     *
     *     f(x) = 1/sqrt(x)
     *     log2(f) = (-1/2) log2(x)
     *     2^(-23)f_bits + µ - 127 ≈ (-1/2) [2^(-23)x_bits + µ - 127]
     *     2^(-23)f_bits ≈ (-1/2)(2^(-23))x_bits - (3/2)[µ - 127]
     *     f_bits ≈ (-1/2)x_bits - (3/2)(2^23)[µ - 127]
     *     f_bits ≈ (3/2)(2^23)[127 - µ] - (1/2)x_bits
     *     f_bits ≈ C - (1/2)x_bits
     * 
     * We have a constant, magic_inv_sqrt, and a division by 2 is
     * just a bit shift:
     */
    x_bits = CGW_INV_SQRT_LOG_CONST_TERM - (x_bits >> 1);
    
    y = *(float*)&x_bits;       /* Type-cast f_bits back to a float value */
    
    /*
     * Do Newton's method to refine the result, since we know that
     * the inverse function on y should yield the original argument
     * x:
     *
     *     y = 1/sqrt(x)  =>  x = 1/y^2 = y^(-2)
     *
     * The target function we seek to find a root for is:
     *
     *     f(x) = x_bar - x = x_bar - y^(-2) = f(y)
     *     f'(y) = (2)y^(-3)
     *
     * The Newton steps will look like:
     *
     *     y' = y - f(y)/f'(y)
     *        = y - [x_bar - y^(-2)] / [(2)y^(-3)]
     *        = y - (1/2)(y^3)[x_bar - y^(-2)]
     *        = y - (1/2)(y^3)x_bar + (1/2)y
     *        = (3/2)y - (1/2)(y^3)x_bar
     *        = y•[(3/2) - (1/2)(y^2)x_bar]
     */
    while ( n_iter-- )
        y = y * (1.5f - (half_x * y * y));
        
    return y;
}

#undef CGW_INV_SQRT_LOG_CONST_TERM

//

#define CGW_SQRT_LOG_CONST_TERM 0xe04ab477

float
CGWQSqrtF(
    float           x,
    unsigned int    n_iter
)
{
    int32_t         x_bits;
    float           y;
    
    x_bits = *(int32_t*)&x;       /* Type-cast that allows us to work with
                                   the bitwise form of the float and not
                                   do floating-point arithmetic on it */
    
    /*
     * For the mantissa M and exponent E:
     *     x = (1 + 2^(-23)•M) 2^(E-127)
     *     x_bits = M + 2^23•E
     *
     * So:
     *     log2(x) = log2(1 + 2^(-23)•M) + E - 127
     *
     * For x∈[0,1], log2(1+x) ≈ x + µ, so:
     *
     *     log2(x) ≈ 2^(-23)•M + µ + E - 127
     *             ≈ 2^(-23)[M + 2^23E] + µ - 127
     *             ≈ 2^(-23)x_bits + µ - 127
     *
     * Given the function in question:
     *
     *     f(x) = sqrt(x)
     *     log2(f) = (1/2) log2(x)
     *     2^(-23)f_bits + µ - 127 ≈ (1/2) [2^(-23)x_bits + µ - 127]
     *     2^(-23)f_bits ≈ (1/2)(2^(-23))x_bits - (1/2)[µ - 127]
     *     f_bits ≈ (1/2)x_bits - (1/2)(2^23)[µ - 127]
     *     f_bits ≈ (1/2)x_bits - C
     * 
     * We have a constant, magic_sqrt, and a division by 2 is
     * just a bit shift:
     */
    x_bits = (x_bits >> 1) - CGW_SQRT_LOG_CONST_TERM;
    
    y = *(float*)&x_bits;       /* Type-cast f_bits back to a float value */
    
    /*
     * Do Newton's method to refine the result, since we know that
     * the inverse function on y should yield the original argument
     * x:
     *
     *     y = sqrt(x)  =>  x = y^2
     *
     * The target function we seek to find a root for is:
     *
     *     f(x) = x_bar - x = x_bar - y^2 = f(y)
     *     f'(y) = -2y
     *
     * The Newton steps will look like:
     *
     *     y' = y - f(y)/f'(y)
     *        = y - [x_bar - y^2] / (-2y)
     *        = y + (1/2)(y^(-1))[x_bar - y^2]
     *        = y + (1/2)(x_bar/y) - (1/2)y
     *        = (1/2)(x_bar/y) + (1/2)y
     *        = (1/2)[x_bar/y + y]
     */
    while ( n_iter-- )
        y = 0.5f * ( x/y + y );
    return y;
}

#undef CGW_SQRT_LOG_CONST_TERM

