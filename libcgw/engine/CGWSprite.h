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
#include "CGWBitvector.h"
#include "CGWTileTable.h"

/**
 * A sprite template
 * Each sprite is constructed of a 2D array of underlying
 * tiles.  There may be multiple instances of a sprite
 * on-screen, so rather than duplicate the tile data an
 * immutable template holds all fields that are common
 * to each instance.
 */
typedef struct  __attribute__((packed))  {
    const char          *name;      /*!< optional name for the sprite; useful for debugging */
    CGWSize             tile_dims;  /*!< the tile dimensions of the sprite; the product w * h
                                         is the number of items in the \p tiles list */
    CGWTileTableIndex   tiles[];    /*!< the tile array in row-major ordering */
} CGWSpriteTmpl;
/**
 * Reference to a sprite template
 * Since sprite templates are always dynamically-allocated, a
 * reference (pointer) is used when working with it.
 */
typedef CGWSpriteTmpl * CGWSpriteTmplRef;


typedef struct CGWSprite * CGWSpriteRef;

enum {
    kCGWSpriteIsImmutable               = (1 << 0)
};

typedef void (*CGWSpriteKeyframeCallback)(CGWSpriteRef the_sprite);
typedef bool (*CGWSpriteHitDetectCallback)(CGWSpriteRef the_sprite, CGWPointI2D );

/**
 * A sprite instance
 * A sprite is instantiated for display on-screen as an
 * object of this type.  The instance includes a reference
 * to the template object and coordinate data.
 *
 * Instances can be mutable or immutable w.r.t. the parent
 * template.  Immutable instances reference the template's
 * tile array, whereas mutable instances include their own
 * copy the template's tile array.  The goombas in SMB 1
 * were animated in coordinate fashion; this would be
 * accomplished by using immutable instances and modifying
 * the template.  Using mutable instances would allow each
 * goomba to be animated independently.
 */
typedef struct CGWSprite {
    CGWSpriteTmplRef    tmpl;           /*!< parent template for this instance */
    CGWBitvector        options;        /*!< behavioral options */
    const char          *name;          /*!< optional name for the sprite; useful for debugging */
    CGWPointFP2D        origin;         /*!< screen coordinate at which the sprite starts being
                                             drawn */
    CGWRectI2D          hit_box;        /*!< the pixel area within the sprite that is used for hit
                                             detection */
    CGWAttr             *attrs;
    CGWTileTableIndex   *tiles;         /*!< pointer to the array of tiles; for immutable instances
                                             this is just a pointer to the template's array */
    const void          *extra_data;    /*!< 
} CGWSprite;






typedef struct {
    CGWTileTableIndex       tile_index;
    CGWAttr                 attributes;
    CGWPointFP              origin;
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