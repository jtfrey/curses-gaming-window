/**
 * @file CGWGeom.c
 * @brief Curses Gaming Window, Geometrical support
 * 
 * Source code accompanying CGWGeom.h.
 *
 */
 
#include "CGWGeom.h"

const CGWPoint CGWOrigin = { .x = 0, .y = 0 };

bool
CGWRectOverlapsRect(
    CGWRect     r1,
    CGWRect     r2
)
{
    int         x1, x2, y1, y2;
    
    /* If we project both rectangles onto the x-axis,
     * do their ranges overlap?
     */
    x1 = r1.origin.x + r1.size.w; x2 = r2.origin.x + r2.size.w;
    if ( (r1.origin.x >= r2.origin.x && r1.origin.x <= x2) ||
         (x1 >= r2.origin.x && x1 <= x2) ) return true;
    
    /* If we project both rectangles onto the y-axis,
     * do their ranges overlap?
     */
    y1 = r1.origin.y + r1.size.h; y2 = r2.origin.y + r2.size.h;
    if ( (r1.origin.y >= r2.origin.y && r1.origin.y <= y2) ||
         (y1 >= r2.origin.y && y1 <= y2) ) return true;
    
    return false;
}
