/**
 * @file CGWMetaTileTable.h
 * @brief Curses Gaming Window, Table of tiles composed of tiles
 * 
 * Types and public functions that deal with an array of
 * tiles that are composed of base tiles.
 *
 */
 
#ifndef __CGWMETATILETABLE_H__
#define __CGWMETATILETABLE_H__

#include "CGWBitvector.h"
#include "CGWTileTable.h"
#include "CGWDecoder.h"

/**
 * Meta tile definition
 * A meta tile is a graphics tile that is comprised of a 2D array
 * of underlying base tiles.  The base tiles are referenced as indices
 * relative to a CGWTileTable.
 */
typedef struct __attribute__((packed)) {
    CGWTileTableIndex   tile_indices[CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT];   /*!< array of base tile indices */
} CGWMetaTile;

/**
 * CGWMetaTileTable options
 * Bit vector of functional options associated with a CGWMetaTileTable.
 */
enum {
    kCGWMetaTileTableOptionsShouldDestroyBaseTileTable = (1 << 0), /*!< destroy the CGWTileTable when the object is destroyed */
    kCGWMetaTileTableOptionsMask = kCGWMetaTileTableOptionsShouldDestroyBaseTileTable /*!< all options */
};

/**
 * A table of graphics meta-tiles
 * Meta-tiles are comprised of 2D arrays of base graphics tiles.  E.g.
 * in Super Mario Bros. the 8x8 base tiles are combined to make 16x16
 * environment elements (question blocks, etc.).
 */
typedef struct {
    unsigned int        n_tiles;        /*!< the number of meta tiles in the table */
    CGWBitvector        options;        /*!< functional options */
    CGWTileTableRef     base_tiles;     /*!< the table of base graphics tiles referenced by this meta-tile table */
    CGWMetaTile         meta_tiles[];   /*!< the meta-tile definitions */
} CGWMetaTileTable;

/**
 * Reference to a table of meta-tiles
 * Since an array of meta-tiles is always dynamically-allocated, a
 * reference (pointer) is used when working with it.
 */
typedef CGWMetaTileTable * CGWMetaTileTableRef;

/**
 * Create an empty meta tile table with n_tiles slots.
 * Dynamically-allocate a new CGWMetaTileTable with room for \p n_tiles
 * graphics tiles.
 * @param n_tiles       number of array slots
 * @param options       behavioral options
 * @param base_tiles    the underlying base tile table from which meta
 *                      tiles will be composed
 * @return              a reference to the newly-initialized tile table
 *                      or NULL on error
 */
CGWMetaTileTableRef CGWMetaTileTableCreate(unsigned int n_tiles, CGWBitvector options, CGWTileTableRef base_tiles);

/**
 * Destroy a tile table
 * Dispose of memory occupied by the \p tile_table.
 * @param tile_table    the tile table to dispose of
 */
void CGWMetaTileTableDestroy(CGWMetaTileTableRef tile_table);

/**
 * Create a CGWMetaTileTable using data from a decoder
 * A CGWMetaTileTable is read from the \p decoder.
 *
 * A signature word ('MTBL') precedes the meta tile data.
 * A header follows:
 *
 *     uint32_t         n_tiles
 *     <TTBL> …         the encoded base tile table
 *
 * A sequence of encoded CGWMetaTile structures follows.
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param tile_table    pointer to a CGWMetaTileTableRef to
 *                      be filled-in with a reference to the
 *                      allocated object
 * @return              Boolean true if successful
 */
bool CGWMetaTileTableCreateWithDecoder(CGWDecoderRef decoder, CGWMetaTileTableRef *tile_table);

/**
 * A meta tile iterator for use with CGWMetaTileTable objects
 * For iterating across columns, down/up rows, a tile table
 * iterator is needed to keep track of each base tile
 * in the current meta tile row.
 *
 * The total pixels loaded into all the iterators is
 * n_pixels_row, which is decremented on each iteration;
 * when n_pixels_row == 0, the next row of base tiles
 * (r++) is loaded.
 *
 * x,y are updated on each iteration to be the pixel
 * coordinate of the current pixel.  The n_pixels is
 * decremented on each iteration; when it reaches zero
 * the iteration is complete.
 */
typedef struct {
    CGWTileTableRef             base_tiles;
    CGWTileTableIndex           *tile_indices;
    CGWTileTablePixelIterator   iter_by_column[CGW_META_TILE_WIDTH];
    CGWPixel                    pixel;              /*!< the last-iterated pixel value */
    unsigned short              x;
    unsigned short              y;
    unsigned short              n_pixels_row;
    unsigned short              n_pixels;
} CGWMetaTileTablePixelIterator;

/**
 * Initialize a meta tile table tile pixel iterator
 * Initializes a meta tile iterator with data associated
 * with the meta tile at index \p MTI in table \p MTT.  The
 * iterator runs from top-left to bottom-right.
 * @param MTT       a CGWMetaTileTableRef
 * @param MTI       a meta tile index in the table \p MTT
 * @param MTPI      a CGWMetaTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWMetaTilePixelIteratorInit(MTT, MTI, MTPI) \
    ((MTPI).base_tiles = (MTT)->base_tiles, (MTPI).tile_indices = &(MTT)->meta_tiles[(MTI)].tile_indices[0], \
    (MTPI).x = -1, (MTPI).y = (MTPI).n_pixels_row = 0, \
    (MTPI).n_pixels = CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#else
#   define CGWMetaTilePixelIteratorInit(MTT, MTI, MTPI) \
    ((MTPI).base_tiles = (MTT)->base_tiles, (MTPI).tile_indices = &(MTT)->meta_tiles[(MTI)].tile_indices[0], \
    (MTPI).x = -1, (MTPI).n_pixels_row = 0, \
    (MTPI).n_pixels = CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#endif

/**
 * Advance to the next pixel in the meta tile iterator
 * Sets the \p pixel field of \p MTPI to the next pixel in the
 * meta tile being iterated.  The coordinate of the pixel will be
 * present in the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels remaining.
 * @param MTPI      a CGWMetaTileTablePixelIterator
 * @param MTPI      a CGWMetaTileTablePixelIterator
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWMetaTilePixelIteratorNext(MTPI) \
    if ( (MTPI).n_pixels ) { \
        unsigned int    pidx; \
        if ( ++(MTPI).x == CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH ) { \
            ++(MTPI).y, (MTPI).x = 0; \
        } \
        if ( (MTPI).n_pixels_row == 0 ) { \
            (MTPI).n_pixels_row = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT; \
            for ( pidx = 0; pidx < CGW_META_TILE_WIDTH; pidx++ ) {\
                CGWTileTablePixelIteratorInit((MTPI).base_tiles, *(MTPI).tile_indices++, (MTPI).iter_by_column[pidx]); \
            } \
        } \
        CGWTileTablePixelIteratorNext((MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH]); \
        (MTPI).pixel = (MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH].pixel; \
        (MTPI).n_pixels_row--, (MTPI).n_pixels--; \
    }
#else
#   define CGWMetaTilePixelIteratorNext(MTPI) \
    if ( (MTPI).n_pixels ) { \
        unsigned int    pidx; \
        if ( ++(MTPI).x == CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH ) { \
            (MTPI).x = 0; \
        } \
        if ( (MTPI).n_pixels_row == 0 ) { \
            (MTPI).n_pixels_row = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT; \
            for ( pidx = 0; pidx < CGW_META_TILE_WIDTH; pidx++ ) {\
                CGWTileTablePixelIteratorInit((MTPI).base_tiles, *(MTPI).tile_indices++, (MTPI).iter_by_column[pidx]); \
            } \
        } \
        CGWTileTablePixelIteratorNext((MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH]); \
        (MTPI).pixel = (MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH].pixel; \
        (MTPI).n_pixels_row--, (MTPI).n_pixels--; \
    }
#endif

/**
 * Initialize a reverse-order meta tile table tile pixel iterator
 * Initializes a meta tile iterator with data associated
 * with the meta tile at index \p MTI in table \p MTT.  The
 * iterator runs from bottom-right to top-left.
 * @param MTT       a CGWMetaTileTableRef
 * @param MTI       a meta tile index in the table \p MTT
 * @param MTPI      a CGWMetaTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWMetaTileReversePixelIteratorInit(MTT, MTI, MTPI) \
    ((MTPI).base_tiles = (MTT)->base_tiles, \
    (MTPI).tile_indices = &(MTT)->meta_tiles[(MTI)].tile_indices[CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT - 1], \
    (MTPI).x = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH, \
    (MTPI).y = CGW_META_TILE_HEIGHT * CGW_BASE_TILE_HEIGHT - 1, \
    (MTPI).n_pixels_row = 0, \
    (MTPI).n_pixels = CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#else
#   define CGWMetaTileReversePixelIteratorInit(MTT, MTI, MTPI) \
    ((MTPI).base_tiles = (MTT)->base_tiles, \
    (MTPI).tile_indices = &(MTT)->meta_tiles[(MTI)].tile_indices[CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT - 1], \
    (MTPI).x = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH, \
    (MTPI).n_pixels_row = 0, \
    (MTPI).n_pixels = CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)
#endif

/**
 * Advance to the next pixel in the reverse-order meta tile iterator
 * Sets the \p pixel field of \p MTPI to the next pixel in the
 * meta tile being iterated.  The coordinate of the pixel will be
 * present in the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels remaining.
 * @param MTPI      a CGWMetaTileTablePixelIterator
 */
#ifndef CGW_TILE_ITERATOR_NO_COORDS
#   define CGWMetaTileReversePixelIteratorNext(MTPI) \
    if ( (MTPI).n_pixels ) { \
        unsigned int    pidx; \
        if ( (MTPI).x == 0 ) { \
            --(MTPI).y, (MTPI).x = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH - 1; \
        } else { \
            --(MTPI).x; \
        } \
        if ( (MTPI).n_pixels_row == 0 ) { \
            (MTPI).n_pixels_row = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT; \
            for ( pidx = CGW_META_TILE_WIDTH; pidx > 0 ; ) {\
                --pidx; \
                CGWTileTableReversePixelIteratorInit((MTPI).base_tiles, *(MTPI).tile_indices--, (MTPI).iter_by_column[pidx]); \
            } \
        } \
        CGWTileReversePixelIteratorNext((MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH]); \
        (MTPI).pixel = (MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH].pixel; \
        (MTPI).n_pixels_row--, (MTPI).n_pixels--; \
    }
#else
#   define CGWMetaTileReversePixelIteratorNext(MTPI) \
    if ( (MTPI).n_pixels ) { \
        unsigned int    pidx; \
        if ( (MTPI).x == 0 ) { \
            (MTPI).x = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH - 1; \
        } else { \
            --(MTPI).x; \
        } \
        if ( (MTPI).n_pixels_row == 0 ) { \
            (MTPI).n_pixels_row = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT; \
            for ( pidx = CGW_META_TILE_WIDTH; pidx > 0 ; ) {\
                --pidx; \
                CGWTileTableReversePixelIteratorInit((MTPI).base_tiles, *(MTPI).tile_indices--, (MTPI).iter_by_column[pidx]); \
            } \
        } \
        CGWTileReversePixelIteratorNext((MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH]); \
        (MTPI).pixel = (MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH].pixel; \
        (MTPI).n_pixels_row--, (MTPI).n_pixels--; \
    }
#endif

/**
 * Initialize a horizontal-flip meta tile table tile pixel iterator
 * Initializes a meta tile iterator with data associated
 * with the meta tile at index \p MTI in table \p MTT.  The
 * iterator runs from top-right to bottom-left.
 * @param MTT       a CGWMetaTileTableRef
 * @param MTI       a meta tile index in the table \p MTT
 * @param MTPI      a CGWMetaTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#define CGWMetaTileHFlipPixelIteratorInit(MTT, MTI, MTPI) \
    ((MTPI).base_tiles = (MTT)->base_tiles, (MTPI).tile_indices = &(MTT)->meta_tiles[(MTI)].tile_indices[0], \
    (MTPI).x = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH, (MTPI).y = (MTPI).n_pixels_row = 0, \
    (MTPI).n_pixels = CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)

/**
 * Advance to the next pixel in the horizontal-flip meta tile iterator
 * Sets the \p pixel field of \p MTPI to the next pixel in the
 * meta tile being iterated.  The coordinate of the pixel will be
 * present in the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels remaining.
 * @param MTPI      a CGWMetaTileTablePixelIterator
 * @param MTPI      a CGWMetaTileTablePixelIterator
 */
#define CGWMetaTileHFlipPixelIteratorNext(MTPI) \
    if ( (MTPI).n_pixels ) { \
        unsigned int    pidx; \
        if ( (MTPI).x-- == 0 ) { \
            ++(MTPI).y, (MTPI).x = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH - 1; \
        } \
        if ( (MTPI).n_pixels_row == 0 ) { \
            (MTPI).n_pixels_row = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT; \
            for ( pidx = 0; pidx < CGW_META_TILE_WIDTH; pidx++ ) {\
                CGWTileTableHFlipPixelIteratorInit((MTPI).base_tiles, *(MTPI).tile_indices++, (MTPI).iter_by_column[pidx]); \
            } \
        } \
        CGWTileTableHFlipPixelIteratorNext((MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH]); \
        (MTPI).pixel = (MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH].pixel; \
        (MTPI).n_pixels_row--, (MTPI).n_pixels--; \
    }

/**
 * Initialize a vertical-flip meta tile table tile pixel iterator
 * Initializes a meta tile iterator with data associated
 * with the meta tile at index \p MTI in table \p MTT.  The
 * iterator runs from bottom-left to top-right.
 * @param MTT       a CGWMetaTileTableRef
 * @param MTI       a meta tile index in the table \p MTT
 * @param MTPI      a CGWMetaTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#define CGWMetaTileVFlipPixelIteratorInit(MTT, MTI, MTPI) \
    ((MTPI).base_tiles = (MTT)->base_tiles, \
    (MTPI).tile_indices = &(MTT)->meta_tiles[(MTI)].tile_indices[CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT - 1], \
    (MTPI).x = -1, \
    (MTPI).y = CGW_META_TILE_HEIGHT * CGW_BASE_TILE_HEIGHT - 1, \
    (MTPI).n_pixels_row = 0, \
    (MTPI).n_pixels = CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT)

/**
 * Advance to the next pixel in the vertical-flip meta tile iterator
 * Sets the \p pixel field of \p MTPI to the next pixel in the
 * meta tile being iterated.  The coordinate of the pixel will be
 * present in the \p x and \p y fields of \p TPI, and the \p n_pixels
 * will be updated to indicate the number of pixels remaining.
 * @param MTPI      a CGWMetaTileTablePixelIterator
 */
#define CGWMetaTileVFlipPixelIteratorNext(MTPI) \
    if ( (MTPI).n_pixels ) { \
        unsigned int    pidx; \
        if ( ++(MTPI).x == CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH ) { \
            --(MTPI).y, (MTPI).x = 0; \
        } \
        if ( (MTPI).n_pixels_row == 0 ) { \
            (MTPI).n_pixels_row = CGW_META_TILE_WIDTH * CGW_BASE_TILE_WIDTH * CGW_BASE_TILE_HEIGHT; \
            for ( pidx = CGW_META_TILE_WIDTH; pidx > 0 ; ) {\
                --pidx; \
                CGWTileTableVFlipPixelIteratorInit((MTPI).base_tiles, *(MTPI).tile_indices--, (MTPI).iter_by_column[pidx]); \
            } \
        } \
        CGWTileVFlipPixelIteratorNext((MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH]); \
        (MTPI).pixel = (MTPI).iter_by_column[(MTPI).x / CGW_BASE_TILE_WIDTH].pixel; \
        (MTPI).n_pixels_row--, (MTPI).n_pixels--; \
    }


#endif /* __CGWMETATILETABLE_H__ */
