/**
 * @file CGWSprite.h
 * @brief Curses Gaming Window, Graphical sprite
 * 
 * A sprite is a 2D array of tiles that can be positioned at an
 * arbitrary (x,y) coordinate in a pixel map.
 *
 */

#ifndef __CGWSPRITE_H__
#define __CGWSPRITE_H__

#include "CGWGeom.h"
#include "CGWAttr.h"
#include "CGWTileTable.h"


typedef struct {
    CGWTileTableIndex       tile_index;
    CGWAttr                 attributes;
    CGWPoint                origin;
} CGWSpriteCell;

typedef struct {
    unsigned int            n_slots;
    unsigned int            n_used;
    CGWSpriteCell           sprites[];
} CGWSpriteTable;

typedef CGWSpriteTable * CGWSpriteTableRef;

CGWSpriteTableRef CGWSpriteTableCreate(unsigned int n_slots);

#define CGWSpriteTableReset(ST) ((ST)->n_used = 0, (ST))

int CGWSpriteTableAddSprite(CGWSpriteTableRef sprite_table, CGWTileTableIndex tile_index, CGWAttr attributes, CGWPoint origin);

CGWSpriteTableRef CGWSpriteTableSort(CGWSpriteTableRef sprite_table);


/**
 * A tile index and attribute in one cell of a sprite
 * A sprite is comprised of one or more graphics tiles
 * and accompanying attributes.  We reuse and alias the
 * type that was defined by the tile map code.
 */
typedef CGWTileMapCell CGWSpriteCell;
    
/**
 * A sprite template
 * Sprites exist first as a template that defines the
 * layout of graphics tiles and base attributes; these
 * templates can be copied into a sprite table, where
 * the fields may be altered (to change palettes, flip
 * directionality, etc.).
 */
typedef struct {
    CGWSize         tile_dimensions;    /*!< the number of tiles wide and tall */
    CGWRect         collision_box;      /*!< a rectangle in the sprite's coordinate system
                                             that can collide with other objects */
    CGWSpriteCell   tile_grid[];        /*!< the flattened array of tile index and attribute
                                             comprising the sprite */
} CGWSpriteTmpl;

typedef CGWSpriteTmpl * CGWSpriteTmplPtr;



typedef struct {
    CGWSpriteTmpl   tile_data;          /*!< the baseline tile data */
    CGWPoint        origin;             /*!< the integral position */
    CGWFixedPrec    dx, dy;             /*!< the fractional position */
} CGWSprite;

typedef CGWSprite * CGWSpritePtr;

static inline
CGWSprite
CGWSpriteCreate(
    CGWSpriteTmplPtr    sprite_tmpl,
    CGWPoint            origin,
    CGWAttr             attr_mask,
    CGWAttr             attr_value
)
{
    CGWSprite           new_sprite = {
                            .tile_data = *sprite_tmpl,
                            .origin = origin,
                            .dx = CGWFixedPrecZero,
                            .dy = CGWFixedPrecZero
                        };
    if ( attr_mask ) {
        unsigned int    i = sprite_tmpl.tile_dimensions.w * sprite_tmpl.tile_dimensions.h;
        
        while ( i-- ) new_sprite.tile_data.tile_grid[i].attr =
                            new_sprite.tile_data.tile_grid[i].attr & ~attr_mask | 
                            (attr_value | attr_mask);
    }
    return new_sprite;
}

static inline
CGWSpritePtr
CGWSpriteInit(
    CGWSpritePtr        a_sprite,
    CGWSpriteTmplPtr    sprite_tmpl,
    CGWPoint            origin,
    CGWAttr             attr_mask,
    CGWAttr             attr_value
)
{
    a_sprite->sprite_tmpl = *sprite_tmpl;
    a_sprite->origin = origin;
    a_sprite->dx = a_sprite->dy = CGWFixedPrecZero;
    if ( attr_mask ) {
        unsigned int    i = sprite_tmpl.tile_dimensions.w * sprite_tmpl.tile_dimensions.h;
        
        while ( i-- ) new_sprite.tile_data.tile_grid[i].attr =
                            new_sprite.tile_data.tile_grid[i].attr & ~attr_mask | 
                            (attr_value | attr_mask);
    }
    return a_sprite;
}

static inline
CGWSpritePtr
CGWSpriteMove(
    CGWSpritePtr    a_sprite,
    CGWFixedPrec    dx,
    CGWFixedPrec    dy
)
{
    CGWFixedPrec    DX = CGWFixedPrecAdd(a_sprite->dx, dx);
    CGWFixedPrec    DY = CGWFixedPrecAdd(a_sprite->dy, dy);
    
    if ( CGWFixedPrecAbs(DX) > CGWFixedPrecOne ) {
        a_sprite->origin.x += CGWFixedPrecToInt(CGWFixedPrecExtractWhole(DX));
        DX = CGWFixedPrecExtractFrac(DX);
    }
    if ( CGWFixedPrecAbs(DY) > CGWFixedPrecOne ) {
        a_sprite->origin.y += CGWFixedPrecToInt(CGWFixedPrecExtractWhole(DY));
        DY = CGWFixedPrecExtractFrac(DY);
    }
    a_sprite->dx = DX, a_sprite->dy = DY;
}


#endif /* __CGWSPRITE_H__ */