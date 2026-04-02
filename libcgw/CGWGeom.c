/**
 * @file CGWGeom.c
 * @brief Curses Gaming Window, Geometrical support
 * 
 * Source code accompanying CGWGeom.h.
 *
 */
 
#include "CGWGeom.h"

const CGWPoint CGWOrigin = { .x = 0, .y = 0 };

void
CGWPointFPStandardize(
    CGWPointFP      *pfp
)
{
    int             w_neg, f_neg;
    
    if ( (pfp->whole.x != 0) && (pfp->fx != CGWFixedPrecZero) ) {
        w_neg = (pfp->whole.x < 0), f_neg = CGWFixedPrecIsNeg(pfp->fx);    
        if ( w_neg != f_neg) {
            if ( w_neg ) {
                /* whole is negative, frac is positive:  add 1 to whole, subtract 1 from frac */
                pfp->whole.x++, pfp->fx = CGWFixedPrecAdd(pfp->fx, CGWFixedPrecNegOne);
            }
            else {
                /* frac is negative, whole is positive:  subtract 1 from whole, add 1 to frac */
                pfp->whole.x--, pfp->fx = CGWFixedPrecAdd(pfp->fx, CGWFixedPrecOne);
            }
        }
    }
    if ( (pfp->whole.y != 0) && (pfp->fy != CGWFixedPrecZero) ) {
        w_neg = (pfp->whole.y < 0), f_neg = CGWFixedPrecIsNeg(pfp->fy);    
        if ( w_neg != f_neg) {
            if ( w_neg ) {
                /* whole is negative, frac is positive:  add 1 to whole, subtract 1 from frac */
                pfp->whole.y++, pfp->fy = CGWFixedPrecAdd(pfp->fy, CGWFixedPrecNegOne);
            }
            else {
                /* frac is negative, whole is positive:  subtract 1 from whole, add 1 to frac */
                pfp->whole.y--, pfp->fy = CGWFixedPrecAdd(pfp->fy, CGWFixedPrecOne);
            }
        }
    }
}

//

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

//

bool
CGWRectOverlapsCircle(
    CGWRect     r,
    CGWCircle   c
)
{
    int64_t     dist, rem;
    int64_t     xn, yn;
    
    // Project the circle origin onto the boundary of the
    // rectangle:
    xn = r.origin.x + r.size.w;     // rect x_max
    xn = ( c.origin.x < xn ) ? c.origin.x : xn;
    xn = ( r.origin.x < xn ) ? xn : r.origin.x;
    yn = r.origin.y + r.size.h;     // rect y_max
    yn = ( c.origin.y < yn ) ? c.origin.y : yn;
    yn = ( r.origin.y < yn ) ? yn : r.origin.y;
    
    // Distance from the circle center to the projected
    // point on rectangle:
    xn = c.origin.x - xn, yn = c.origin.y - yn;
    dist = CGWSqrtInt64(xn * xn + yn * yn, &rem);
    
    return ((dist < c.radius) || ((dist == c.radius) && (rem == 0)));
}