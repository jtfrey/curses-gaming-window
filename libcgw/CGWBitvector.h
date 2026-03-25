/**
 * @file CGWBitvector.h
 * @brief Curses Gaming Window, Bitvectors
 * 
 * A header file that implements basic bitvector types
 * and operations.
 *
 */

#ifndef __CGWBITVECTOR_H__
#define __CGWBITVECTOR_H__

#include "CGWConfig.h"

#if CGW_BITVECTOR_BITS == 64
#   define CGW_BITVECTOR_TYPE uint64_t
#   define CGW_BITVECTOR_MAX_BIT 0x8000000000000000U
#   define CGW_BITVECTOR_FMT "0x%016llX"
#elif CGW_BITVECTOR_BITS == 32
#   define CGW_BITVECTOR_TYPE uint32_t
#   define CGW_BITVECTOR_MAX_BIT 0x80000000U
#   define CGW_BITVECTOR_FMT "0x%08X"
#elif CGW_BITVECTOR_BITS == 16
#   define CGW_BITVECTOR_TYPE uint16_t
#   define CGW_BITVECTOR_MAX_BIT 0x8000U
#   define CGW_BITVECTOR_FMT "0x%04hX"
#elif CGW_BITVECTOR_BITS == 8
#   define CGW_BITVECTOR_TYPE uint8_t
#   define CGW_BITVECTOR_MAX_BIT 0x80U
#   define CGW_BITVECTOR_FMT "0x%02hhX"
#else
#   error CGW_BITVECTOR_BITS is not in {8, 16, 32, 64}
#endif

/**
 * A fixed-length vector of bits
 * In many instances an integer variable is partitioned
 * into bits with specific meanings.  A CGWBitvector
 * is a uniform type with bitwise operations that can
 * be applied to it.  
 *
 * The bit-depth is determined by the CGW_BITVECTOR_BITS
 * compile-time constant.  The macro CGW_BITVECTOR_MAX_BIT
 * is defined to hold a one bit in the highest possible
 * position for the chosen type.
 */
typedef CGW_BITVECTOR_TYPE CGWBitvector;

/**
 * A bit vector of all zero bits
 */
#define CGWBitvectorZeroes ((CGWBitvector)0U)

/**
 * A bit vector of all one bits
 */
#define CGWBitvectorOnes (~(CGWBitvector)0U)

/**
 * Retain only those bits indicated by a mask
 * Sets \p BV to only those bits associated with \p M.
 * @param BV        a CGWBitvector
 * @param M         the mask of bits to retain
 * @return          the updated value of \p BV
 */
#define CGWBitvectorApplyMask(BV, M) \
    ((BV) &= (M))

/**
 * Value of bit at index
 * Returns the value of \p BV at bit index \p BI.
 * @param BV        a CGWBitvector
 * @param BI        the bit index; bounds are not checked
 * @return          the (unshifted) value at the bit index
 */
#define CGWBitvectorGetBitIndex(BV, BI) \
    ((BV) & ((CGWBitvector)1 << (BI)))

/**
 * Zero the bit at index
 * Alters \p BV to have the bit at index \p BI as a zero.
 * @param BV        a CGWBitvector
 * @param BI        the bit index; bounds are not checked
 * @return          the updated value of \p BV
 */
#define CGWBitvectorSetBitIndexZero(BV, BI) \
    ((BV) &= ~((CGWBitvector)1 << (BI)))

/**
 * Set the bit at index to one
 * Alters \p BV to have the bit at index \p BI as a one.
 * @param BV        a CGWBitvector
 * @param BI        the bit index; bounds are not checked
 * @return          the updated value of \p BV
 */
#define CGWBitvectorSetBitIndexOne(BV, BI) \
    ((BV) |= ((CGWBitvector)1 << (BI)))

/**
 * Set value of bit at index
 * Alters \p BV to have the bit at index \p BI as a one
 * (\p S is true) or zero (\p S is false).
 * @param BV        a CGWBitvector
 * @param BI        the bit index; bounds are not checked
 * @param S         the bit value
 * @return          the updated value of \p BV
 */
#define CGWBitvectorSetBitIndex(BV, BI, S) \
    ((BV) = (S) ? ((BV) | ((CGWBitvector)1 << (BI))) : ((BV) & ~((CGWBitvector)1 << (BI))))

/**
 * Toggle value of bit at index
 * Alters \p BV to have the bit at index \p BI flipped
 * versus its current value.
 * @param BV        a CGWBitvector
 * @param BI        the bit index; bounds are not checked
 * @return          the updated value of \p BV
 */
#define CGWBitvectorToggleBitIndex(BV, BI) \
    ((BV) ^= ((CGWBitvector)1 << (BI)))

/**
 * Value of bits indicated by a mask
 * Returns the value of \p BV at bit indices set in \p M.
 * @param BV        a CGWBitvector
 * @param M         the mask of bits to check
 * @return          the (unshifted) value of the desired bits
 */
#define CGWBitvectorGetBits(BV, M) \
    ((BV) & (M))

/**
 * Set values of bits indicated by a mask
 * Alters \p BV at bit indices set in \p M using bit values
 * in \p V.
 * @param BV        a CGWBitvector
 * @param M         the mask of bits to alter
 * @param V         the bit values to set
 * @return          the updated value of \p BV
 */
#define CGWBitvectorSetBits(BV, M, V) \
    ((BV) = (((BV) & ~(M)) | ((V) & (M))))

/**
 * Toggle values of bits indicated by a mask
 * Alters \p BV as bit indices set in \p M to be flipped
 * versus their current values.
 * @param BV        a CGWBitvector
 * @param M         the mask of bits to alter
 * @return          the updated value of \p BV
 */
#define CGWBitvectorToggleBits(BV, M) \
    ((BV) ^= (M))

/**
 * Is the bit at an index one?
 * Returns a boolean indicating whether or not \p BV has the
 * bit at index \p BI set.
 * @param BV        a CGWBitvector
 * @param BI        the bit index; bounds are not checked
 * @return          true if the bit is set, false if not
 */
#define CGWBitvectorIsSetBitIndex(BV, BI) \
    (CGWBitvectorGetBit(BV, BI) ? true : false)

/**
 * Do the bits associated with a mask have a specific value?
 * Returns a boolean indicating whether or not \p BV has the
 * bits indicated by \p M set the same as \p V.
 * @param BV        a CGWBitvector
 * @param M         the mask of bits to alter
 * @param V         the bit values to check
 * @return          true if the bits match \p V
 */
#define CGWBitvectorAreSetBits(BV, M, V) \
    ((((BV) & (M)) == ((V) & (M))) ? true : false)

/**
 * How many bits are set?
 * Count the number of bits in \p bv that are set (one).
 * @param bv        a CGWBitvector
 * @return          the number of bits that are set
 */
static inline
int
CGWBitvectorCountOnes(
    CGWBitvector    bv
)
{                           /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
    static int      dn[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
    int             n = 0;
    
    while ( bv ) {
        n += dn[bv & 0xF];
        bv >>= 4;
    }
    return n;
}

/**
 * How many bits are unset?
 * Count the number of bits in \p bv that are unset (zero).
 * @param bv        a CGWBitvector
 * @return          the number of bits that are unset
 */
static inline
int
CGWBitvectorCountZeroes(
    CGWBitvector    bv
)
{
    return CGWBitvectorCountOnes(~bv);
}

/**
 * Reverse the bit ordering
 * Bit 0 becomes the highest bit, etc.
 * @param bv        a CBWBitvector
 * @return          the bit vector in reverse sequence
 */
static inline
CGWBitvector
CGWBitvectorReverseOrder(
    CGWBitvector    bv
)
{
    CGWBitvector    rbv = (CGWBitvector)0;
    int             i = sizeof(CGWBitvector) * 8;
    
    while ( i-- ) rbv = (rbv << 1) | (bv & (CGWBitvector)1), bv >>= 1;
    return rbv;
}
    

#endif /* __CGWBITVECTOR_H__ */