/**
 * @file CGWPalette.h
 * @brief Curses Gaming Window, Palettes
 * 
 * Types and public functions that deal with color palettes.
 *
 */

#ifndef __CGWPALETTE_H__
#define __CGWPALETTE_H__

#include "CGWColor.h"
#include "CGWDecoder.h"

/**
 * The number of colors per palette
 * The chosen \ref CGW_BITS_PER_PIXEL dictates how many color
 * indices an image can reference.  Thus, a palette used to draw
 * an image will have \ref CGW_COLORS_PER_PALETTE colors defined
 * in it.
 */
#define CGW_COLORS_PER_PALETTE          (1 << (CGW_BITS_PER_PIXEL))

/**
 * A palette of colors
 * The chosen \ref CGW_BITS_PER_PIXEL decides the \ref
 * CGW_COLORS_PER_PALETTE, which provides the number of array
 * elements in this data structure.  E.g. for a \ref
 * CGW_BITS_PER_PIXEL of 2, there will be \ref CGW_COLORS_PER_PALETTE
 * of (1 << 2) = 4 colors.
 */
typedef struct __attribute__((packed)) {
    CGWColor    colors[CGW_COLORS_PER_PALETTE]; /*!< array of RGB colors in the palette */
} CGWPalette;
/**
 * Pointer to a palette
 * Type of a pointer to a CGWPalette.  This type is of a size determined at
 * compile time and is relatively small, so no dynamic allocation is
 * employed -- thus, we don't use "references" to them, just straight-up
 * pointers.
 */
typedef CGWPalette * CGWPalettePtr;

/**
 * Initialize a CGWPalette from an decoder
 * A CGWPalette object is read from the decoder.
 *
 * A signature word 'PLT_' precedes the data, followed
 * by a sub-type identifier:
 *
 *     uint8_t          sub-type
 *
 * where sub-types are
 *
 *     0                R, G, B components
 *     1                gamut + index
 *
 * For an RGB sub-type, 
 *
 * whereas for a gamut + index sub-type:
 *
 *     uint8_t          gamut
 *     uint8_t          index[
 *
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param out_palette   pointer to a CGWPalette to accept
 *                      the palette data
 * @return              Boolean true if successful
 */
bool CGWPaletteSetWithDecoder(CGWDecoderRef decoder, CGWPalettePtr out_palette);

/**
 * Translate a palette from one gamut to another
 * Searches the NES color gamut \p from for colors in
 * \p the_palette.  The first matching color index is
 * used to map to the same index in color gamut \p to
 * and overwrites the data in \p the_palette.
 * @param the_palette   the palatte whose colors
 *                      should be mapped
 * @param from          the color gamut containing
 *                      the colors in \p the_palatte
 *                      when called
 * @param to            the color gamut to map to
 * @return              true if the colors in \p
 *                      the_palatte were successfully
 *                      mapped, false otherwise
 */
bool CGWPaletteTranslateGamuts(CGWPalettePtr the_palette, CGWColorGamutNESIds from, CGWColorGamutNESIds to);

#endif /* __CGWPALETTE_H__ */
