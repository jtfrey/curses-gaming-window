/**
 * @file CGWPaletteTableArray.h
 * @brief Curses Gaming Window, Palette table arrays
 * 
 * Types and public functions that deal with arrays of color
 * palette tables.
 *
 */

#ifndef __CGWPALETTETABLEARRAY_H__
#define __CGWPALETTETABLEARRAY_H__

#include "CGWPaletteTable.h"
#include "CGWDecoder.h"

/**
 * An array of palette tables
 * Palette tables may be reused across tile maps.  Rather than repeatedly storing
 * the same palette table, an array of palette tables can be leveraged.
 *
 * This is a variable-length data structure, meaning that the final field is an
 * array of indeterminate size.  The \ref n_tables field indicates how many
 * slots were alloted when the object was created.
 */
typedef struct __attribute__((packed)) {
    uint32_t            n_tables;       /*!< the number of tables in the array */
    CGWPaletteTable     tables[];       /*!< array of palette tables */
} CGWPaletteTableArray;
/**
 * Reference to an array of palette tables
 * Since an array of palette tables is always dynamically-allocated, a
 * reference (pointer) is used when working with it.
 */
typedef CGWPaletteTableArray * CGWPaletteTableArrayRef;

/**
 * Allocate a CGWPaletteTableArray of a given size
 * A new CGWPaletteTableArray is allocated with space for
 * \p n_tables palette tables.  All memory is initialized to
 * zero, effedtively making all colors black.
 */
CGWPaletteTableArrayRef CGWPaletteTableArrayCreate(uint32_t n_tables);

/**
 * Deallocate a CGWPaletteTableArray
 * Dispose of all memory occupied by \p table_array and
 * invalidate the object.
 * @param table_array       the array of palette tables to destroy
 */
void CGWPaletteTableArrayDestroy(CGWPaletteTableArrayRef table_array);

/**
 * Initialize a CGWPaletteTableArray from an decoder
 * A CGWPaletteTableArray is read from the \p decoder.
 *
 * A signature word ('PLTS') precedes the data.  The rest
 * of the stream looks like:
 *
 *     [........]       n_tables (32-bit word)
 *     'PLTT' + data    palette table 0
 *       :
 *     'PLTT' + data    palette table n-1
 *
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param out_tables    pointer to a CGWPaletteTableArrayRef to accept the
 *                      palette tables data
 * @return              Boolean true if successful
 */
bool CGWPaletteTableArrayCreateWithDecoder(CGWDecoderRef decoder, CGWPaletteTableArrayRef *out_array);

/**
 * Translate an array of palette tables from one gamut to another.
 * Invokes \ref CGWPaletteTableTranslateGamuts() on all the tables
 * in \p table_array.  The \p from and \p to arguments
 * function as in \ref CGWPaletteTranslateGamuts().
 * @param table_array       the object whose palette tables
 *                          should be mapped
 * @param from              the color gamut containing
 *                          the colors in \p table_array
 *                          when called
 * @param to                the color gamut to map to
 * @return                  true if the colors in \p
 *                          table_array were successfully
 *                          mapped, false otherwise
 */
bool CGWPaletteTableArrayTranslateGamuts(CGWPaletteTableArrayRef table_array, CGWColorGamutNESIds from, CGWColorGamutNESIds to);

#endif /* __CGWPALETTETABLEARRAY_H__ */
