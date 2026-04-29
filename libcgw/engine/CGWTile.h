/**
 * @file CGWTile.h
 * @brief Curses Gaming Window, Tiles
 * 
 * Types and public functions that deal with tile data.
 *
 */
 
#ifndef __CGWTILE_H__
#define __CGWTILE_H__

#include "CGWPixel.h"
#include "CGWPalette.h"
#include "CGWDecoder.h"

/**
 * Number of packed pixel words required for tile
 * Given the \ref CGW_BASE_TILE_WIDTH and \ref CGW_BASE_TILE_HEIGHT, this macro
 * holds the number of \ref CGWPackedPixels words required to hold all the
 * pixels.
 */
#define CGW_BASE_TILE_PACKED_PIXEL_COUNT    ((CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT) / CGW_PIXELS_PER_ATOM)

/**
 * Flattened pixel coordinate
 * Pixels are a flat one-dimensional array; a two-dimensional coordinate
 * is translated to a one-dimensional offset using this macro.
 *
 * The top-left pixel is coordinate (0, 0).
 * @param X         the x-coordinate
 * @param Y         the y-coordinate
 * @return          the one-dimensional offset of the pixel
 */
#define CGW_BASE_TILE_PIXEL_OFFSET(X, Y)    ((Y) * CGW_BASE_TILE_WIDTH + (X))

/**
 * Pixel index in packed pixel array
 * Given a pixel coordinate (\pX, \pY), this macro returns the index into
 * the array of \ref CGWPackedPixels words that contains the pixel.
 *
 * The top-left pixel is coordinate (0, 0).
 * @param X         the x-coordinate
 * @param Y         the y-coordinate
 * @return          the packed pixel array index
 */
#define CGW_BASE_TILE_PIXEL_INDEX(X, Y)     (CGW_BASE_TILE_PIXEL_OFFSET(X, Y) / CGW_PIXELS_PER_ATOM)

/**
 * Pixel mask in packed pixel array
 * Given a pixel coordinate (\pX, \pY), this macro returns the bitmask associated
 * with the \ref CGWPackedPixels word that contains the pixel.  A bitwise-AND
 * of this mask with the \ref CGWPackedPixel word will isolate the pixel -- at whatever
 * bit position it occurs.  The actual pixel value is found by shifting the result
 * by some number of positions.
 *
 * The top-left pixel is coordinate (0, 0).
 * @param X         the x-coordinate
 * @param Y         the y-coordinate
 * @return          the pixel bitmask
 */
#define CGW_BASE_TILE_PIXEL_MASK(X, Y)      ((CGWPackedPixels)CGW_PIXEL_MASK << (CGW_BITS_PER_PIXEL * (CGW_BASE_TILE_PIXEL_OFFSET(X, Y) % CGW_PIXELS_PER_ATOM)))

/**
 * A graphical sub-image
 * A tile consists of an array of packed pixels, with its size determined by the
 * compiled-in \ref CGW_BITS_PER_PIXEL value.
 *
 * The top-left pixel occurs at the lowest-order bits of the zeroeth word, with the
 * next pixel in the first row at the next-higher bits, moving up through the
 * remaining words.
 */
typedef struct __attribute__((packed)) {
    CGWPackedPixels     pixels[CGW_BASE_TILE_PACKED_PIXEL_COUNT];   /*!< the array of packed pixel words */
} CGWTile;
/**
 * Pointer to a CGWTile
 * Type of a pointer to a CGWTile.  This type is of a size determined at
 * compile time and is relatively small, so no dynamic allocation is
 * employed -- thus, we don't use "references" to them, just straight-up
 * pointers.
 */
typedef CGWTile * CGWTilePtr;

/**
 * Initialize a CGWTile from a decoder
 * A CGWTile is read from the \p decoder.
 *
 * A signature word ('TILE') precedes tile data.  What
 * follows is a header:
 *
 *     uint16_t         sub_type
 *     uint16_t         bits_per_pixel
 *     uint32_t         width
 *     uint32_t         height
 *
 * Valid sub-types are:
 *
 *         0            native binary format (pixels packed into
 *                      \ref CGWPackedPixels words)
 *         1            unpacked format (each pixel is a separate
 *                      integer value)
 *
 * For sub-type 0, the header is followed by
 * \ref CGW_BASE_TILE_PACKED_PIXEL_COUNT words.  For sub-type 1,
 * the header is followed by (width * height) integers sized
 * according to the bits_per_pixel value (e.g. bpp <= 8, uint8_t
 * is used).
 *
 * If the width, height, and bpp do not match the configured
 * \ref CGW_BASE_TILE_WIDTH, \ref CGW_BASE_TILE_HEIGHT, and
 * \ref CGW_BITS_PER_PIXEL, the tile will fail to load.
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param out_tile      pointer to a CGWTile to
 *                      accept the pixel data
 * @return              Boolean true if successful
 */
bool CGWTileSetWithDecoder(CGWDecoderRef decoder, CGWTilePtr out_tile);

/**
 * Retrieve the packed pixel word containing a coordinate
 * Given a pixel coordinate (\pX, \pY), this macro returns the \ref CGWPackedPixels
 * word that contains the pixel at that coordinate.
 *
 * The top-left pixel is coordinate (0, 0).
 * @param T         the \ref CGWTile
 * @param X         the x-coordinate
 * @param Y         the y-coordinate
 * @return          the packed pixel word containing the coordinate
 */
#define CGW_BASE_TILE_PIXEL_COARSE(T, X, Y) ((T).pixels[CGW_BASE_TILE_PIXEL_INDEX(X, Y)])

/**
 * Read the pixel value at a coordinate
 * Given a pixel coordinate (\pX, \pY), this macro returns the \ref CGWPixel
 * value at that coordinate.
 *
 * The top-left pixel is coordinate (0, 0).  No range checks are effected on
 * the coordinate.
 * @param T         the \ref CGWTile
 * @param X         the x-coordinate
 * @param Y         the y-coordinate
 * @return          the pixel value at the coordinate
 */
#define CGW_BASE_TILE_PIXEL(T, X, Y)        (CGWPixel)((CGW_BASE_TILE_PIXEL_COARSE(T, X, Y) >> ((CGW_BASE_TILE_PIXEL_OFFSET(X, Y) % CGW_PIXELS_PER_ATOM) * CGW_BITS_PER_PIXEL)) & CGW_PIXEL_MASK)

/**
 * Open a tile pixel iterator block
 * A tile pixel iterator block is a loop structure that begins
 * at the top-left pixel of the tile and loops over a row of
 * pixels then across the next row, etc.
 *
 * The \p TILE argument is the CGWTile structure that will
 * be iterated (not a pointer to a structure).
 *
 * The loop is setup to reference the pixel coordinate as
 * the variables \p p_x and \p p_y and the pixel value as
 * \p p_pixel, e.g.
 *
 *      CGWTile     a_tile;
 *
 *      CGWTilePixelIteratorOpen(a_tile);
 *          printf("(%2d, %2d) = 0x%X\n", p_x, p_y, p_pixel);
 *      CGWTilePixelIteratorClose();
 *
 * @param TILE          CGWTile data structure to iterate
 */
#define CGWTilePixelIteratorBlockOpen(TILE)                     \
    {                                                           \
        CGWPackedPixels     __pixels;                           \
        CGWPixel            p_pixel;                            \
        unsigned int        p_x, p_y, __p_i = 0, __p_c = 0;     \
        for ( p_y = 0; p_y < CGW_BASE_TILE_HEIGHT; p_y++ ) {    \
            for ( p_x = 0; p_x < CGW_BASE_TILE_WIDTH; p_x++ ) { \
                if ( __p_c == 0 ) {                             \
                    __p_c = CGW_PIXELS_PER_ATOM;                \
                    __pixels = (TILE).pixels[__p_i++];          \
                }                                               \
                p_pixel = (CGWPixel)(__pixels & CGW_PIXEL_MASK)

/**
 * Close a tile pixel iterator block
 * Terminate the loop structure of a tile pixel iterator block.
 */
#define CGWTilePixelIteratorBlockClose()                        \
                __pixels >>= CGW_BITS_PER_PIXEL, __p_c--;       \
            }                                                   \
        }                                                       \
    }

/**
 * A tile pixel iterator
 * Data structure that encapsulates the same state variables
 * used in an iterator block.  This iterator method affords the
 * code a bit more flexibility and lends itself to reuse in
 * other other APIs of this library.
 */
typedef struct {
    CGWPackedPixels     *pixels_list;       /*!< pointer to the array of packed pixels */
    CGWPackedPixels     pixels;             /*!< the current packed pixel record being processed */
    CGWPackedPixels     mask;               /*!< bit mask (for alternate order of iteration) */
    CGWPixel            pixel;              /*!< the last-iterated pixel value */
    unsigned short      x;                  /*!< current pixel x-coordinate */
    unsigned short      y;                  /*!< current pixel y-coordinate */
    unsigned short      c;                  /*!< number of pixels remaining in \p pixels */
    unsigned char       shift;              /*!< bit shift count */
    unsigned short      n_pixels;           /*!< total number of pixels remaining */
} CGWTilePixelIterator;

/**
 * Initialize a tile pixel iterator
 * Given the CGWTile \p T, initialize the CGWTilePixelIterator
 * data structure \p TPI to iterate over the pixels in that
 * tile.  The iterator runs from the top-left pixel to the
 * bottom-right.
 * @param T         a CGWTile
 * @param TPI       a CGWTilePixelIterator
 * @return          the total number of pixels to be iterated
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWTilePixelIteratorInit(T, TPI) \
    ((TPI).pixels_list = &(T).pixels[0], \
    (TPI).x = -1, (TPI).y = (TPI).c = 0, \
    (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#else
#   define CGWTilePixelIteratorInit(T, TPI) \
    ((TPI).pixels_list = &(T).pixels[0], (TPI).c = 0, \
    (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#endif

/**
 * Advance to the next pixel in the tile iterator
 * Given a CGWTilePixelIterator initialized with \ref
 * CGWTilePixelIteratorInit(), advance to the next
 * pixel value, setting the \p pixel field of \p TPI to its
 * value.  The coordinate of the pixel will be present in
 * the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels
 * remaining.
 * @param TPI       a CGWTilePixelIterator
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWTilePixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( ++(TPI).x == CGW_BASE_TILE_WIDTH ) { \
            ++(TPI).y, (TPI).x = 0; \
        } \
        if ( (TPI).c == 0 ) { \
            (TPI).c = CGW_PIXELS_PER_ATOM; \
            (TPI).pixels = *(TPI).pixels_list; \
            (TPI).pixels_list++; \
        } \
        (TPI).pixel = (CGWPixel)((TPI).pixels & CGW_PIXEL_MASK); \
        (TPI).pixels >>= CGW_BITS_PER_PIXEL, (TPI).c--, (TPI).n_pixels--; \
    }
#else
#   define CGWTilePixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( (TPI).c == 0 ) { \
            (TPI).c = CGW_PIXELS_PER_ATOM; \
            (TPI).pixels = *(TPI).pixels_list; \
            (TPI).pixels_list++; \
        } \
        (TPI).pixel = (CGWPixel)((TPI).pixels & CGW_PIXEL_MASK); \
        (TPI).pixels >>= CGW_BITS_PER_PIXEL, (TPI).c--, (TPI).n_pixels--; \
    }
#endif

/*
 *   [0]   00 02 04 06 08 0A 0C 0E
 *   [0]   10 12 14 16 18 1A 1C 1E
 *   [0]   20 22 24 26 28 2A 2C 2E
 *   [0]   30 32 34 36 38 3A 3C 3E
 *   [1]   00 02 04 06 08 0A 0C 0E
 *   [1]   10 12 14 16 18 1A 1C 1E
 *   [1]   20 22 24 26 28 2A 2C 2E
 *   [1]   30 32 34 36 38 3A 3C 3E
 *
 * - Reverse iterator - h-flip + v-flip = 180° rotation
 *     - x = CGW_BASE_TILE_WIDTH - 1
 *     - y = CGW_BASE_TILE_HEIGHT - 1
 *     - coarse index = CGW_PIXELS_PER_ATOM
 *     - while coarse index-- > 0:
 *         - shift, bitmask = 64 - CGW_BITS_PER_PIXEL = 62, CGW_PIXEL_MASK << shift
 *         - while bitmask != 0:
 *             - pixel = (pixels & bitmask) >> shift
 *             - bitmask >>= CGW_BITS_PER_PIXEL
 *             - shift -= CGW_BITS_PER_PIXEL
 *         - coarse index--
 */

/**
 * Initialize a reverse-order tile pixel iterator
 * Given the CGWTile \p T, initialize the CGWTilePixelIterator
 * data structure \p TPI to iterate over the pixels in that
 * tile.  The iterator runs from the bottom-right pixel to the
 * top-left.
 * @param T         a CGWTile
 * @param TPI       a CGWTilePixelIterator
 * @return          the total number of pixels to be iterated
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWTileReversePixelIteratorInit(T, TPI) \
    ((TPI).pixels_list = &(T).pixels[CGW_BASE_TILE_PACKED_PIXEL_COUNT - 1], \
    (TPI).x = CGW_BASE_TILE_WIDTH, (TPI).y = CGW_BASE_TILE_HEIGHT - 1, \
    (TPI).c = 0, (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#else
#   define CGWTileReversePixelIteratorInit(T, TPI) \
    ((TPI).pixels_list = &(T).pixels[CGW_BASE_TILE_PACKED_PIXEL_COUNT - 1], \
    (TPI).c = 0, (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#endif

/**
 * Advance to the next pixel in the reverse-order tile iterator
 * Given a CGWTilePixelIterator initialized with \ref
 * CGWTileReversePixelIteratorInit(), advance to the next
 * pixel value, setting the \p pixel field of \p TPI to its
 * value.  The coordinate of the pixel will be present in
 * the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels remaining.
 * @param TPI       a CGWTilePixelIterator
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWTileReversePixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( (TPI).x == 0 ) { \
            --(TPI).y, (TPI).x = CGW_BASE_TILE_WIDTH - 1; \
        } else { \
          --(TPI).x; \
        } \
        if ( (TPI).c == 0 ) { \
            (TPI).c = CGW_PIXELS_PER_ATOM; \
            (TPI).pixels = *(TPI).pixels_list; \
            (TPI).pixels_list--; \
            (TPI).mask = (CGWPackedPixels)CGW_PIXEL_MASK << ((TPI).shift = CGW_PACKED_PIXELS_BITS - CGW_BITS_PER_PIXEL); \
        } \
        (TPI).pixel = (CGWPixel)(((TPI).pixels & (TPI).mask) >> (TPI).shift); \
        (TPI).mask >>= CGW_BITS_PER_PIXEL, (TPI).shift -= CGW_BITS_PER_PIXEL, (TPI).c--, (TPI).n_pixels--; \
    }
#else
#   define CGWTileReversePixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( (TPI).c == 0 ) { \
            (TPI).c = CGW_PIXELS_PER_ATOM; \
            (TPI).pixels = *(TPI).pixels_list; \
            (TPI).pixels_list--; \
            (TPI).mask = (CGWPackedPixels)CGW_PIXEL_MASK << ((TPI).shift = CGW_PACKED_PIXELS_BITS - CGW_BITS_PER_PIXEL); \
        } \
        (TPI).pixel = (CGWPixel)(((TPI).pixels & (TPI).mask) >> (TPI).shift); \
        (TPI).mask >>= CGW_BITS_PER_PIXEL, (TPI).shift -= CGW_BITS_PER_PIXEL, (TPI).c--, (TPI).n_pixels--; \
    }
#endif

/*
 *   [0]   00 02 02 03 03 02 01 00
 *   [0]   00 00 02 03 02 01 00 00
 *   [0]   00 00 00 02 02 01 00 00
 *   [0]   00 00 00 00 02 01 00 00
 *   [1]   00 00 00 02 02 01 00 00
 *   [1]   00 00 02 03 03 02 01 00
 *   [1]   00 02 03 03 03 02 01 00
 *   [1]   02 03 03 03 03 02 01 00
 *
 * # Vertical-flip iterator
 *
 * Start in the highest packed-pixel unit with a mask at the lower-left position
 * (e.g. 0x0003000000000000) and a copy of that original mask saved.  Iterate
 * CGW_BASE_TILE_WIDTH times, shifting the mask UP each time
 * (e.g. 0x000C000000000000, 0x003000000000000, etc.).  Next, shift the saved
 * mask DOWN by (CGW_BASE_TILE_WIDTH * CGW_BITS_PER_PIXEL) bits; if not zero
 * repeat the upwards-shift iteration pattern (e.g. 0x0000000300000000 ->
 * 0x0000000C00000000, 0x000000300000000, etc.).
 *
 * Once the shifted saved mask has gone to zero, move to the next lower packed-pixel
 * unit and repeat.  If the last packed-pixel unit has been used, the iteration is
 * complete.
 *
 * ## Init
 *
 * If the CGW_PIXELS_PER_ATOM is a multiple of the CGW_BASE_TILE_WIDTH,
 * the following will work:
 *
 *     - x = CGW_BASE_TILE_WIDTH - 1, y = CGW_BASE_TILE_HEIGHT
 *     - pixels_base = &pixels[CGW_BASE_TILE_PACKED_PIXEL_COUNT]
 *     - pixel_shift = 0
 *     - n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT
 *     - n_pixels_in_atom = 0
 *
 * Otherwise, a naive iteration by pixel coordinate (with recalculation of
 * packed-pixel index, bitmask, and bit shift on each iteration) is used:
 *
 *     - x = CGW_BASE_TILE_WIDTH - 1, y = CGW_BASE_TILE_HEIGHT
 *     - pixels_base = &pixels[0]
 *     - n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT
 *
 * ## Iterate
 *
 * If the CGW_PIXELS_PER_ATOM is a multiple of the CGW_BASE_TILE_WIDTH,
 * the following will work:
 *
 *     - if x == CGW_BASE_TILE_WIDTH - 1:
 *         - --y
 *         - x = 0
 *         - pixel_shift = CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET(0 + y * CGW_BASE_TILE_WIDTH)
 *         - pixel_mask = CGW_PACKED_PIXEL_MASK_FOR_OFFSET(0 + y * CGW_BASE_TILE_WIDTH)
 *     - else:
 *         - x++
 *         - pixel_mask <<= CGW_BITS_PER_PIXEL
 *         - pixel_shift += CGW_BITS_PER_PIXEL
 *     - if n_pixels_in_atom == 0:
 *         - n_pixels_in_atom = CGW_PIXELS_PER_ATOM
 *         - pixel = *(--pixels_base)
 *     - out_pixel = (pixel & pixel_mask) >> pixel_shift
 *     - n_pixels_in_atom--, n_pixels--
 *
 * Otherwise, a naive iteration by pixel coordinate (with recalculation of
 * packed-pixel index, bitmask, and bit shift on each iteration) is used:
 *
 *     - if x == CGW_BASE_TILE_WIDTH - 1:
 *         - --y
 *         - x = 0
 *     - else:
 *         - x++
 *     - out_pixel = (pixels_list[CGW_PACKED_PIXEL_INDEX_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)] 
 *                      & CGW_PACKED_PIXEL_MASK_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)) >> 
 *                        CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)
 *     - n_pixels--
 */
#if (CGW_PIXELS_PER_ATOM > CGW_BASE_TILE_WIDTH) && ((CGW_PIXELS_PER_ATOM % CGW_BASE_TILE_WIDTH) == 0)
#   define CGWTileVFlipPixelIteratorTypeStr "CGWTileVFlipPixelIteratorOptimized"
/**
 * Initialize a vertical-flip tile pixel iterator
 * Given the CGWTile \p T, initialize the CGWTilePixelIterator
 * data structure \p TPI to iterate over the pixels in that
 * tile.  The iterator runs from the bottom-left pixel to the
 * top-right.
 * @param T         a CGWTile
 * @param TPI       a CGWTilePixelIterator
 * @return          the total number of pixels to be iterated
 */
#   define CGWTileVFlipPixelIteratorInit(T, TPI) \
    ((TPI).x = CGW_BASE_TILE_WIDTH - 1, (TPI).y = CGW_BASE_TILE_HEIGHT, \
     (TPI).pixels_list = &(T).pixels[CGW_BASE_TILE_PACKED_PIXEL_COUNT], \
     (TPI).c = 0, (TPI).shift = 0, \
     (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
/**
 * Advance to the next pixel in the vertical-flip tile iterator
 * Given a CGWTilePixelIterator initialized with \ref
 * CGWTileVFlipPixelIteratorInit(), advance to the next
 * pixel value, setting the \p pixel field of \p TPI to its
 * value.  The coordinate of the pixel will be present in
 * the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels remaining.
 * @param TPI       a CGWTilePixelIterator
 */
#   define CGWTileVFlipPixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( (TPI).x == CGW_BASE_TILE_WIDTH - 1 ) { \
            --(TPI).y; \
            (TPI).x = 0, (TPI).shift = CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET((TPI).y * CGW_BASE_TILE_WIDTH); \
            (TPI).mask = CGW_PACKED_PIXEL_MASK_FOR_OFFSET((TPI).y * CGW_BASE_TILE_WIDTH); \
        } else { \
            (TPI).x++, (TPI).mask <<= CGW_BITS_PER_PIXEL, (TPI).shift += CGW_BITS_PER_PIXEL; \
        } \
        if ( (TPI).c == 0 ) { \
            (TPI).c = CGW_PIXELS_PER_ATOM, (TPI).pixels = *(--(TPI).pixels_list); \
        } \
        (TPI).pixel = ((TPI).pixels & (TPI).mask) >> (TPI).shift, (TPI).c--, (TPI).n_pixels--; \
    }
#else
/*
 * This is an alternative that resorts to iterating over pixel
 * coordinates rather and recomputing the packed-pixel index,
 * mask, and shift each time.  The implementation above is optimized
 * for when the packed-pixel type is a multiple of the bit width
 * of a row of pixels.
 */
#   define CGWTileVFlipPixelIteratorTypeStr "CGWTileVFlipPixelIteratorBasic"
#   define CGWTileVFlipPixelIteratorInit(T, TPI) \
    ((TPI).x = CGW_BASE_TILE_WIDTH - 1, (TPI).y = CGW_BASE_TILE_HEIGHT, \
     (TPI).pixels_list = &(T).pixels[0], \
     (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#   define CGWTileVFlipPixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( (TPI).x == CGW_BASE_TILE_WIDTH - 1 ) { \
            --(TPI).y, (TPI).x = 0; \
        } else { \
            (TPI).x++; \
        } \
        (TPI).c = (TPI).x + (TPI).y * CGW_BASE_TILE_WIDTH; \
        (TPI).pixel = ((TPI).pixels_list[CGW_PACKED_PIXEL_INDEX_FOR_OFFSET((TPI).c)] & \
                            CGW_PACKED_PIXEL_MASK_FOR_OFFSET((TPI).c)) >> CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET((TPI).c); \
        (TPI).n_pixels--; \
    }
#endif

/*
 *   [0]   00 02 02 03 03 02 01 00
 *   [0]   00 00 02 03 02 01 00 00
 *   [0]   00 00 00 02 02 01 00 00
 *   [0]   00 00 00 00 02 01 00 00
 *   [1]   00 00 00 02 02 01 00 00
 *   [1]   00 00 02 03 03 02 01 00
 *   [1]   00 02 03 03 03 02 01 00
 *   [1]   02 03 03 03 03 02 01 00
 *
 * # Horizontal-flip iterator
 *
 * Start in the lower packed-pixel unit with a mask at the upper-right position
 * (e.g. 0x000000000000C000) and a copy of that original mask saved.  Iterate
 * CGW_BASE_TILE_WIDTH times, shifting the mask DOWN each time
 * (e.g. 0x0000000000003000, 0x000000000000C00, etc.).  Next, shift the saved
 * mask UP by (CGW_BASE_TILE_WIDTH * CGW_BITS_PER_PIXEL) bits; if not zero
 * repeat the upwards-shift iteration pattern (e.g. 0x000000000000C000 ->
 * 0x00000000C0000000, 0x000000030000000, etc.).
 *
 * Once the shifted saved mask has gone to zero, move to the next higher packed-pixel
 * unit and repeat.  If the last packed-pixel unit has been used, the iteration is
 * complete.
 *
 * ## Init
 *
 * If the CGW_PIXELS_PER_ATOM is a multiple of the CGW_BASE_TILE_WIDTH,
 * the following will work:
 *
 *     - x = 0, y = -1
 *     - pixels_base = &pixels[0]
 *     - pixel_shift = 0
 *     - n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT
 *     - n_pixels_in_atom = 0
 *
 * Otherwise, a naive iteration by pixel coordinate (with recalculation of
 * packed-pixel index, bitmask, and bit shift on each iteration) is used:
 *
 *     - x = 0, y = -1
 *     - pixels_base = &pixels[0]
 *     - n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT
 *
 * ## Iterate
 *
 * If the CGW_PIXELS_PER_ATOM is a multiple of the CGW_BASE_TILE_WIDTH,
 * the following will work:
 *
 *     - if x == 0:
 *         - ++y
 *         - x = CGW_BASE_TILE_WIDTH - 1
 *         - pixel_shift = CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)
 *         - pixel_mask = CGW_PACKED_PIXEL_MASK_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)
 *     - else:
 *         - x--
 *         - pixel_mask >>= CGW_BITS_PER_PIXEL
 *         - pixel_shift -= CGW_BITS_PER_PIXEL
 *     - if n_pixels_in_atom == 0:
 *         - n_pixels_in_atom = CGW_PIXELS_PER_ATOM
 *         - pixel = *pixels_base++
 *     - out_pixel = (pixel & pixel_mask) >> pixel_shift
 *     - n_pixels_in_atom--, n_pixels--
 *
 * Otherwise, a naive iteration by pixel coordinate (with recalculation of
 * packed-pixel index, bitmask, and bit shift on each iteration) is used:
 *
 *     - if x == 0:
 *         - --y
 *         - x = CGW_BASE_TILE_WIDTH - 1
 *     - else:
 *         - x--
 *     - out_pixel = (pixels_list[CGW_PACKED_PIXEL_INDEX_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)] 
 *                      & CGW_PACKED_PIXEL_MASK_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)) >> 
 *                        CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET(x + y * CGW_BASE_TILE_WIDTH)
 *     - n_pixels--
 */
#if (CGW_PIXELS_PER_ATOM > CGW_BASE_TILE_WIDTH) && ((CGW_PIXELS_PER_ATOM % CGW_BASE_TILE_WIDTH) == 0)
#   define CGWTileHFlipPixelIteratorTypeStr "CGWTileHFlipPixelIteratorOptimized"
/**
 * Initialize a horizontal-flip tile pixel iterator
 * Given the CGWTile \p T, initialize the CGWTilePixelIterator
 * data structure \p TPI to iterate over the pixels in that
 * tile.  The iterator runs from the bottom-left pixel to the
 * top-right.
 * @param T         a CGWTile
 * @param TPI       a CGWTilePixelIterator
 * @return          the total number of pixels to be iterated
 */
#   define CGWTileHFlipPixelIteratorInit(T, TPI) \
    ((TPI).x = 0, (TPI).y = -1, \
     (TPI).pixels_list = &(T).pixels[0], \
     (TPI).c = 0, (TPI).shift = 0, \
     (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
/**
 * Advance to the next pixel in the horizontal-flip tile iterator
 * Given a CGWTilePixelIterator initialized with \ref
 * CGWTileHFlipPixelIteratorInit(), advance to the next
 * pixel value, setting the \p pixel field of \p TPI to its
 * value.  The coordinate of the pixel will be present in
 * the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels remaining.
 * @param TPI       a CGWTilePixelIterator
 */
#   define CGWTileHFlipPixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( (TPI).x == 0 ) { \
            ++(TPI).y, (TPI).x = CGW_BASE_TILE_WIDTH - 1; \
            (TPI).shift = CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET((TPI).x + (TPI).y * CGW_BASE_TILE_WIDTH); \
            (TPI).mask = CGW_PACKED_PIXEL_MASK_FOR_OFFSET((TPI).x + (TPI).y * CGW_BASE_TILE_WIDTH); \
        } else { \
            (TPI).x--, (TPI).mask >>= CGW_BITS_PER_PIXEL, (TPI).shift -= CGW_BITS_PER_PIXEL; \
        } \
        if ( (TPI).c == 0 ) { \
            (TPI).c = CGW_PIXELS_PER_ATOM, (TPI).pixels = *(TPI).pixels_list++; \
        } \
        (TPI).pixel = ((TPI).pixels & (TPI).mask) >> (TPI).shift, (TPI).c--, (TPI).n_pixels--; \
    }
#else
/*
 * This is an alternative that resorts to iterating over pixel
 * coordinates rather and recomputing the packed-pixel index,
 * mask, and shift each time.  The implementation above is optimized
 * for when the packed-pixel type is a multiple of the bit width
 * of a row of pixels.
 */
#   define CGWTileHFlipPixelIteratorTypeStr "CGWTileHFlipPixelIteratorBasic"
#   define CGWTileHFlipPixelIteratorInit(T, TPI) \
    ((TPI).x = 0, (TPI).y = -1, \
     (TPI).pixels_list = &(T).pixels[0], \
     (TPI).n_pixels = CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#   define CGWTileHFlipPixelIteratorNext(TPI) \
    if ( (TPI).n_pixels ) { \
        if ( (TPI).x == 0 ) { \
            ++(TPI).y, (TPI).x = CGW_BASE_TILE_WIDTH - 1; \
        } else { \
            (TPI).x--; \
        } \
        (TPI).c = (TPI).x + (TPI).y * CGW_BASE_TILE_WIDTH; \
        (TPI).pixel = ((TPI).pixels_list[CGW_PACKED_PIXEL_INDEX_FOR_OFFSET((TPI).c)] & \
                            CGW_PACKED_PIXEL_MASK_FOR_OFFSET((TPI).c)) >> CGW_PACKED_PIXEL_SHIFT_FOR_OFFSET((TPI).c); \
        (TPI).n_pixels--; \
    }
#endif
 
 

#endif /* __CGWTILE_H__ */
