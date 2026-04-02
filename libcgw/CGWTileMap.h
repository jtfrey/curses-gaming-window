/**
 * @file CGWTileMap.h
 * @brief Curses Gaming Window, Tiles
 * 
 * Types and public functions that deal with 2D grids
 * of graphics tiles.
 *
 */
 
#ifndef __CGWTILEMAP_H__
#define __CGWTILEMAP_H__

#include "CGWAttr.h"
#include "CGWBitvector.h"
#include "CGWGeom.h"
#include "CGWPaletteTable.h"
#include "CGWTileTable.h"
#include "CGWTileCell.h"
#include "CGWMetaTileTable.h"
#include "CGWDecoder.h"

/**
 * Optional properties of a CGWTileMap object
 * A bit vector of different properties/behaviors for a CGWTileMap
 * object.
 */
enum {
    kCGWTileMapOptionsTileTableTypeBase         = 0,        /*!< the map references a CGWTileTable */
    kCGWTileMapOptionsTileTableTypeMeta         = 1,        /*!< the map references a CGWMetalTileTable */
    kCGWTileMapOptionsTileTableTypeMask         = 0b11,     /*!< the lowest two bits are reserved for the type of
                                                                 tiles referenced */
    kCGWTileMapOptionsShouldDestroyTileTable    = (1 << 2), /*!< set if the tile table should be destroyed when
                                                                 this tile map is destroyed */
                                                             
    kCGWTileMapOptionsMask = kCGWTileMapOptionsTileTableTypeMask |
                             kCGWTileMapOptionsShouldDestroyTileTable /*!< all bits in use */
};

/**
 * A 2D grid of tiles
 * Given a color palette table and a tile table, a CGWTileMap is a 2D grid
 * of references as (tile index, color palette index).  A CGWTileMap can
 * reference either a CGWTileTable or a CGWMetaTileTable.
 *
 * The 2D grid exists as a flattened list of CGWTileCell structures.  The
 * array is arranged in column-major order, reflecting the optimal filling
 * of a horizontally-scrolling graphics buffer.
 */
typedef struct {
    CGWBitvector            options;        /*!< the tile table type, other options */
    CGWSize                 dimensions;     /*!< the width vs. height of the tile map (in cells) */
    CGWPaletteTable         palette_table;  /*!< the color palette table associated with the map */
    union {
        CGWTileTableRef     base;           /*!< reference to the associated CGWTileTable */
        CGWMetaTileTableRef meta;           /*!< reference to the associated CGWMetalTileTable */
    } tile_table;                           /*!< union of tile table reference types */
    CGWTileCell             tile_grid[];     /*!< the array of grid cell structures */
} CGWTileMap;
/**
 * Reference to a tile map
 * Type of a reference to a CGWTileMap.  Since a tile map is always dynamically-allocated, a
 * reference (pointer) is used when working with one.
 */
typedef CGWTileMap * CGWTileMapRef;

/**
 * Allocate and initialize a CGWTileMap
 * Allocate a CGWTileMap with a 2D cell grid of the given \p dimensions.  The
 * color palette table at \p palette_table is copied into the CGWTileMap
 * instance and the type of tile table and other properties/behaviors of the
 * new instance are passed in \p options.  If a \ref CGWTileTable is being used,
 * \p options must indicate \ref kCGWTileMapOptionsTileTableTypeBase and the
 * \ref CGWTileTableRef passed as the fourth argument; otherwise, \p options
 * must indicate \ref kCGWTileMapOptionsTileTableTypeMeta and the \ref
 * CGWMetaTileTable passed as the fourth argument.
 * @param dimensions            the size of the tile grid
 * @param options               properties/behaviors of the instance
 * @param palette_table         the color palette table to be associated with
 *                              the tile map
 * @return                      a newly-initialized CGWTileMap reference or
 *                              NULL on error
 */
CGWTileMapRef CGWTileMapCreate(CGWSize dimensions, CGWBitvector options, CGWPaletteTablePtr palette_table, ...);

/**
 * Destroy a CGWTileMap
 * Dispose of all resources associated with \p tile_map.
 *
 * If \p tile_map was created with the \ref kCGWTileMapOptionsShouldDestroyTileTable
 * option, the CGWTileTable/CGWMetaTileTable associated with \p tile_map is also
 * destroyed.
 * @param tile_map      the CGWTileMap to destroy
 */
void CGWTileMapDestroy(CGWTileMapRef tile_map);

/**
 * Which tile table type is used by the tile map
 * Returns the tile table type associated with \p TM, as indicated by
 * the constants defined in \ref CGWTileMapOptions.
 * @param TM            the CGWTileMap
 * @return              the tile table type used by \p TM
 */
#define CGWTileMapTileTableType(TM) CGWBitvectorGetBits((TM)->options, kCGWTileMapOptionsTileTableTypeMask)

/**
 * Map a 2D grid cell coordinate to an array index
 * Returns the flat array index associated with grid coordinate (X, Y).
 *
 * The coordinate (0,0) is the top-left of the map.  Note that no bounds-checks
 * are performed on the coordinate.
 * @param TM            the CGWTileMap
 * @param X             the x-coordinate
 * @param Y             the y-coordinate
 * @return              the computed array index
 */
#define CGWTileMapComputeCellIndex(TM, X, Y) ((X)*(TM)->dimensions.height+(Y))

/**
 * Grid cell value at a 2D grid coordinate.
 * Returns the grid cell value associated with grid coordinate (X, Y).
 *
 * The coordinate (0,0) is the top-left of the map.  Note that no bounds-checks
 * are performed on the coordinate.
 * @param TM            the CGWTileMap
 * @param X             the x-coordinate
 * @param Y             the y-coordinate
 * @return              the CGWTileCell at the coordinate
 */
#define CGWTileMapGetCell(TM, X, Y) ((TM)->tile_grid[CGWTileMapComputeCellIndex(TM, X, Y)])

/**
 * Grid cell pointer at a 2D grid coordinate.
 * Returns a pointer to the grid cell associated with grid coordinate (X, Y).
 *
 * The coordinate (0,0) is the top-left of the map.  Note that no bounds-checks
 * are performed on the coordinate.
 * @param TM            the CGWTileMap
 * @param X             the x-coordinate
 * @param Y             the y-coordinate
 * @return              the CGWTileCellPtr at the coordinate
 */
#define CGWTileMapGetCellPtr(TM, X, Y) (&(TM)->tile_grid[CGWTileMapComputeCellIndex(TM, X, Y)])

/**
 * Create a CGWTileMap using data from a decoder
 * A CGWTileMap is read from the \p decoder.
 *
 * A signature word ('TMAP') precedes the tile map data.
 * A header follows:
 *
 *     uint32_t         width
 *     uint32_t         height
 *     uint32_t         options
 *     <PLTT> …         the palette table for the map
 *
 * Depending on the type of tile comprising the map, what
 * follows will be either a base- or meta-tile table:
 *
 *     <TTBL> …         the encoded base tile table
 *       -or-
 *     <MTBL> …         the encoded base tile table
 *
 * Following the tile table, the grid of CGWTileCell
 * structures follows (width * height records).
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param tile_map      pointer to a CGWTileMapRef to be
 *                      filled-in with a reference to the
 *                      allocated object
 * @return              Boolean true if successful
 */
bool CGWTileMapCreateWithDecoder(CGWDecoderRef decoder, CGWTileMapRef *tile_map);

/**
 * Tile map iterator context
 * Data structure used to hold state for the iteration over
 * the cells of a tile map.
 */
typedef struct {
    CGWTileCell         *base_tile_cell;    /*!< pointer to the starting cell in the grid */\
    unsigned int        x;                  /*!< x-coordinate of the current cell in the grid */
    unsigned int        y;                  /*!< y-coordinate of the current cell in the grid */
    int                 stride;             /*!< increment to move between columns/rows */
    unsigned int        n_cells;            /*!< number of cells remaining */
    bool                is_started;         /*!< has the iterator been called yet? */
} CGWTileMapCellIterator;

/**
 * Initialize a tile map column iterator
 * Prepares a \ref CGWTileMapCellIterator data structure \p TMI
 * to iterate over the cells in column \p X of \p TM from top to
 * bottom.
 *
 * The stride is initialized but never actually used.
 * @param TM            a CGWTileMapRef
 * @param X             the x-coordinate of the column to iterate
 * @param TMI           the CGWTileMapCellIterator to init
 * @return              the number of cells to be iterated
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#   define CGWTileMapCellColumnIteratorInit(TM, X, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[(X) * (TM)->dimensions.h], \
     (TMI).x = (X), (TMI).y = 0, \
     (TMI).stride = 1, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.h)
#else
#   define CGWTileMapCellColumnIteratorInit(TM, X, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[(X) * (TM)->dimensions.h], \
     (TMI).stride = 1, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.h)
#endif

/**
 * Advance to the next cell in a column iterator
 * Advance the iterator \p TMI to the next cell and return a
 * CGWTileCellPtr to the cell.
 * @param TMI       the CGWTileMapCellIterator
 * @return          NULL if the iterator is complete, a CGWTileCellPtr
 *                  otherwise
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#   define CGWTileMapCellColumnIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? ((TMI).y++, ++(TMI).base_tile_cell) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#else
#   define CGWTileMapCellColumnIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? (++(TMI).base_tile_cell) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#endif

/**
 * Initialize a tile map row iterator
 * Prepares a \ref CGWTileMapCellIterator data structure \p TMI
 * to iterate over the cells in row \p Y of \p TM from top to
 * bottom.
 * @param TM            a CGWTileMapRef
 * @param Y             the y-coordinate of the row to iterate
 * @param TMI           the CGWTileMapCellIterator to init
 * @return              the number of cells to be iterated
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#   define CGWTileMapCellRowIteratorInit(TM, Y, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[(Y)], \
     (TMI).x = 0, (TMI).y = (Y), \
     (TMI).stride = (TM)->dimensions.h, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.w)
#else
#   define CGWTileMapCellRowIteratorInit(TM, Y, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[(Y)], \
     (TMI).stride = (TM)->dimensions.h, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.w)
#endif

/**
 * Advance to the next cell in a row iterator
 * Advance the iterator \p TMI to the next cell and return a
 * CGWTileCellPtr to the cell.
 * @param TMI       the CGWTileMapCellIterator
 * @return          NULL if the iterator is complete, a CGWTileCellPtr
 *                  otherwise
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#   define CGWTileMapCellRowIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? ((TMI).x++, (TMI).base_tile_cell += (TMI).stride) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#else
#   define CGWTileMapCellRowIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? ((TMI).base_tile_cell += (TMI).stride) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#endif

/**
 * Initialize a reverse-order tile map column iterator
 * Prepares a \ref CGWTileMapCellIterator data structure \p TMI
 * to iterate over the cells in column \p X of \p TM from bottom to
 * top.
 * @param TM            a CGWTileMapRef
 * @param X             the x-coordinate of the column to iterate
 * @param TMI           the CGWTileMapCellIterator to init
 * @return              the number of cells to be iterated
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#   define CGWTileMapCellReverseColumnIteratorInit(TM, X, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[((X)+1) * (TM)->dimensions.h - 1], \
     (TMI).x = (X), (TMI).y = (TM)->dimensions.h - 1, \
     (TMI).stride = -1, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.h)
#else
#   define CGWTileMapCellReverseColumnIteratorInit(TM, X, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[((X)+1) * (TM)->dimensions.h - 1], \
     (TMI).stride = -1, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.h)
#endif

/**
 * Advance to the next cell in a reverse-order column iterator
 * Advance the iterator \p TMI to the next cell and return a
 * CGWTileCellPtr to the cell.
 * @param TMI       the CGWTileMapCellIterator
 * @return          NULL if the iterator is complete, a CGWTileCellPtr
 *                  otherwise
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#    define CGWTileMapCellReverseColumnIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? ((TMI).y--, --(TMI).base_tile_cell) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#else
#    define CGWTileMapCellReverseColumnIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? (--(TMI).base_tile_cell) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#endif

/**
 * Initialize a reverse-order tile map row iterator
 * Prepares a \ref CGWTileMapCellIterator data structure \p TMI
 * to iterate over the cells in row \p Y of \p TM from top to
 * bottom.
 * @param TM            a CGWTileMapRef
 * @param Y             the y-coordinate of the row to iterate
 * @param TMI           the CGWTileMapCellIterator to init
 * @return              the number of cells to be iterated
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#   define CGWTileMapCellReverseRowIteratorInit(TM, Y, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[((TM)->dimensions.w - 1) * (TM)->dimensions.h + (Y)], \
     (TMI).x = (TM)->dimensions.w - 1, (TMI).y = (Y), \
     (TMI).stride = -(TM)->dimensions.h, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.w)
#else
#   define CGWTileMapCellReverseRowIteratorInit(TM, Y, TMI) \
    ((TMI).base_tile_cell = &(TM)->tile_grid[((TM)->dimensions.w - 1) * (TM)->dimensions.h + (Y)], \
     (TMI).stride = -(TM)->dimensions.h, (TMI).is_started = 0, (TMI).n_cells = (TM)->dimensions.w)
#endif

/**
 * Advance to the next cell in a row iterator
 * Advance the iterator \p TMI to the next cell and return a
 * CGWTileCellPtr to the cell.
 * @param TMI       the CGWTileMapCellIterator
 * @return          NULL if the iterator is complete, a CGWTileCellPtr
 *                  otherwise
 */
#ifndef CGW_TILEMAP_ITERATOR_NO_COORDS
#   define CGWTileMapCellReverseRowIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? ((TMI).x--, (TMI).base_tile_cell += (TMI).stride) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#else
#   define CGWTileMapCellReverseRowIteratorNext(TMI) \
    ( (TMI).n_cells ? ( (TMI).n_cells--, \
        ((TMI).is_started ? ((TMI).base_tile_cell += (TMI).stride) : \
            ((TMI).is_started = 1, TMI.base_tile_cell))) : NULL )
#endif

#endif /* __CGWTILEMAP_H__ */
