/**
 * @file CGWUtil.c
 * @brief Curses Gaming Window, Utilities
 * 
 * Source code accompanying CGWUtil.h.
 *
 */
 
#include "CGWUtil.h"

#define CGWMemfillWordOfTypeFnDef(NAME, TYPE)                                   \
    void                                                                        \
    CGWMemfill##NAME (                                                          \
        const TYPE      pattern,                                                \
        unsigned int    count,                                                  \
        TYPE            *dst                                                    \
    )                                                                           \
    {                                                                           \
        TYPE           *d = (TYPE*)dst;                                         \
        if ( count * sizeof(TYPE) >= CGWMEMFILL_INITIAL_THRESHOLD_BYTES  ) {    \
            unsigned int    n = 5, div = 5, rem = count % 5;                    \
            while ( rem && (n-- > 1) ) {                                        \
                if ( (count % n) < rem ) {                                      \
                    div = n;                                                    \
                    rem = count % n;                                            \
                }                                                               \
            }                                                                   \
            if ( n == 1 ) {                                                     \
                div = 1, rem = count;                                           \
            }                                                                   \
            else if ( div == 5 ) {                                              \
                unsigned int    one_fifth = count / 5;                          \
                count = one_fifth;                                              \
                while ( count-- ) *d++ = pattern;                               \
                memcpy(d, (TYPE*)dst, one_fifth * sizeof(TYPE));                \
                d += one_fifth;                                                 \
                memcpy(d, (TYPE*)dst, 2 * one_fifth * sizeof(TYPE));            \
                d += 2 * one_fifth;                                             \
                memcpy(d, (TYPE*)dst, one_fifth * sizeof(TYPE));                \
                d += one_fifth;                                                 \
            }                                                                   \
            else if ( div == 4 ) {                                              \
                unsigned int    one_quarter = count / 4;                        \
                count = one_quarter;                                            \
                while ( count-- ) *d++ = pattern;                               \
                memcpy(d, (TYPE*)dst, one_quarter * sizeof(TYPE));              \
                d += one_quarter;                                               \
                memcpy(d, (TYPE*)dst, 2 * one_quarter * sizeof(TYPE));          \
                d += 2 * one_quarter;                                           \
            }                                                                   \
            else if ( div == 3 ) {                                              \
                unsigned int    one_third = count / 3;                          \
                count = one_third;                                              \
                while ( count-- ) *d++ = pattern;                               \
                memcpy(d, (TYPE*)dst, one_third * sizeof(TYPE));                \
                d += one_third;                                                 \
                memcpy(d, (TYPE*)dst, one_third * sizeof(TYPE));                \
                d += one_third;                                                 \
                memcpy(d, (TYPE*)dst, one_third * sizeof(TYPE));                \
                d += one_third;                                                 \
            }                                                                   \
            else if ( div == 2 ) {                                              \
                unsigned int    one_half = count / 2;                           \
                count = one_half;                                               \
                while ( count-- ) *d++ = pattern;                               \
                memcpy(d, (TYPE*)dst, one_half * sizeof(TYPE));                 \
                d += one_half;                                                  \
            }                                                                   \
            while ( rem-- ) *d++ = pattern;                                     \
        } else {                                                                \
            while ( count-- ) *d++ = pattern;                                   \
        }                                                                       \
    }

/* CGWMemfillUInt16 */
CGWMemfillWordOfTypeFnDef(UInt16, uint16_t)
/* CGWMemfillUInt32 */
CGWMemfillWordOfTypeFnDef(UInt32, uint32_t)
/* CGWMemfillUInt64 */
CGWMemfillWordOfTypeFnDef(UInt64, uint64_t)

#undef CGWMemfillWordOfTypeFnDef

void
CGWMemfill(
    const void      *pattern,
    size_t          pattern_len,
    unsigned int    count,
    void            *dst
)
{
    
    if ( pattern_len == 1 ) {
        memset(dst, (int)*((uint8_t*)pattern), count);
    } else {
        size_t      total_len = pattern_len * count;
        
        if ( total_len <= CGWMEMFILL_INITIAL_THRESHOLD_BYTES ) {
            switch ( pattern_len ) {
                case 2: {
                    uint16_t    s = *((uint16_t*)pattern),
                                *d = (uint16_t*)dst;
                    while ( count-- ) *d++ = s;
                    break;
                }
                case 4: {
                    uint32_t    s = *((uint32_t*)pattern),
                                *d = (uint32_t*)dst;
                    while ( count-- ) *d++ = s;
                    break;
                }
                case 8: {
                    uint64_t    s = *((uint64_t*)pattern),
                                *d = (uint64_t*)dst;
                    while ( count-- ) *d++ = s;
                    break;
                }
                default: {
                    while ( count-- ) {
                        memcpy(dst, pattern, pattern_len);
                        dst += pattern_len;
                    }
                    break;
                }
            }
        } else {
            /* Combination manual set of a fraction of buffer with
             * memcpy() to the rest.  */
            switch ( pattern_len ) {
                case 2: {
                    CGWMemfillUInt16(*((uint16_t*)pattern), count, (uint16_t*)dst);
                    break;
                }
                case 4: {
                    CGWMemfillUInt32(*((uint32_t*)pattern), count, (uint32_t*)dst);
                    break;
                }
                case 8: {
                    CGWMemfillUInt64(*((uint64_t*)pattern), count, (uint64_t*)dst);
                    break;
                }
                default: {
                    while ( count-- ) {
                        memcpy(dst, pattern, pattern_len);
                        dst += pattern_len;
                    }
                }
            }
        }
    }
}
