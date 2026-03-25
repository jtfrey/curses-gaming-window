/**
 * @file CGWTileTable.h
 * @brief Curses Gaming Window, Table of tiles
 * 
 * Types and public functions that deal with an array of
 * tile data.
 *
 */
 
#ifndef __CGWTILETABLE_H__
#define __CGWTILETABLE_H__

#include "CGWTile.h"
#include "CGWDecoder.h"

/**
 * Type of indices into tile tables
 * With the configured index limit, this macro stands-in
 * for the smallest integer type permissible.
 */
#if CGW_TILE_TABLE_MAX_INDEX_BITS > 32
#   error Tile table indices exceeding 32-bits are not implemented.
#elif CGW_TILE_TABLE_MAX_INDEX_BITS > 16
#   define CGWTileTableIndexFmt "0x%08X"
#   define CGWTileTableIndexCodingType kCGWDecoderItem32Bit
#   define CGWTileTableIndex uint32_t
#elif CGW_TILE_TABLE_MAX_INDEX_BITS > 8
#   define CGWTileTableIndexFmt "0x%04X"
#   define CGWTileTableIndexCodingType kCGWDecoderItem16Bit
#   define CGWTileTableIndex uint16_t
#else
#   define CGWTileTableIndexFmt "0x%02X"
#   define CGWTileTableIndexCodingType kCGWDecoderItem8Bit
#   define CGWTileTableIndex uint8_t
#endif

/**
 * An array of graphics tiles
 * A dynamically-sized array of CGWTile objects.  The tiles are all
 * of the configured pixel dimensions \ref CGW_BASE_TILE_WIDTH and
 * \ref CGW_BASE_TILE_HEIGHT.
 */
typedef struct {
    unsigned int        n_tiles;    /*!< the number of tiles in the array */
    unsigned int        n_set;      /*!< filled-in by the \ref CGWTileTableCreateWithDecoder()
                                         function when used with the indexed tile sub-type;
                                         otherwise equals n_tiles */
    CGWTile             tiles[];    /*!< the array of tiles */
} CGWTileTable;

/**
 * Reference to a table of base tiles
 * Since an array of base tiles is always dynamically-allocated, a
 * reference (pointer) is used when working with it.
 */
typedef CGWTileTable * CGWTileTableRef;

/**
 * Create an empty tile table with n_tiles slots.
 * Dynamically-allocate a new CGWTileTable with room for \p n_tiles
 * graphics tiles.
 * @param n_tiles       number of array slots
 * @return              a reference to the newly-initialized tile table
 *                      or NULL on error
 */
CGWTileTableRef CGWTileTableCreate(unsigned int n_tiles);

/**
 * Destroy a tile table
 * Dispose of memory occupied by the \p tile_table.
 * @param tile_table    the tile table to dispose of
 */
void CGWTileTableDestroy(CGWTileTableRef tile_table);

/**
 * Create a CGWTileTable using data from a decoder
 * A CGWTileTable is read from the \p decoder.
 *
 * A signature word ('TTBL') precedes the tile data.  A header
 * follows:
 *
 *     uint32_t         n_tiles
 *     uint16_t         bits_per_pixel
 *     uint32_t         width
 *     uint32_t         height
 *     uint16_t         sub_type
 *
 * A sequence of encoded CGWTile structures follows.  If the
 * sub_type is 0, the full table of all \p n_tiles tiles
 * are present.  If the sub_type is 1, then each tile is
 * preceded by an index at which it should be written into
 * the tile array and tile reading terminates either once
 * \p n_tiles have been read OR when an index of
 * 0xFFFF = 65535 is encountered.
 *
 * If the width, height, and bpp do not match the configured
 * \ref CGW_BASE_TILE_WIDTH, \ref CGW_BASE_TILE_HEIGHT, and
 * \ref CGW_BITS_PER_PIXEL, the tile table will fail to load.
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param tile_table    pointer to a CGWTileTableRef to
 *                      be filled-in with a reference to the
 *                      allocated object
 * @return              Boolean true if successful
 */
bool CGWTileTableCreateWithDecoder(CGWDecoderRef decoder, CGWTileTableRef *tile_table);

/**
 * A tile iterator for use with CGWTileTable objects.
 * A tile table being an array of CGWTile objects, the
 * iterator for a tile at a given index just reuses
 * the CGWTilePixelIterator data structure and API.
 */
#define CGWTileTablePixelIterator CGWTilePixelIterator

/**
 * Initialize a tile table tile pixel iterator
 * Calls the underlying \ref CGWTilePixelIteratorInit() API
 * to initialize a tile iterator with data associated
 * with the tile at index \p TI in table \p TT.
 * @param TT        a CGWTileTableRef
 * @param TI        a tile index in the table \p TT
 * @param TPI       a CGWTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#define CGWTileTablePixelIteratorInit(TT, TI, TPI) \
    CGWTilePixelIteratorInit((TT)->tiles[(TI)], (TPI))

/**
 * Advance to the next pixel in the tile iterator
 * Calls the underlying \ref CGWTilePixelIteratorNext() API
 * to advance to the next pixel in the tile.
 * @param TPI   a CGWTileTablePixelIterator
 */
#define CGWTileTablePixelIteratorNext(TPI) \
    CGWTilePixelIteratorNext((TPI))

/**
 * Initialize a reverse-order tile table tile pixel iterator
 * Calls the underlying \ref CGWTileReversePixelIteratorInit() API
 * to initialize a tile iterator with data associated
 * with the tile at index \p TI in table \p TT.
 * @param TT        a CGWTileTableRef
 * @param TI        a tile index in the table \p TT
 * @param TPI       a CGWTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#define CGWTileTableReversePixelIteratorInit(TT, TI, TPI) \
    CGWTileReversePixelIteratorInit((TT)->tiles[(TI)], (TPI))

/**
 * Advance to the next pixel in the reverse-order tile iterator
 * Calls the underlying \ref CGWTileReversePixelIteratorNext() API
 * to advance to the next pixel in the tile.
 * @param TPI   a CGWTileTablePixelIterator
 */
#define CGWTileTableReversePixelIteratorNext(TPI) \
    CGWTileReversePixelIteratorNext((TPI))

/**
 * Initialize a vertical-flip tile table tile pixel iterator
 * Calls the underlying \ref CGWTileVFlipPixelIteratorInit() API
 * to initialize a tile iterator with data associated
 * with the tile at index \p TI in table \p TT.
 * @param TT        a CGWTileTableRef
 * @param TI        a tile index in the table \p TT
 * @param TPI       a CGWTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#define CGWTileTableVFlipPixelIteratorInit(TT, TI, TPI) \
    CGWTileVFlipPixelIteratorInit((TT)->tiles[(TI)], (TPI))

/**
 * Advance to the next pixel in the vertical-flip tile iterator
 * Calls the underlying \ref CGWTileVFlipPixelIteratorNext() API
 * to advance to the next pixel in the tile.
 * @param TPI   a CGWTileTablePixelIterator
 */
#define CGWTileTableVFlipPixelIteratorNext(TPI) \
    CGWTileVFlipPixelIteratorNext((TPI))

/**
 * Initialize a horizontal-flip tile table tile pixel iterator
 * Calls the underlying \ref CGWTileHFlipPixelIteratorInit() API
 * to initialize a tile iterator with data associated
 * with the tile at index \p TI in table \p TT.
 * @param TT        a CGWTileTableRef
 * @param TI        a tile index in the table \p TT
 * @param TPI       a CGWTileTablePixelIterator
 * @return          the number of pixels to be iterated
 */
#define CGWTileTableHFlipPixelIteratorInit(TT, TI, TPI) \
    CGWTileHFlipPixelIteratorInit((TT)->tiles[(TI)], (TPI))

/**
 * Advance to the next pixel in the horizontal-flip tile iterator
 * Calls the underlying \ref CGWTileHFlipPixelIteratorNext() API
 * to advance to the next pixel in the tile.
 * @param TPI   a CGWTileTablePixelIterator
 */
#define CGWTileTableHFlipPixelIteratorNext(TPI) \
    CGWTileHFlipPixelIteratorNext((TPI))

#endif /* __CGWTILETABLE_H__ */
