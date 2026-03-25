/**
 * @file CGWTileView.h
 * @brief Curses Gaming Window, Graphical tile-based view
 * 
 * A view encompasses a curses window and display rectangle; a
 * tile map; a pixel buffer; and a variety of options dictating
 * how the display behaves w.r.t. the tile map.
 *
 */

#ifndef __CGWTILEVIEW_H__
#define __CGWTILEVIEW_H__

#include "CGWUtil.h"
#include "CGWBitvector.h"
#include "CGWFixedPrec.h"
#include "CGWTileMap.h"
#include "CGWPixelBuffer.h"
#include "CGWCompositePixelBuffer.h"

typedef struct {
    CGWFixedPrec        rate;
    CGWFixedPrec        delta;
    int                 absolute;
} CGWTileScroll;

#define CGWTileScrollUpdate(TS, STEP) \
    ( (TS).delta += CGWFixedPrecMul((TS).rate, (STEP)), \
      ((TS).delta >= CGWFixedPrecOne) ? \
        ((TS).absolute += CGWFixedPrecOne, ((TS).delta -= CGWFixedPrecOne, (TS).absolute += 1, +1) : \
        ((TS).delta <= CGWFixedPrecNegOne) ? ((TS).delta += CGWFixedPrecOne, (TS).absolute -= 1, -1) : 0))


typedef struct {
    CGWTileMapRef       tile_map;
    CGWTileScroll       x, y;


typedef struct {
    CGWBitvector        options;
    struct {
        WINDOW          *window;
        CGRect          display_rect;
    } curses;
    union {
        CGWTileMapRef       single;
        CGWTileMapRef       *multi;
    } tile_map;
    union {
        CGWPixelBufferRef           single;
        CGWCompositePixelBufferRef  multi;
    } pixbuf;
} CGWTileView;

#endif /* __CGWTILEVIEW_H__ */