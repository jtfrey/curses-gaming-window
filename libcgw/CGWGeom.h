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
#include "CGWMath.h"
#include "CGWFixedPrec.h"

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
 * A point augmented with fractional digits
 * A CGWPoint that is augmented by fractional x- and y-coordinate
 * offsets.  This forms the basis of the "sub-pixel" handling of
 * more-precise animation mechanics.  The granularity of the
 * fractional coordinate is dictated by the definition of the
 * CGWFixedPrec format.
 *
 * The \ref CGWPointFPStandardize() function can be used to
 * standardize the coordinates to keep the fractional components
 * positive.
 */
typedef struct {
    CGWPoint        whole;      /*!< the integer x- and y-coordiate values */
    CGWFixedPrec    fx, fy;     /*!< the fractional coordinate values; should be
                                     kept strictly positive */
} CGWPointFP;

/**
 * Standardize the fraction coordinates of \p pfp
 * Fractional coordinate components must be kept strictly positive.
 * E.g. a whole x-coordinate of 5 with a fraction coordinate of
 * -1/4 is standardized to 4 and 3/4, respectively.
 * @param pfp       pointer to the fixed-precison augmented
 *                  coordinate to fix
 */
void CGWPointFPStandardize(CGWPointFP *pfp);

/**
 * Duplicate a coordinate and return the standardized form
 * Convenience function that duplicates the fixed-precison augmented
 * coordinate \p pfp and returns the standardized form.
 * @param pfp       the fixed-precision augmented coordinate
 *                  to fix
 * @return          the standardized coordinate
 */
static inline
CGWPointFP
CGWPointFPStandardized(
    CGWPointFP      pfp
)
{
    CGWPointFPStandardize(&pfp);
    return pfp;
}

/**
 * Create a CGWPointFP from floating-point coordinates
 * Converts the floating-point coordinate values \p x and \p y
 * to whole and fractional components and returns a CGWPointFP
 * initialized with the closest representable values.
 * @param x             the x-coordinate
 * @param y             the y-coordinate
 * @return              the CGWPointFP coordinate
 */
static inline
CGWPointFP
CGWPointFPMake(
    double      x,
    double      y
)
{
    CGWPointFP  the_point;
    double      fx = modf(x, &x),
                fy = modf(y, &y);
    the_point.whole = CGWPointMake((int)x, (int)y),
        the_point.fx = CGWFixedPrecCreateWithDouble(fx),
        the_point.fy = CGWFixedPrecCreateWithDouble(fy);
    return the_point;
}

/**
 * Translate a CGWPointFP by whole integers
 * Given the whole integer values \p dx and \p dy, the
 * CGWPointFP pointed to by \p pfp is translated by those
 * distances.  The resulting value is standardized.
 * @param pfp           pointer to the CGWPointFP to translate
 * @param dx            the shift in the x-direction
 * @param dy            the shift in the y-direction
 */
static inline
void
CGWPointFPTranslateWhole(
    CGWPointFP      *pfp,
    int             dx,
    int             dy
)
{
    pfp->whole.x += dx, pfp->whole.y += dy;
    CGWPointFPStandardize(pfp);
}

/**
 * Translate a CGWPointFP by fractional distances
 * Given the fractional values \p dx and \p dy, the
 * CGWPointFP pointed to by \p pfp is translated by those
 * distances.  The resulting value is standardized.
 * @param pfp           pointer to the CGWPointFP to translate
 * @param dx            the shift in the x-direction
 * @param dy            the shift in the y-direction
 */
static inline
void
CGWPointFPTranslateFrac(
    CGWPointFP      *pfp,
    CGWFixedPrec    dx,
    CGWFixedPrec    dy
)
{
    CGWFixedPrec    DX = CGWFixedPrecAdd(pfp->fx, dx),
                    DY = CGWFixedPrecAdd(pfp->fy, dy);
    pfp->fx = CGWFixedPrecMod(DX, &DX);
    pfp->fy = CGWFixedPrecMod(DY, &DY);
    pfp->whole.x += CGWFixedPrecToInt(DX);
    pfp->whole.y += CGWFixedPrecToInt(DY);
    CGWPointFPStandardize(pfp);
}

/**
 * Sum two CGWPointFP coordinates
 * The coordinate \p pfp1 is translated by coordinate \p pfp2
 * (with \p pfp2 treated like a vector).
 * @param pfp1          the first fixed-point augmented coordinate
 * @param pfp2          the second fixed-point augmented coordinate
 *                      (treated like a vector)
 * @return              a new coordinate that is the first coordinate
 *                      translated by the second
 */
static inline
CGWPointFP
CGWPointFPSum(
    CGWPointFP      pfp1,
    CGWPointFP      pfp2
)
{
    CGWPointFPTranslateFrac(&pfp1, pfp2.fx, pfp2.fy);
    CGWPointFPTranslateWhole(&pfp1, pfp2.whole.x, pfp2.whole.y);
    return pfp1;
}

/**
 * Difference two CGWPointFP coordinates
 * The coordinate \p pfp1 is translated by negated coordinate \p pfp2
 * (with \p pfp2 treated like a negated vector).
 * @param pfp1          the first fixed-point augmented coordinate
 * @param pfp2          the second fixed-point augmented coordinate
 *                      (treated like a vector)
 * @return              a new coordinate that is the first coordinate
 *                      translated by the negated second
 */
static inline
CGWPointFP
CGWPointFPDiff(
    CGWPointFP      pfp1,
    CGWPointFP      pfp2
)
{
    CGWPointFPTranslateFrac(&pfp1, -pfp2.fx, -pfp2.fy);
    CGWPointFPTranslateWhole(&pfp1, -pfp2.whole.x, -pfp2.whole.y);
    return pfp1;
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

/**
 * A 2D circle
 * A circle consists of an origin point and a radius.  The radius
 * must be positive.
 */
typedef struct {
    CGWPoint        origin;     /*!< the point at which the circle is centered */
    unsigned int    radius;     /*!< the radius of the circle */
} CGWCircle;

/**
 * Create a CGWCircle from component values
 * A convenience function that manufactures a CGWCircle
 * structure from component coordinates and radius.
 * @param x     x-coordinate of origin
 * @param y     y-coordinate of origin
 * @param r     radius
 * @return      A CGWCircle structure containing the values
 */
static inline
CGWCircle
CGWCircleMake(
    int             x,
    int             y,
    unsigned int    r
)
{
    CGWCircle       new_circle = {
                        .origin.x = x,
                        .origin.y = y,
                        .radius = r
                    };
    return new_circle;
}

/**
 * Does a circle contain a point?
 * Determine if circle \p c contains the given
 * point \p p.
 * @param c     a circle
 * @param p     the point to test
 * @return      Boolean true if \p p is contained
 *              by \p c
 */
static inline
bool
CGWCircleContainsPoint(
    CGWCircle       c,
    CGWPoint        p
)
{
    int64_t         dist, rem;
    int64_t         dx = c.origin.x - p.x,
                    dy = c.origin.y - p.y;
    dist = CGWSqrtInt64(dx * dx + dy * dy, &rem);
    return ((dist < c.radius) || ((dist == c.radius) && (rem == 0)));
}

/**
 * Do two circles overlap each other?
 * Determine if the region occupied by circle \p c1
 * intersects the region occupied by \p c2
 * @param c1    a circle
 * @param c2    another circle
 * @return      Boolean true if \p c1 and \p c2
 *              overlap
 */
static inline
bool
CGWCircleOverlapsCircle(
    CGWCircle       c1,
    CGWCircle       c2
)
{
    int64_t         dist, rem, odist = c2.radius + c1.radius;
    int64_t         dx = c2.origin.x - c1.origin.x,
                    dy = c2.origin.y - c1.origin.y;
    dist = CGWSqrtInt64(dx * dx + dy * dy, &rem);
    return ((dist < odist) || ((dist == odist) && (rem == 0)));
}

/**
 * Do a circle and rectangle overlap?
 * The point on the perimeter of the rectangle nearest to the
 * center of the circle is found, and if it is ≤ the radius of the
 * circle away from the circle's center, the two figures overlap.
 * @param r         a rectangle
 * @param c         a circle
 * @return          boolean true if the two figures overlap,
 *                  false otherwise
 */
bool CGWRectOverlapsCircle(CGWRect r, CGWCircle c);

/**
 * Alias circle-rect to the rect-circle function
 * In case the programmer forgets which function is actually
 * defined -- rect then circle vs. circle then rect -- this
 * macro rewrites the latter to be the former.
 * @param C         a circle
 * @param R         a rectangle
 * @return          boolean true if the two figures overlap,
 *                  false otherwise
 */
#define CGWCircleOverlapsRect(C, R) CGWRectOverlapsCircle((R), (C))

#endif /* __CGWGEOM_H__ */