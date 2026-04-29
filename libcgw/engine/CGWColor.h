/**
 * @file CGWColor.h
 * @brief Curses Gaming Window, Colors
 * 
 * Types and public functions that deal with color data.
 *
 */

#ifndef __CGWCOLOR_H__
#define __CGWCOLOR_H__

#include "CGWConfig.h"
#include "CGWDecoder.h"

/**
 * An RGB color
 * A color with 8-bit red, green, and blue channels.
 */
typedef struct __attribute__((packed)) {
    uint8_t     r;  /*!< red component, [0, 255] */
    uint8_t     g;  /*!< green component, [0, 255] */
    uint8_t     b;  /*!< blue component, [0, 255] */
} CGWColor;
/**
 * Pointer to a CGWColor
 * Type of a pointer to a CGWColor.  This type is of a size determined at
 * compile time and is relatively small, so no dynamic allocation is
 * employed -- thus, we don't use "references" to them, just straight-up
 * pointers.
 */
typedef CGWColor * CGWColorPtr;

/**
 * A set of in-gamut colors
 * The NES could represent 64 colors (56 unique) on-screen.
 * This type holds an array of the unique color values.
 */
typedef CGWColor CGWColorGamutNES[64];

/**
 * Built-in NES color gamut ids
 * Enumeration of the ids of the sets of representable colors
 * available to the NES hardware.
 */
typedef enum {
    kCGWColorGamutNES_2C02          = 0x0,
    kCGWColorGamutNES_2C03          = 0x1,
    kCGWColorGamutNES_2C04_0001     = 0x2,
    kCGWColorGamutNES_2C04_0002     = 0x3,
    kCGWColorGamutNES_2C04_0003     = 0x4,
    kCGWColorGamutNES_2C04_0004     = 0x5,
    kCGWColorGamutNES_2C05_01       = 0x6,
    kCGWColorGamutNES_2C05_02       = 0x7,
    kCGWColorGamutNES_2C05_03       = 0x8,
    kCGWColorGamutNES_2C05_04       = 0x9,
    kCGWColorGamutNES_2C05_05       = 0xA,
    kCGWColorGamutNES_Max
} CGWColorGamutNESIds;

/**
 * Black
 * The color black.
 */
extern const CGWColor CGWColorBlack;

/**
 * White
 * The color white.
 */
extern const CGWColor CGWColorWhite;

/**
 * Built-in NES color gamuts
 * The set of NES color gamuts.
 */
extern const CGWColorGamutNES CGWColorNESGamuts[kCGWColorGamutNES_Max];

/**
 * Create a CGWColor from component values
 * A convenience function that manufactures a CGWColor
 * structure from component color values.
 * @param r     Red value, [0, 255]
 * @param g     Green value, [0, 255]
 * @param b     Blue value, [0, 255]
 * @return      A CGWColor structure containing the values
 */
static inline
CGWColor
CGWColorMake(
    uint8_t     r,
    uint8_t     g,
    uint8_t     b
)
{
    CGWColor    new_color = { .r = r, .g = g, .b = b };
    return new_color;
}

/**
 * Create a CGWColor from an index within a pre-defined gamut
 * Returns the CGWColor associated with an index within an
 * included NES gamut.
 * @param color_index   the color index in [0, 63]
 * @param gamut         the NES gamut in question
 * @return              the corresponding CGWColor or
 *                      CGWColorBlack if the indices are
 *                      out of range
 */
static inline
CGWColor
CGWColorMakeWithIndexToBuiltInGamut(
    int                 color_index,
    CGWColorGamutNESIds gamut
)
{
    return ((color_index>=0 && color_index<64) && (gamut>=0 && gamut <kCGWColorGamutNES_Max)) ?
                CGWColorNESGamuts[gamut][color_index] :
                CGWColorBlack;
}

/**
 * Create a CGWColor from an index within a gamut
 * Returns the CGWColor associated with an index into an
 * externally-defined gamut.
 * @param color_index   the color index in [0, 63]
 * @param gamut         the NES gamut in question
 * @return              the corresponding CGWColor or
 *                      CGWColorBlack if the indices are
 *                      out of range
 */
static inline
CGWColor
CGWColorMakeWithIndexInGamut(
    int                 color_index,
    CGWColorGamutNES    gamut
)
{
    return ( color_index>=0 && color_index<64 ) ?
                gamut[color_index] :
                CGWColorBlack;
}


/**
 * Initialize a CGWColor from an decoder
 * A CGWColor is read from the \p decoder.
 *
 * The color data are encoded as a sequence of four 8-bit
 * intensities in the range [0,255]:
 *
 *     [..]         red
 *     [..]         green
 *     [..]         blue
 *     [..]         unused padding
 *
 * @param decoder   the decoder from which the data will be
 *                  read
 * @param out_color pointer to a CGWColor to accept the color
 *                  data
 * @return          Boolean true if successful
 */
bool CGWColorSetWithDecoder(CGWDecoderRef decoder, CGWColorPtr out_color);

/**
 * Replace a color in one gamut with its counterpart in another
 * Search the NES gamut \p from for a color matching that of
 * \p a_color.  If found, lookup the color at the same index in
 * gamut \p to and fill-in \p a_color with that color value.
 * @param a_color       pointer to a color structure
 * @param from          the NES gamut in which a_color should be
 *                      found
 * @param to            the NES gamut used to replace the color
 *                      value
 * @return              boolean true if the color is found and
 *                      replaced, false otherwise
 */
bool CGWColorTranslateGamuts(CGWColorPtr a_color, CGWColorGamutNESIds from, CGWColorGamutNESIds to);

#endif /* __CGWCOLOR_H__ */
