/**
 * @file CGWUtil.h
 * @brief Curses Gaming Window, Utilities
 * 
 * A header file that pulls-in a number of baseline header files
 * and declares types and functions used throughout the library.
 *
 */

#ifndef __CGWUTIL_H__
#define __CGWUTIL_H__

#include "CGWConfig.h"

/**
 * Byte-ordering variants
 * Enumerates the various byte-orderings possible in
 * a system.
 */
typedef enum {
    kCGWEndianUnset,    /*!< endian has not been determined yet */
    kCGWEndianUnknown,  /*!< tried to determine endian but it's unclear */
    kCGWEndianBE,       /*!< determined the system is big endian */
    kCGWEndianLE        /*!< determined the system is little endian */
} CGWEndian;

/**
 * Detect system byte-ordering
 * On the first call, the byte ordering is determined.  The
 * result is cached and returned henceforth.
 */
static inline
CGWEndian
CGEndianDetect()
{
    static uint16_t     __CGWENDIAN_TEST = 0xcafe;
    static uint8_t      __CGWENDIAN_BE[2] = { 0xca, 0xfe };
    static uint8_t      __CGWENDIAN_LE[2] = { 0xfe, 0xca };
    static CGWEndian    __endian = kCGWEndianUnset;
    
    if ( __endian == kCGWEndianUnset ) {
        if ( __CGWENDIAN_TEST == *((uint16_t*)__CGWENDIAN_BE) )
            __endian = kCGWEndianBE;
        else if ( __CGWENDIAN_TEST == *((uint16_t*)__CGWENDIAN_LE) )
            __endian = kCGWEndianLE;
        else
            __endian = kCGWEndianUnknown;
    }
    return __endian;
}

/**
 * Byte-swap a 16-bit value from BE to host endian
 * If the system is determined to be BE, returns the same
 * \p in_value.  For LE, the two bytes are swapped and
 * that value is returned.
 * @param in_value      the 16-bit value to byte-swap
 * @result              the properly-ordered value
 */
static inline
uint16_t
CGWEndianToHost16Bit(
    uint16_t    in_value
)
{
    if (CGEndianDetect() == kCGWEndianBE) return in_value;
    return ((in_value & 0x00FF) << 8) | ((in_value & 0xFF00) >> 8);
}

/**
 * Byte-swap a 16-bit value
 * The two bytes are swapped and that value is returned.
 * @param in_value      the 16-bit value to byte-swap
 * @result              the reordered value
 */
static inline
uint16_t
CGWEndianSwap16Bit(
    uint16_t    in_value
)
{
    return ((in_value & 0x00FF) << 8) | ((in_value & 0xFF00) >> 8);
}

/**
 * Byte-swap a 32-bit value from BE to host endian
 * If the system is determined to be BE, returns the same
 * \p in_value.  For LE, the four bytes are swapped and
 * that value is returned.
 * @param in_value      the 32-bit value to byte-swap
 * @result              the properly-ordered value
 */
static inline
uint32_t
CGWEndianToHost32Bit(
    uint32_t    in_value
)
{
    if (CGEndianDetect() == kCGWEndianBE) return in_value;
    return ((in_value & 0x000000FF) << 24) | ((in_value & 0x0000FF00) <<  8) |
           ((in_value & 0x00FF0000) >>  8) | ((in_value & 0xFF000000) >> 24);
}

/**
 * Byte-swap a 32-bit value
 * The four bytes are swapped and that value is returned.
 * @param in_value      the 32-bit value to byte-swap
 * @result              the properly-ordered value
 */
static inline
uint32_t
CGWEndianSwap32Bit(
    uint32_t    in_value
)
{
    return ((in_value & 0x000000FF) << 24) | ((in_value & 0x0000FF00) <<  8) |
           ((in_value & 0x00FF0000) >>  8) | ((in_value & 0xFF000000) >> 24);
}

/**
 * Byte-swap a 64-bit value from BE to host endian
 * If the system is determined to be BE, returns the same
 * \p in_value.  For LE, the four bytes are swapped and
 * that value is returned.
 * @param in_value      the 64-bit value to byte-swap
 * @result              the properly-ordered value
 */
static inline
uint64_t
CGWEndianToHost64Bit(
    uint64_t    in_value
)
{
    if (CGEndianDetect() == kCGWEndianBE) return in_value;
    return ((in_value & 0x00000000000000FF) << 56) | ((in_value & 0x000000000000FF00) << 40) |
           ((in_value & 0x0000000000FF0000) << 24) | ((in_value & 0x00000000FF000000) <<  8) |
           ((in_value & 0x000000FF00000000) >>  8) | ((in_value & 0x0000FF0000000000) >> 24) |
           ((in_value & 0x00FF000000000000) >> 40) | ((in_value & 0xFF00000000000000) >> 56);
}

/**
 * Byte-swap a 64-bit value
 * The four bytes are swapped and that value is returned.
 * @param in_value      the 64-bit value to byte-swap
 * @result              the properly-ordered value
 */
static inline
uint64_t
CGWEndianSwap64Bit(
    uint64_t    in_value
)
{
    return ((in_value & 0x00000000000000FF) << 56) | ((in_value & 0x000000000000FF00) << 40) |
           ((in_value & 0x0000000000FF0000) << 24) | ((in_value & 0x00000000FF000000) <<  8) |
           ((in_value & 0x000000FF00000000) >>  8) | ((in_value & 0x0000FF0000000000) >> 24) |
           ((in_value & 0x00FF000000000000) >> 40) | ((in_value & 0xFF00000000000000) >> 56);
}

/**
 * Patterned memory-fill prototype generator
 * Generates the prototype of a function that copies a pattern of bytes
 * of a specific atomic type into a range of memory locations.
 * @param NAME      the suffix to add to the base function name
 *                  CGWMemfill
 * @param TYPE      the atomic type of the pattern
 */
#define CGWMemfillWordOfTypeFnDecl(NAME, TYPE)                                  \
    void                                                                        \
    CGWMemfill##NAME (                                                          \
        const TYPE      pattern,                                                \
        unsigned int    count,                                                  \
        TYPE            *dst                                                    \
    );
    
/**
 * 16-bit pattern copy
 * Prototype of a function that copies a 16-bit value across
 * a range of memory locations.  The generated function name
 * will be \ref CGWMemfillUInt16().
 *
 * The function accepts three arguments:
 * 1. The uint16_t value acting as the pattern
 * 2. The number of uint16_t values to copy
 * 3. The memory location at which the fill should start
 */
CGWMemfillWordOfTypeFnDecl(UInt16, uint16_t)

/**
 * 32-bit pattern copy
 * Prototype of a function that copies a 32-bit value across
 * a range of memory locations.  The generated function name
 * will be \ref CGWMemfillUInt32().
 *
 * The function accepts three arguments:
 * 1. The uint32_t value acting as the pattern
 * 2. The number of uint32_t values to copy
 * 3. The memory location at which the fill should start
 */
CGWMemfillWordOfTypeFnDecl(UInt32, uint32_t)

/**
 * 64-bit pattern copy
 * Prototype of a function that copies a 64-bit value across
 * a range of memory locations.  The generated function name
 * will be \ref CGWMemfillUInt16().
 *
 * The function accepts three arguments:
 * 1. The uint64_t value acting as the pattern
 * 2. The number of uint64_t values to copy
 * 3. The memory location at which the fill should start
 */
CGWMemfillWordOfTypeFnDecl(UInt64, uint64_t)

#undef CGWMemfillWordOfTypeFnDecl

/**
 * Memory fill with an arbitrary pattern
 * Given a sequence of \p pattern_len bytes at \p pattern, create
 * \p count copies of that pattern starting at \p dst.
 *
 * The function includes a variety of optimizations meant to
 * improve speed.
 * @param pattern       pointer to the pattern bytes
 * @param pattern_len   the number of bytes in the pattern
 * @param count         the number of copies to create
 * @param dst           the starting memory location into
 *                      which the pattern should be copied
 */
void CGWMemfill(const void *pattern, size_t pattern_len, unsigned int count, void *dst);

#endif /* __CGWUTIL_H__ */