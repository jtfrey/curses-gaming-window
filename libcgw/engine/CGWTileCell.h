/**
 * @file CGWTileCell.h
 * @brief Curses Gaming Window, Tiles
 * 
 * Types and public functions that deal with 2D grids
 * of graphics tiles.
 *
 */
 
#ifndef __CGWTILECELL_H__
#define __CGWTILECELL_H__

#include "CGWAttr.h"
#include "CGWTileTable.h"

/**
 * A tile index combined with attribute data
 * Each cell in a tile map grid or sprite is occupied by
 * a tile index (relative to a tile table associated it) and
 * attribute bits.  The attribute contains AT LEAST the
 * palette index (relative to a palette table) for drawing
 * that cell.
 */
typedef struct __attribute__((packed)) {
    CGWTileTableIndex   tile_index;     /*!< a tile index */
    CGWAttr             attributes;     /*!< attribute bits */
} CGWTileCell;
/**
 * Pointer to a tile map cell
 * Type of a pointer to a CGWTileMapCell.
 */
typedef CGWTileCell * CGWTileCellPtr;

#endif /* __CGWTILECELL_H__ */
