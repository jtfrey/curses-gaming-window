/**
 * @file CGWPixel.h
 * @brief Curses Gaming Window, Pixels
 * 
 * Types and public functions that deal with pixel data.
 *
 */

#ifndef __CGWPIXEL_H__
#define __CGWPIXEL_H__

#include "CGWConfig.h"

/**
 * Working type for pixel indices
 * Type used for holding pixel indices for working computation.
 */
typedef int CGWPixel;

/**
 * Type for packed pixel indices
 * Type used for holding packed sequences of pixel indices.  This
 * should be an atomic integer type on which the CPU can do
 * arithmetic operations easily.
 */
typedef CGW_PACKED_PIXELS_TYPE CGWPackedPixels;

/**
 * Fundamental bitmask for extracting pixel value
 * The chosen \ref CGW_BITS_PER_PIXEL determines the bitmask that
 * can be bitwise-ANDed with a packed pixel sequence to extract a
 * single pixel value.
 */
#define CGW_PIXEL_MASK  (CGW_COLORS_PER_PALETTE - 1)

/**
 * Packed-pixel index for offset
 * Packed-pixel word associated with the given offset within
 * the flattened array of CGWPackedPixels values.
 * @param OFF       the pixel offset (x + y * height)
 * @return          bitmask fo the given pixel
 */
#define CGW_PACKED_PIXEL_INDEX_FOR_OFFSET(OFF) \
        ((OFF) / (CGW_PIXELS_PER_ATOM))

/**
 * Pixel extraction mask for offset
 * Pixel mask associated with the given offset within
 * the flattened array of CGWPackedPixels values.
 * @param OFF       the pixel offset (x + y * height)
 * @return          bitmask fo the given pixel
 */
#define CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET(OFF) \
        (((OFF) % CGW_PIXELS_PER_ATOM) * CGW_BITS_PER_PIXEL)

/**
 * Pixel extraction mask for offset
 * Pixel mask associated with the given offset within
 * the flattened array of CGWPackedPixels values.
 * @param OFF       the pixel offset (x + y * height)
 * @return          bitmask fo the given pixel
 */
#define CGW_PACKED_PIXEL_MASK_FOR_OFFSET(OFF) \
        ((CGWPackedPixels)CGW_PIXEL_MASK << CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET(OFF))

#endif /* __CGWPIXEL_H__ */
