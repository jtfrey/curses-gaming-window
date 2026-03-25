/**
 * @file CGWPaletteTable.h
 * @brief Curses Gaming Window, Palette tables
 * 
 * Types and public functions that deal with color palette tables.
 *
 */

#ifndef __CGWPALETTETABLE_H__
#define __CGWPALETTETABLE_H__

#include "CGWPalette.h"
#include "CGWDecoder.h"

/**
 * Curses color pair that is black text on white background
 * Defaults to the default color pair, -1.  When \ref
 * CGWPaletteTableSetCursesColorPairs() is called this will
 * be modified (best not to cache its value).
 */
extern short int CGWCursesColorPairBW;

/**
 * Curses color pair that is white text on black background
 * Defaults to the default color pair, -1.  When \ref
 * CGWPaletteTableSetCursesColorPairs() is called this will
 * be modified (best not to cache its value).
 */
extern short int CGWCursesColorPairWB;

/**
 * The number of palettes per table
 * The chosen \ref CGW_BITS_PER_PALETTE dictates how many color
 * palettes a tile can reference.
 */
#define CGW_PALETTES_PER_TABLE          (1 << (CGW_BITS_PER_PALETTE))

/**
 * Fundamental bitmask for extracting palette value
 * The chosen \ref CGW_BITS_PER_PALETTE determines the bitmask that
 * can be bitwise-ANDed with a tile attribute to extract the
 * palette index.
 */
#define CGW_PALETTE_MASK                (CGW_PALETTES_PER_TABLE - 1)

/**
 * A table of palettes
 * The chosen \ref CGW_BITS_PER_PALETTE decides the \ref
 * CGW_PALETTES_PER_TABLE, which provides the number of array
 * elements in this data structure.  E.g. for a \ref
 * CGW_BITS_PER_PALETTE of 3, there will be \ref CGW_PALETTES_PER_TABLE
 * of (1 << 3) = 8 palettes.
 */
typedef struct __attribute__((packed)) {
    CGWPalette  palettes[CGW_PALETTES_PER_TABLE];   /*!< the array of color palettes */
} CGWPaletteTable;
/**
 * Pointer to a palette table
 * Type of a pointer to a CGWPaletteTable.  This type is of a size determined at
 * compile time and is relatively small, so no dynamic allocation is
 * employed -- thus, we don't use "references" to them, just straight-up
 * pointers.
 */
typedef CGWPaletteTable * CGWPaletteTablePtr;

/**
 * Initialize a CGWPaletteTable from an decoder
 * A CGWPaletteTable is read from the \p decoder.
 *
 * A signature word ('PLTT') precedes the data.  The rest
 * of the stream looks like:
 *
 *     uint32_t         n_palettes
 *     'PLT_' + data    palette 0
 *       :
 *     'PLT_' + data    palette <n_palettes>-1
 *
 * If n_palettes does not match the configured
 * \ref CGW_PALETTES_PER_TABLE, the table will fail to load
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param out_table     pointer to a CGWPaletteTable to
 *                      accept the palette table data
 * @return              Boolean true if successful
 */
bool CGWPaletteTableSetWithDecoder(CGWDecoderRef decoder, CGWPaletteTablePtr out_table);

/**
 * Set curses colors and color pairs
 * Initializes the curses color table with the RGB colors present
 * in \p palette_table.  Color indexes [0, <n> - 1] and
 * pair indexes [1, <n>] are used, where <n> is the product of
 * \ref CGW_PALETTES_PER_TABLE and \ref CGW_COLORS_PER_PALETTE.
 *
 * Errors may be encountered when \ref init_color() or
 * \ref init_pair() are called.  Any error will halt the process
 * and yield a return value of false.
 * @param palette_table     pointer to the palette table to load into curses
 * @return                  Boolean true on success
 */
bool CGWPaletteTableSetCursesColorPairs(CGWPaletteTablePtr palette_table);

/**
 * Map palette index to base curses pair index
 * Each palette consists of 4 colors, and pairs start at
 * 1:  so the first pair index for palette
 * \p palette_index occurs at (1 + 4 * \p palette_index).
 * @param palette_index     palette index in table
 * @return                  the corresponding pair index
 */
static inline
int
CGWPaletteTableMapToPairBase(
    unsigned int        palette_index
)
{
    return 1 + (palette_index << CGW_BITS_PER_PIXEL);
}

/**
 * Translate a palette table from one gamut to another.
 * Invokes \ref CGWPaletteTranslateGamuts() on all the palettes
 * in \p palette_table.  The \p from and \p to arguments
 * function as in \ref CGWPaletteTranslateGamuts().
 * @param palette_table the palatte table whose palettes
 *                      should be mapped
 * @param from          the color gamut containing
 *                      the colors in \p palette_table
 *                      when called
 * @param to            the color gamut to map to
 * @return              true if the colors in \p
 *                      palette_table were successfully
 *                      mapped, false otherwise
 */
bool CGWPaletteTableTranslateGamuts(CGWPaletteTablePtr palette_table, CGWColorGamutNESIds from, CGWColorGamutNESIds to);

#endif /* __CGWPALETTETABLE_H__ */
