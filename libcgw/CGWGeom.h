/**
 * @file CGWGeom.h
 * @brief Curses Gaming Window, Geometrical support
 * 
 * Types and functions that implement basic geometric utilities.
 *
 */

#ifndef __CGWGEOM_H__
#define __CGWGEOM_H__

#include "CGWConfig.h"

/**
 * A 2D point
 * A point in a two-dimensional integral coordinate system
 */
typedef struct {
    int     x;      /*!< the x-coordinate */
    int     y;      /*!< the y-coordinate */
} CGWPoint;

/**
 * The Cartesian origin
 * A constant holding the Cartesian origin, (0,0)
 */
extern const CGWPoint CGWOrigin;

/**
 * Create a CGWPoint from component values
 * A convenience function that manufactures a CGWPoint
 * structure from component coordinates.
 * @param x     x-coordinate
 * @param y     y-coordinate
 * @return      A CGWPoint structure containing the values
 */
static inline
CGWPoint
CGWPointMake(
    int         x,
    int         y
)
{
    CGWPoint    the_point = { .x = x, .y = y };
    return the_point;
}

/**
 * Sum of two CGWPoint coordinates
 * A convenience function that manufactures a CGWPoint
 * structure as the sum of the two input points.
 * @param p1    first coordinate
 * @param p2    second coordinate
 * @return      A CGWPoint structure containing the sum
 */
static inline
CGWPoint
CGWPointSum(
    CGWPoint    p1,
    CGWPoint    p2
)
{
    return CGWPointMake(p2.x + p1.x, p2.y + p1.y);
}

/**
 * Difference of two CGWPoint coordinates
 * A convenience function that manufactures a CGWPoint
 * structure as the difference, \p p2 - \p p1.
 * @param p1    first coordinate
 * @param p2    second coordinate
 * @return      A CGWPoint structure containing the difference
 */
static inline
CGWPoint
CGWPointDiff(
    CGWPoint    p1,
    CGWPoint    p2
)
{
    return CGWPointMake(p2.x - p1.x, p2.y - p1.y);
}

/**
 * Dimensions in 2D
 * A width and height
 */
typedef struct {
    unsigned int    w;      /*!< the x-distance */
    unsigned int    h;      /*!< the y-distance */
} CGWSize;

/**
 * Create a CGWSize from component values
 * A convenience function that manufactures a CGWSize
 * structure from component coordinates.
 * @param w     x-distance
 * @param h     y-distance
 * @return      A CGWSize structure containing the values
 */
static inline
CGWSize
CGWSizeMake(
    unsigned int    w,
    unsigned int    h
)
{
    CGWSize         the_size = { .w = w, .h = h };
    return the_size;
}

/**
 * A 2D rectangle
 * A rectangle consists of a point of origin and distances in
 * the two coordinate directions (size).  The size should be
 * kept positive, with the origin always chosen to fulfill that
 * condition.
 */
typedef struct {
    CGWPoint    origin; /*!< the origin of the rectangle */
    CGWSize     size;   /*!< the width and height */
} CGWRect;

/**
 * Create a CGWRect from component values
 * A convenience function that manufactures a CGWRect
 * structure from component coordinates and distances.
 * @param x     x-coordinate of origin
 * @param y     y-coordinate of origin
 * @param w     width in the x-direction
 * @param h     height in the y-direction
 * @return      A CGWRect structure containing the values
 */
static inline
CGWRect
CGWRectMake(
    int             x,
    int             y,
    unsigned int    w,
    unsigned int    h
)
{
    CGWRect     the_rect = { .origin.x = x, .origin.y = y,
                             .size.w = w, .size.h = h };
    return the_rect;
}

/**
 * Create a CGWRect from two points
 * A convenience function that manufactures a CGWRect
 * structure according to the coordinates of two
 * opposing corners of the rectangle.
 * @param p1    First corner
 * @param p2    Opposing corner to p1
 * @return      A CGWRect structure containing the
 *              rectangle's origin and size
 */
static inline
CGWRect
CGWRectMakeWithPoints(
    CGWPoint    p1,
    CGWPoint    p2
)
{
    int         dx = p2.x - p1.x;
    int         dy = p2.y - p1.y;
    
    return CGWRectMake(
        ( dx < 0 ) ? p2.x : p1.x,
        ( dy < 0 ) ? p2.y : p1.y,
        ( dx < 0 ) ? -dx : dx,
        ( dy < 0 ) ? -dy : dy);
}

/**
 * Does a rectangle contain a point?
 * Determine if rectangle \p r contains the given
 * point \p p.
 * @param r     a rectangle
 * @param p     the point to test
 * @return      Boolean true if \p p is contained
 *              by \p r
 */
static inline
bool
CGWRectContainsPoint(
    CGWRect     r,
    CGWPoint    p
)
{
    int         dx = p.x - r.origin.x,
                dy = p.y - r.origin.y;
    return ((dx > 0) && (dx < r.size.w) && (dy > 0) && (dy < r.size.h)) ? true : false;
}

/**
 * Do two rectangles overlap each other?
 * Determine if the region occupied by rectangle \p r1
 * intersects the region occupied by \p r2
 * @param r1    a rectangle
 * @param r2    another rectangle
 * @return      Boolean true if \p r1 and \p r2
 *              overlap
 */
bool CGWRectOverlapsRect(CGWRect r1, CGWRect r2);

#endif /* __CGWGEOM_H__ */