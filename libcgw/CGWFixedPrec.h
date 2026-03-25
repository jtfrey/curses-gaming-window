/**
 * @file CGWFixedPrec.h
 * @brief Curses Gaming Window, Fixed-precision math.
 * 
 * Fixed-precision mathematics.
 *
 */

#ifndef __CGWFIXEDPREC_H__
#define __CGWFIXEDPREC_H__

#include "CGWConfig.h"

#if CGW_FIXEDPREC_MIN_BITS <= 8
/**
 * Type of a signed fixed-precision value
 * Dependent on the value of \ref CGW_FIXEDPREC_MIN_BITS this type
 * will be 8-, 16-, 32-, or 64-bits wide.  The lowest \ref
 * CGW_FIXEDPREC_FRAC_BITS bits provide the fractional component.
 */
typedef int8_t CGWFixedPrec;
/**
 * Type of an usigned fixed-precision value
 * As with \ref CGWFixedPrec, the width of this type will vary.
 * It is primarily used when applying bitwise operations to
 * fixed-precision values.
 */
typedef uint8_t CGWUFixedPrec;
/**
 * The actual number of bits in a CGWFixedPrec
 * Bits associated with the selected CGWFixedPrec type.
 */
#   define CGW_FIXEDPREC_BITS 8
/**
 * Significant bits in a CGWFixedPrec
 * Non-sign bits associated with the selected CGWFixedPrec
 * type.
 */
#   define CGW_FIXEDPREC_SIGBITS 7
/**
 * Minimum representable value for a CGWFixedPrec
 * The largest negative value a CGWFixedPrec can adopt.
 */
#   define CGW_FIXEDPREC_MIN ((CGWFixedPrec)INT8_MIN)
/**
 * Maximum representable value for a CGWFixedPrec
 * The largest positive value a CGWFixedPrec can adopt.
 */
#   define CGW_FIXEDPREC_MAX ((CGWFixedPrec)INT8_MAX)
/**
 * Format string for a CGWFixedPrec output
 * When displaying a CGWFixedPrec value in \ref printf()
 * this format string should be used.  The output will be
 * the hexadecimal value, not the real (whole.fractional)
 * form.
 */
#   define CGW_FIXEDPREC_FMT "0x%02hhX"
#elif CGW_FIXEDPREC_MIN_BITS <= 16
typedef int16_t CGWFixedPrec;
typedef uint16_t CGWUFixedPrec;
#   define CGW_FIXEDPREC_BITS 16
#   define CGW_FIXEDPREC_SIGBITS 15
#   define CGW_FIXEDPREC_MIN ((CGWFixedPrec)INT16_MIN)
#   define CGW_FIXEDPREC_MAX ((CGWFixedPrec)INT16_MAX)
#   define CGW_FIXEDPREC_FMT "0x%04hX"
#elif CGW_FIXEDPREC_MIN_BITS <= 32
typedef int32_t CGWFixedPrec;
typedef uint32_t CGWUFixedPrec;
#   define CGW_FIXEDPREC_BITS 32
#   define CGW_FIXEDPREC_SIGBITS 31
#   define CGW_FIXEDPREC_MIN ((CGWFixedPrec)INT32_MIN)
#   define CGW_FIXEDPREC_MAX ((CGWFixedPrec)INT32_MAX)
#   define CGW_FIXEDPREC_FMT "0x%08X"
#elif CGW_FIXEDPREC_MIN_BITS <= 64
typedef int64_t CGWFixedPrec;
typedef uint64_t CGWUFixedPrec;
#   define CGW_FIXEDPREC_BITS 64
#   define CGW_FIXEDPREC_SIGBITS 63
#   define CGW_FIXEDPREC_MIN ((CGWFixedPrec)INT64_MIN)
#   define CGW_FIXEDPREC_MAX ((CGWFixedPrec)INT64_MAX)
#   define CGW_FIXEDPREC_FMT "0x%016llX"
#endif

/**
 * Fixed precision zero
 * The number zero as the fixed-precision type.
 */
#define CGWFixedPrecZero        ((CGWFixedPrec)0)
/**
 * Fixed precision one
 * The number one as the fixed-precision type.
 */
#define CGWFixedPrecOne         ((CGWFixedPrec)((CGWUFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS))
/**
 * Fixed precision negative one
 * The number negative one as the fixed-precision type.
 */
#define CGWFixedPrecNegOne      ((CGWFixedPrec)(CGW_FIXEDPREC_MAX << CGW_FIXEDPREC_FRAC_BITS))
/**
 * Fixed precision two
 * The number two as the fixed-precision type.
 */
#define CGWFixedPrecTwo         ((CGWFixedPrec)((CGWUFixedPrec)1 << (CGW_FIXEDPREC_FRAC_BITS + 1)))
#if CGW_FIXEDPREC_FRAC_BITS > 0
/**
 * Fixed precision one half
 * The number one half (0.5) as the fixed-precision type.  This
 * macro is only defined if \ref CGW_FIXEDPREC_FRAC_BITS is at
 * least 1 bit.
 */
#define CGWFixedPrecOneHalf     ((CGWFixedPrec)((CGWUFixedPrec)1 << (CGW_FIXEDPREC_FRAC_BITS - 1)))
#if CGW_FIXEDPREC_FRAC_BITS > 1
/**
 * Fixed precision one quarter
 * The number one quarter (0.25) as the fixed-precision type.  This
 * macro is only defined if \ref CGW_FIXEDPREC_FRAC_BITS is at
 * least 2 bits.
 */
#define CGWFixedPrecOneQuarter  ((CGWFixedPrec)(((CGWUFixedPrec)1 << (CGW_FIXEDPREC_FRAC_BITS - 2))))
#if CGW_FIXEDPREC_FRAC_BITS > 2
/**
 * Fixed precision one eighth
 * The number one eighth (0.125) as the fixed-precision type.  This
 * macro is only defined if \ref CGW_FIXEDPREC_FRAC_BITS is at
 * least 3 bits.
 */
#define CGWFixedPrecOneEighth   ((CGWFixedPrec)((CGWUFixedPrec)1 << (CGW_FIXEDPREC_FRAC_BITS - 3)))
#endif
#endif
#endif

/**
 * The largest positive whole value
 * The maximum positive whole value that a CGWFixedPrec can
 * represent.
 */
#define CGWFixedPrecMaxWholeComp ((CGWFixedPrec)CGW_FIXEDPREC_MAX >> CGW_FIXEDPREC_FRAC_BITS)

/**
 * Fixed precision error codes
 * The fixed-precision functions will set \ref cgw_fperrno in case
 * of error.  It will be set to values from this enumeration.
 */
typedef enum {
    kCGWFixedPrecErrorNone      = 0,    /*!< no error */
    kCGWFixedPrecErrorOverflow,         /*!< value exceeded the largest representable value */
    kCGWFixedPrecErrorUnderflow         /*!< value was smaller than the smallest representable
                                             value */
} CGWFixedPrecError;

/**
 * Global fixed-precision error register
 * Set by all fixed-precision functions to indicate the success
 * or failure of an operation.
 */
extern CGWFixedPrecError cgw_fperrno;

/**
 * Reset the fixed-precision error register
 * Sets the register to indicate no error condition.
 */
#define CGWFixedPrecErrorReset() (cgw_fperrno = kCGWFixedPrecErrorNone)

/**
 * Convert an int to a CGWFixedPrec
 * Attempt to convert the int value \p i to a \ref CGWFixedPrec.
 * The \ref cgw_fperrno will be updated.
 *
 * If overflow occurs then the maximum negative or maximum positive
 * value will be returned.
 * @param i         the value to be converted
 * @return          the CGWFixedPrec value
 */
static inline
CGWFixedPrec
CGWFixedPrecCreateWithInt(
    int             i
)
{
#ifndef CGW_FIXEDPREC_FASTMATH
    if ( i < (-CGWFixedPrecMaxWholeComp - 1) ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return CGW_FIXEDPREC_MIN;
    }
    else if ( i > CGWFixedPrecMaxWholeComp ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return CGW_FIXEDPREC_MAX;
    }
    else {
        cgw_fperrno = kCGWFixedPrecErrorNone;
    }
#endif
    return (CGWFixedPrec)(i << CGW_FIXEDPREC_FRAC_BITS);
}

/**
 * Convert a CGWFixedPrec to an int
 * Returns the value of fixed-precision number \p n as an
 * int, dropping the fractional component.
 *
 * If overflow occurs, the maximum negative or maximum positive
 * int value is returned.
 * @param n         the fixed-precision value
 * @return          the int value
 */
static inline
int
CGWFixedPrecToInt(
    CGWFixedPrec    n
)
{
#if INT_SIZE_BITS < (CGW_FIXEDPREC_BITS - CGW_FIXEDPREC_FRAC_BITS)
    int             neg = (n < 0) ? (n=-n, 1) : 0;
    
    n >>= CGW_FIXEDPREC_FRAC_BITS;
#   ifndef CGW_FIXEDPREC_FASTMATH
    if ( neg ) {
        if ( n > INT_MAX + 1 ) {
            cgw_fperrno = kCGWFixedPrecErrorOverflow;
            return INT_MIN;
        }
    }
    else if ( n > INT_MAX ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return INT_MAX;
    }
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
    if ( neg ) n = -n;
#else
#   ifndef CGW_FIXEDPREC_FASTMATH
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
    n >>= CGW_FIXEDPREC_FRAC_BITS;
#endif
    return (int)n;
}

/**
 * Convert an unsigned int to a CGWFixedPrec
 * Attempt to convert the unsigned int value \p i to a \ref CGWFixedPrec.
 * The \ref cgw_fperrno will be updated.
 *
 * If overflow occurs then the maximum negative or maximum positive
 * value will be returned.
 * @param i         the value to be converted
 * @return          the CGWFixedPrec value
 */
static inline
CGWFixedPrec
CGWFixedPrecCreateWithUnsignedInt(
    unsigned int        i
)
{
#ifndef CGW_FIXEDPREC_FASTMATH
    if ( i > CGWFixedPrecMaxWholeComp ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return CGW_FIXEDPREC_MAX;
    }
    else {
        cgw_fperrno = kCGWFixedPrecErrorNone;
    }
#endif
    return (CGWFixedPrec)(i << CGW_FIXEDPREC_FRAC_BITS);
}

/**
 * Convert a CGWFixedPrec to an unsigned int
 * Returns the value of fixed-precision number \p n as an
 * unsigned int, dropping the fractional component.
 *
 * If overflow occurs, the maximum positive unsigned int
 * value is returned.
 * @param n         the fixed-precision value
 * @return          the unsigned int value
 */
static inline
unsigned int
CGWFixedPrecToUnsignedInt(
    CGWFixedPrec    n
)
{
    if ( n < 0 ) {
#ifndef CGW_FIXEDPREC_FASTMATH
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
#endif
        return 0;
    }
    
    n >>= CGW_FIXEDPREC_FRAC_BITS;
        
#if INT_SIZE_BITS < (CGW_FIXEDPREC_BITS - CGW_FIXEDPREC_FRAC_BITS)
#   ifndef CGW_FIXEDPREC_FASTMATH
    if ( n > UINT_MAX ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return UINT_MAX;
    }
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
#else
#   ifndef CGW_FIXEDPREC_FASTMATH
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
#endif
    return (unsigned)n;
}

/**
 * Convert an long int to a CGWFixedPrec
 * Attempt to convert the long int value \p i to a \ref CGWFixedPrec.
 * The \ref cgw_fperrno will be updated.
 *
 * If overflow occurs then the maximum negative or maximum positive
 * value will be returned.
 * @param i         the value to be converted
 * @return          the CGWFixedPrec value
 */
static inline
CGWFixedPrec
CGWFixedPrecCreateWithLongInt(
    long int        i
)
{
#ifndef CGW_FIXEDPREC_FASTMATH
    if ( i < (-CGWFixedPrecMaxWholeComp - 1) ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return CGW_FIXEDPREC_MIN;
    }
    else if ( i > CGWFixedPrecMaxWholeComp ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return CGW_FIXEDPREC_MAX;
    }
    else {
        cgw_fperrno = kCGWFixedPrecErrorNone;
    }
#endif
    return (CGWFixedPrec)(i << CGW_FIXEDPREC_FRAC_BITS);
}

/**
 * Convert a CGWFixedPrec to an long int
 * Returns the value of fixed-precision number \p n as a
 * long int, dropping the fractional component.
 *
 * If overflow occurs, the maximum negative or maximum positive
 * long int value is returned.
 * @param n         the fixed-precision value
 * @return          the long int value
 */
static inline
long int
CGWFixedPrecToLongInt(
    CGWFixedPrec    n
)
{
#if LONG_INT_SIZE_BITS < (CGW_FIXEDPREC_BITS - CGW_FIXEDPREC_FRAC_BITS)
    int             neg = (n < 0) ? (n=-n, 1) : 0;
    
    n >>= CGW_FIXEDPREC_FRAC_BITS;
#   ifndef CGW_FIXEDPREC_FASTMATH
    if ( neg ) {
        if ( n > LONG_MAX + 1 ) {
            cgw_fperrno = kCGWFixedPrecErrorOverflow;
            return LONG_MIN;
        }
    }
    else if ( n > LONG_MAX ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return LONG_MAX;
    }
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
    if ( neg ) n = -n;
#else
#   ifndef CGW_FIXEDPREC_FASTMATH
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
    n >>= CGW_FIXEDPREC_FRAC_BITS;
#endif
    return (long int)n;
}

/**
 * Convert an unsigned long int to a CGWFixedPrec
 * Attempt to convert the unsigned long int value \p i to a \ref CGWFixedPrec.
 * The \ref cgw_fperrno will be updated.
 *
 * If overflow occurs then the maximum negative or maximum positive
 * value will be returned.
 * @param i         the value to be converted
 * @return          the CGWFixedPrec value
 */
static inline
CGWFixedPrec
CGWFixedPrecCreateWithUnsignedLongInt(
    unsigned long int   i
)
{
#ifndef CGW_FIXEDPREC_FASTMATH
    if ( i > CGWFixedPrecMaxWholeComp ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return CGW_FIXEDPREC_MAX;
    }
    else {
        cgw_fperrno = kCGWFixedPrecErrorNone;
    }
#endif
    return (CGWFixedPrec)(i << CGW_FIXEDPREC_FRAC_BITS);
}

/**
 * Convert a CGWFixedPrec to an unsigned long int
 * Returns the value of fixed-precision number \p n as an
 * unsigned long int, dropping the fractional component.
 *
 * If overflow occurs, the maximum positive unsigned long int
 * value is returned.
 * @param n         the fixed-precision value
 * @return          the unsigned long int value
 */
static inline
unsigned long int
CGWFixedPrecToUnsignedLongInt(
    CGWFixedPrec    n
)
{
    if ( n < 0 ) {
#ifndef CGW_FIXEDPREC_FASTMATH
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
#endif
        return 0;
    }
    
    n >>= CGW_FIXEDPREC_FRAC_BITS;
        
#if LONG_INT_SIZE_BITS < (CGW_FIXEDPREC_BITS - CGW_FIXEDPREC_FRAC_BITS)
#   ifndef CGW_FIXEDPREC_FASTMATH
    if ( n > ULONG_MAX ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        return ULONG_MAX;
    }
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
#else
#   ifndef CGW_FIXEDPREC_FASTMATH
    cgw_fperrno = kCGWFixedPrecErrorNone;
#   endif
#endif
    return (unsigned long int)n;
}

/**
 * Convert a float to a CGWFixedPrec
 * Attempt to convert the single-precision floating point
 * value \p f to a \ref CGWFixedPrec.  The \ref cgw_fperrno
 * will be updated.
 *
 * If underflow occurs then zero will be returned.  If
 * overflow occurs then the a maximum negative or
 * maximum positive value will be returned.
 * @param f         the value to be converted
 * @return          the CGWFixedPrec value
 */
static inline
CGWFixedPrec
CGWFixedPrecCreateWithFloat(
    float       f
)
{
#ifndef CGW_FIXEDPREC_FASTMATH
    CGWFixedPrec    n = 0;
    
    if ( f == 0 ) return n;
    
    /* Multiply by shift factor: */
    f *= (float)((CGWFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS);
    n = (CGWFixedPrec)f;
    
    if ( f < (float)CGW_FIXEDPREC_MIN ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        n = CGW_FIXEDPREC_MIN;
    }
    else if ( f > (float)CGW_FIXEDPREC_MAX ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        n = CGW_FIXEDPREC_MAX;
    }
    else if ( n == 0 ) {
        cgw_fperrno = kCGWFixedPrecErrorUnderflow;
    }
    else {
        cgw_fperrno = kCGWFixedPrecErrorNone;
    }
    return n;
#else
    return (CGWFixedPrec)(f * (float)((CGWFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS));
#endif
}

/**
 * Convert a CGWFixedPrec to a float
 * Returns the value of fixed-precision number \p n as a
 * single-precision floating point value.
 * @param n         the fixed-precision value
 * @return          the floating-point value
 */
static inline
float
CGWFixedPrecToFloat(
    CGWFixedPrec    n
)
{
    return (float)(n) / (float)((CGWFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS);
}

/**
 * Convert a double to a CGWFixedPrec
 * Double-precision variant of \ref CGWFixedPrecCreateWithFloat().
 * @param f         the value to be converted
 * @return          the CGWFixedPrec value
 */
static inline
CGWFixedPrec
CGWFixedPrecCreateWithDouble(
    double      f
)
{
#ifndef CGW_FIXEDPREC_FASTMATH
    CGWFixedPrec    n = 0;
    
    if ( f == 0 ) return n;
    
    /* Multiply by shift factor: */
    f *= (double)((CGWFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS);
    n = (CGWFixedPrec)f;
    
    if ( f < (double)CGW_FIXEDPREC_MIN ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        n = CGW_FIXEDPREC_MIN;
    }
    else if ( f > (double)CGW_FIXEDPREC_MAX ) {
        cgw_fperrno = kCGWFixedPrecErrorOverflow;
        n = CGW_FIXEDPREC_MAX;
    }
    else if ( n == 0 ) {
        cgw_fperrno = kCGWFixedPrecErrorUnderflow;
    }
    else {
        cgw_fperrno = kCGWFixedPrecErrorNone;
    }
    return n;
#else
    return (CGWFixedPrec)(f * (double)((CGWFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS));
#endif
}


/**
 * Convert a CGWFixedPrec to a double
 * Double-precision variant of \ref CGWFixedPrecToFloat().
 * @param n         the fixed-precision value
 * @return          the floating-point value
 */
static inline
double
CGWFixedPrecToDouble(
    CGWFixedPrec    n
)
{
    return (double)(n) / (double)((CGWFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS);
}

/**
 * Absolute value
 * Returns the absolute value of \p X.
 * @param X     a fixed-precision value
 * @return      |X|
 */
#define CGWFixedPrecAbs(X) ((X < 0) ? (-X) : (X))

/**
 * Drop all fractional digits from a fixed-point value
 * Zeroes all bits in the fractional component of the fixed-point
 * value \ref a.
 * @param a         a fixed-point number
 * @return          \ref a with all fractional bits zeroed
 */
static inline
CGWFixedPrec
CGWFixedPrecExtractWhole(
    CGWFixedPrec    a
)
{
    int             neg = (a < 0) ? (a=-a, 1) : 0;

#ifndef CGW_FIXEDPREC_FASTMATH
    cgw_fperrno = kCGWFixedPrecErrorNone;
#endif
    a = a & ~(((CGWUFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS) - 1);
    return neg ? -a : a;
}

/**
 * Drop all whole digits from a fixed-point value
 * Zeroes all bits in the whole component of the fixed-point
 * value \ref a.
 * @param a         a fixed-point number
 * @return          \ref a with all whole bits zeroed
 */
static inline
CGWFixedPrec
CGWFixedPrecExtractFrac(
    CGWFixedPrec    a
)
{
    int             neg = (a < 0) ? (a=-a, 1) : 0;
    
#ifndef CGW_FIXEDPREC_FASTMATH
    cgw_fperrno = kCGWFixedPrecErrorNone;
#endif
    a = a & (((CGWUFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS) - 1);
    return neg ? -a : a;
}

/**
 * Split a fixed-precision number into whole and fractional components
 * The fixed-precision number \ref a is decomposed into a whole
 * component (copied to the CGWFixedPrec that \p whole points to)
 * and fractional component (as the return value).
 * @param a         a fixed-point number
 * @param whole     pointer to the CGWFixedPrec that will be set
 *                  to the whole component of \p a
 * @return          the fractional component of \p a
 */
static inline
CGWFixedPrec
CGWFixedPrecMod(
    CGWFixedPrec    a,
    CGWFixedPrec    *whole
)
{
    int             neg = (a < 0) ? (a=-a, 1) : 0;
    CGWFixedPrec    f = a;
    
#ifndef CGW_FIXEDPREC_FASTMATH
    cgw_fperrno = kCGWFixedPrecErrorNone;
#endif
    a = a & ~(((CGWUFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS) - 1);
    f = f & (((CGWUFixedPrec)1 << CGW_FIXEDPREC_FRAC_BITS) - 1);
    *whole = neg ? -a : a;
    return neg ? -f : f;
}

/**
 * Add two fixed-precision values
 * The sum of \p a and \p b is calculated.  If the resulting
 * value is too large for CGWFixedPrec, \ref cgw_fperrno will
 * be set to indicate overflow and the appropriate maximum
 * negative/positive value returned.
 * @param a         addend 1
 * @param a         addend 2
 * @return          the sum of \p a and \p b
 */
static inline
CGWFixedPrec
CGWFixedPrecAdd(
    CGWFixedPrec    a,
    CGWFixedPrec    b
)
{
    CGWFixedPrec    sum = a + b;
#ifndef CGW_FIXEDPREC_FASTMATH
    int             sign_a = (a < 0) ? 1 : 0;
    int             sign_b = (b < 0) ? 1 : 0;
    
    if ( (sign_a ^ sign_b) == 0 ) {
        /* Same sign inputs... */
        if ( sign_a && (sum > 0) ) {
            /* Negative inputs, sum is positive: */
            cgw_fperrno = kCGWFixedPrecErrorOverflow;
            sum = CGW_FIXEDPREC_MIN;
        }
        else if ( sum < 0 ) {
            /* Positive inputs, sum is negative: */
            /* Negative inputs, sum is positive: */
            cgw_fperrno = kCGWFixedPrecErrorOverflow;
            sum = CGW_FIXEDPREC_MAX;
        }
        else {
            cgw_fperrno = kCGWFixedPrecErrorNone;
        }
    } else {
        /* Alternate sign inputs, sum is always okay: */
        cgw_fperrno = kCGWFixedPrecErrorNone;
    }
#endif
    return sum;
}

/**
 * Multiply a fixed-precision value by a power of two
 * Multiply \p a by 2^(\p power_of_two).  The \p power_of_two
 * can be negative or positive.
 *
 * If the product is too small to be represented by a
 * CGWFixedPrec then \ref cgw_fperrno is set to indicate an
 * underflow and zero is returned.
 *
 * If the product is too large to be represented by a
 * CGWFixedPrec then \ref cgw_fperrno is set to indicate an
 * overflow and the maximum negative/positive value is returned.
 * @param a             a fixed-precision number
 * @param power_of_two  the power of two by which to multiply
 *                      \p a
 * @return              the product
 */
static inline
CGWFixedPrec
CGWFixedPrecMulPow2(
    CGWFixedPrec    a,
    int             power_of_two
)
{
    if (power_of_two == 0) {
#ifndef CGW_FIXEDPREC_FASTMATH
        cgw_fperrno = kCGWFixedPrecErrorNone;
#endif
        return a;
    }
    else if ( power_of_two > 0 ) {
        int         neg = ((a < 0) ? (a=-a, 1) : 0);
        
#ifdef CGW_FIXEDPREC_FASTMATH
        a <<= power_of_two;
#else
        while ( power_of_two-- ) {
            if ( (a <<= 1) < 0 ) {
                cgw_fperrno = kCGWFixedPrecErrorOverflow;
                return (neg ? CGW_FIXEDPREC_MIN : CGW_FIXEDPREC_MAX);
            }
        }
        cgw_fperrno = kCGWFixedPrecErrorNone;
#endif
        return (neg ? -a : a);
    }
    else {
        int         neg = ((a < 0) ? (a=-a, 1) : 0);
        
#ifdef CGW_FIXEDPREC_FASTMATH
        a >>= -power_of_two;
#else
        while ( power_of_two++ ) {
            if ( (a >>= 1) == 0 ) {
                cgw_fperrno = kCGWFixedPrecErrorUnderflow;
                return 0;
            }
        }
        cgw_fperrno = kCGWFixedPrecErrorNone;
#endif
        return (neg ? -a : a);
    }
}

/**
 * Multiply to fixed-precision numbers
 * Multiply \p a by \p b.
 *
 * If the product is too small to be represented by a
 * CGWFixedPrec then \ref cgw_fperrno is set to indicate an
 * underflow and zero is returned.
 *
 * If the product is too large to be represented by a
 * CGWFixedPrec then \ref cgw_fperrno is set to indicate an
 * overflow and the maximum negative/positive value is returned.
 * @param a             multiplicand 1
 * @param a             multiplicand 2
 * @return              the product
 */
static inline
CGWFixedPrec
CGWFixedPrecMul(
    CGWFixedPrec    a,
    CGWFixedPrec    b
)
{
#ifdef CGW_FIXEDPREC_FASTMATH
    return ((a * b) >> CGW_FIXEDPREC_FRAC_BITS);
#else
    cgw_fperrno = kCGWFixedPrecErrorNone;
    if ( a == 0 || b == 0 ) {
        return 0;
    } else {
        CGWFixedPrec    tmp;
        int             neg_a = ((a < 0) ? (a=-a, 1) : 0);
        int             neg_b = ((b < 0) ? (b=-b, 1) : 0);
        int             power_of_two = -CGW_FIXEDPREC_FRAC_BITS;
        
        if ( a < b ) tmp = b, b = a, a = tmp;
        tmp = 0;
        while ( b && (power_of_two <= (CGW_FIXEDPREC_SIGBITS - CGW_FIXEDPREC_FRAC_BITS)) &&(cgw_fperrno == kCGWFixedPrecErrorNone) ) {
            if ( b & (CGWFixedPrec)0x1 ) {
                tmp = CGWFixedPrecAdd(tmp, CGWFixedPrecMulPow2(a, power_of_two));
            }
            b >>= 1;
            power_of_two++;
        }
        return (neg_a ^ neg_b) ? -tmp : tmp;
    }
#endif
}

#endif /* __CGWFIXEDPREC_H__ */
