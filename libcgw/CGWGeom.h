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
extern const CGWPoint CGWPointOrigin;

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
 * Test equality of two points
 * Evaluates to true if \p P1 and \p P2 are equivalent.
 * @param P1        first point
 * @param P2        second point
 * @return          true if the points are equivalent
 */
#define CGWPointEqual(P1, P2) (((P1).x == (P2).x) && ((P1).y == (P2).y))

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
 * Distance between two points
 * Returns the Cartesian distance between \p p1 and \p p2.
 * @param p1        first coordinate
 * @param p2        second coordinate
 * @return          the distance between the coordinates
 */
static inline
double
CGWPointDistance(
    CGWPoint    p1,
    CGWPoint    p2
)
{
    double      dx = p1.x - p2.x,
                dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
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
 * π/180
 * Precalculated constant for π/180
 */
#define M_PI_180    0.01745329251994329576923690768488612713443

/**
 * 180/π
 * Precalculated constant for 180/π
 */
#define M_180_PI    57.29577951308232087679815481410517033241

/**
 * Convert degrees to radians
 * @param degrees   angle in degrees
 * @return          angle in radians
 */
static inline
float
CGWDegreesToRadians(
    float       degrees
)
{
    return M_PI_180 * degrees;
}

/**
 * Convert radians to degrees
 * @param radians   angle in radians
 * @return          angle in degrees
 */
static inline
float
CGWRadiansToDegrees(
    float       radians
)
{
    return M_180_PI * radians;
}

/**
 * A 2D polar point
 * A polar point in 2D is a distance and angle.
 *
 * All functions for this type standardize the angle
 * to lie in the range [-π, π).
 */
typedef struct {
    unsigned int    r;      /*!< the radius (distance) */
    float           theta;  /*!< the angle w.r.t. the +x-axis */
} CGWPointP;

/**
 * The polar origin
 * A constant holding the polar origin, (0,0)
 */
extern const CGWPointP CGWPointPOrigin;

/**
 * Default minimum representable angle w.r.t. the x-axis
 * This value serves as the threshold for snapping polar coordinate
 * to angles of 0, -π.
 */
extern float CGWPointPThetaMinX;

/**
 * Default minimum representable angle w.r.t. the y-axis
 * This value serves as the threshold for snapping polar coordinate
 * to angles of ±π/2.
 */
extern float CGWPointPThetaMinY;

/**
 * Set minimum angle thresholds based on a visible region
 * Given a Cartesian coordinate system with an origin at (0,0)
 * and dimensions \p cartesian_dims, the minimum angles w.r.t.
 * the x- and y-axes are calculated.
 *
 * The angles are chosen such that a line drawn from the origin
 * to the edge of the boundary will shift away from the axis by
 * at least one pixel.
 * @param cartesian_dims        the visible region size
 */
void CGWPointPSetThetaMin(CGWSize cartesian_dims);

/**
 * Calculate minimum angle thresholds based on a visible region
 * Given a Cartesian coordinate system with an origin at (0,0)
 * and dimensions \p cartesian_dims, the minimum angles w.r.t.
 * the x- and y-axes are calculated.
 *
 * The angles are chosen such that a line drawn from the origin
 * to the edge of the boundary will shift away from the axis by
 * at least one pixel.
 * @param cartesian_dims        the visible region size
 * @param theta_min_x           pointer to the variable to set
 *                              to the minimum angle w.r.t. the
 *                              x-axis
 * @param theta_min_y           pointer to the variable to set
 *                              to the minimum angle w.r.t. the
 *                              y-axis
 */
void CGWPointPGetThetaMin(CGWSize cartesian_dims, float *theta_min_x, float *theta_min_y);

#if defined(__APPLE__) && defined(__aarch64__)

    extern double CGWPointPThetaClampWithThresholdsAsm(double theta, double theta_min_x, double theta_min_y);
#   define CGWPointPThetaClampWithThresholds CGWPointPThetaClampWithThresholdsAsm

#else

    /**
     * Map an angle to the range [-π, π)
     * Ensures the angle \p theta is shifted into the range
     * adopted by the \ref CGWPointP type.  The axis thresholds
     * \p theta_min_x and \p theta_min_y are used to "snap" the
     * angle exactly to {0, -π, ±π/2}.
     * @param theta         the angle to cleanup
     * @param theta_min_x   minimum threshold to use w.r.t. x-axis
     * @param theta_min_y   minimum threshold to use w.r.t. y-axis
     * @return              the in-range angle
     */
    double CGWPointPThetaClampWithThresholds(double theta, double theta_min_x, double theta_min_y);

#endif
    ;

/**
 * Map an angle to the range [-π, π) with default thresholds
 * Calls the \ref CGWPointPThetaClampWithThresholds() function with
 * the \ref CGWPointPThetaMinX and \ref CGWPointPThetaMinY constants
 * for the thresholds.
 * @param THETA         the angle to cleanup
 * @return              the in-range angle
 */
#define CGWPointPThetaClamp(THETA) (CGWPointPThetaClampWithThresholds((THETA), \
                                        CGWPointPThetaMinX, CGWPointPThetaMinY))

/**
 * Create a CGWPointP from component values
 * Creates a CGWPointP with the given \p r and \p theta.  The angle
 * \p theta is standardized by calling \ref CGWPointPThetaClampWithThresholds()
 * with the \p theta_min_x and \p theta_min_y arguments.
 * @param r         radius (distance)
 * @param theta     angle w.r.t. the +x-axis
 * @param theta_min_x   minimum threshold to use w.r.t. x-axis
 * @param theta_min_y   minimum threshold to use w.r.t. y-axis
 * @return          a CGWPointP structure containing the values
 */
CGWPointP CGWPointPMakeWithThresholds(unsigned int r, float theta, float theta_min_x, float theta_min_y);

/**
 * Create a CGWPointP from component values
 * A convenience function that calls \ref CGWPointPMakeWithThresholds()
 * with the \ref CGWPointPThetaMinX and \ref CGWPointPThetaMinY constants
 * for the thresholds.
 * @param R     radius (distance)
 * @param THETA angle w.r.t. the +x-axis
 * @return      a CGWPointP structure containing the values
 */
#define CGWPointPMake(R, THETA) (CGWPointPMakeWithThresholds((R), (THETA), \
                                    CGWPointPThetaMinX, CGWPointPThetaMinY))

typedef enum {
    kCGWCoordConvRoundModeTrunc,
    kCGWCoordConvRoundModeRound,
    kCGWCoordConvRoundModeCeil,
    kCGWCoordConvRoundModeFloor
} CGWCoordConvRoundMode;

/**
 * Convert a polar point to its Cartesian counterpart
 * The polar coordinate \p p is converted to the corresponding
 * Cartesian coordinate.  Angle theta=0 corresponds to the
 * positive x-axis and angle theta=π/2 corresponds to the
 * positive y-axis.
 * @param p         the polar coordinate
 * @param rm        round floating-point values to integers
 *                  using this algorithm
 * @return          the corresponding Cartesian coordinate
 */
CGWPoint CGWPointPToCartesian(CGWPointP p, CGWCoordConvRoundMode rm);

/**
 * Convert a Cartesian point to its polar counterpart
 * The Cartesian coordinate \p c is converted to the corresponding
 * polar coordinate.  The positive x-axis corresponds to theta=0
 * and the positive y-axis is corresponds to theta=π/2.
 *
 * Note that the Cartesian origin has no unique representation
 * in a polar coordinate system.  The function returns polar
 * coordinate { r=0, theta=0 } in this case.
 * @param c         the Cartesian coordinate
 * @param rm        round floating-point values to integers
 *                  using this algorithm
 * @return          the corresponding polar coordinate
 */
CGWPointP CGWPointToPolar(CGWPoint c, CGWCoordConvRoundMode rm);

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
 * A 2D point in floating-point
 * A point in a two-dimensional floating-point coordinate system
 */
typedef struct {
    float       x;  /*!< x-coordinate */
    float       y;  /*!< y-coordinate */
} CGWPointReal;

/**
 * The Cartesian origin
 * A constant holding the Cartesian origin, (0.0,0.0)
 */
extern const CGWPointReal CGWPointRealOrigin;

/**
 * Create a CGWPointReal from component values
 * A convenience function that manufactures a CGWPoint
 * structure from component coordinates.
 * @param x     x-coordinate
 * @param y     y-coordinate
 * @return      A CGWPointReal structure containing the values
 */
static inline
CGWPointReal
CGWPointRealMake(
    float       x,
    float       y
)
{
    CGWPointReal    the_point = { .x = x, .y = y };
    return the_point;
}

/**
 * Sum of two CGWPointReal coordinates
 * A convenience function that manufactures a CGWPointReal
 * structure as the sum of the two input points.
 * @param p1    first coordinate
 * @param p2    second coordinate
 * @return      A CGWPointReal structure containing the sum
 */
static inline
CGWPointReal
CGWPointRealSum(
    CGWPointReal    p1,
    CGWPointReal    p2
)
{
    return CGWPointRealMake(p2.x + p1.x, p2.y + p1.y);
}

/**
 * Difference of two CGWPointReal coordinates
 * A convenience function that manufactures a CGWPointReal
 * structure as the difference, \p p2 - \p p1.
 * @param p1    first coordinate
 * @param p2    second coordinate
 * @return      A CGWPointReal structure containing the difference
 */
static inline
CGWPointReal
CGWPointRealDiff(
    CGWPointReal    p1,
    CGWPointReal    p2
)
{
    return CGWPointRealMake(p2.x - p1.x, p2.y - p1.y);
}

/**
 * Distance between two points
 * Returns the Cartesian distance between \p p1 and \p p2.
 * @param p1        first coordinate
 * @param p2        second coordinate
 * @return          the distance between the coordinates
 */
static inline
double
CGWPointRealDistance(
    CGWPointReal    p1,
    CGWPointReal    p2
)
{
    double      dx = p1.x - p2.x,
                dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

/**
 * Convert a polar point to its floating-point Cartesian counterpart
 * The polar coordinate \p p is converted to the corresponding
 * Cartesian coordinate.  Angle theta=0 corresponds to the
 * positive x-axis and angle theta=π/2 corresponds to the
 * positive y-axis.
 * @param p         the polar coordinate
 * @return          the corresponding Cartesian coordinate
 */
static inline
CGWPointReal
CGWPointPToRealCartesian(
    CGWPointP       p
)
{
    CGWPointReal    pi = {
                        .x = p.r * cos(p.theta),
                        .y = p.r * sin(p.theta) };
    return pi;
}

/**
 * Convert a floating-point Cartesian point to its polar counterpart
 * The Cartesian coordinate \p c is converted to the corresponding
 * polar coordinate.  The positive x-axis corresponds to theta=0
 * and the positive y-axis is corresponds to theta=π/2.
 *
 * Note that the Cartesian origin has no unique representation
 * in a polar coordinate system.  The function returns polar
 * coordinate { r=0, theta=0 } in this case.
 * @param c         the Cartesian coordinate
 * @return          the corresponding polar coordinate
 */
CGWPointP CGWPointRealToPolar(CGWPointReal c);

/**
 * Test equality of two points
 * Evaluates to true if \p P1 and \p P2 are close enough.
 * @param P1        first point
 * @param P2        second point
 * @return          true if the points are equivalent
 */
#define CGWPointRealEqual(P1, P2) (CGWPointRealDistance((P1), (P2)) < 1e-6)

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
 * Get the top-left corner of the rectangle
 * The coordinate system's being the screen, the top-left is
 * the origin of rectangle \p r.
 * @param r         a rectangle
 * @return          the top-left coordinate (in screen orientation)
 */
static inline
CGWPoint
CGWRectTopLeft(
    CGWRect     r
)
{
    return r.origin;
}

/**
 * Get the top-right corner of the rectangle
 * The coordinate system's being the screen, the top-right is
 * the origin of rectangle \p r shifted by the width of \p r
 * in the x-direction.
 * @param r         a rectangle
 * @return          the top-right coordinate (in screen orientation)
 */
static inline
CGWPoint
CGWRectTopRight(
    CGWRect     r
)
{
    r.origin.x += r.size.w;
    return r.origin;
}

/**
 * Get the bottom-left corner of the rectangle
 * The coordinate system's being the screen, the bottom-left is
 * the origin of rectangle \p r shifted by the height of \p r
 * in the y-direction.
 * @param r         a rectangle
 * @return          the bottom-left coordinate (in screen orientation)
 */
static inline
CGWPoint
CGWRectBottomLeft(
    CGWRect     r
)
{
    r.origin.y += r.size.h;
    return r.origin;
}

/**
 * Get the bottom-right corner of the rectangle
 * The coordinate system's being the screen, the bottom-right is
 * the origin of rectangle \p r shifted by the width and height
 * of \p r in the x- and y-direction, respectively.
 * @param r         a rectangle
 * @return          the bottom-left coordinate (in screen orientation)
 */
static inline
CGWPoint
CGWRectBottomRight(
    CGWRect     r
)
{
    r.origin.x += r.size.w;
    r.origin.y += r.size.h;
    return r.origin;
}


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
    return ((dx >= 0) && (dx <= r.size.w) && (dy >= 0) && (dy <= r.size.h)) ? true : false;
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

/**
 * Is one rectangle completely contained inside another?
 * Determine whether \p r2 is completely contained inside \p r1.
 * @param r1        does this rectangle contain the other?
 * @param r2        the other rectangle
 * @return          boolean true if \p r2 is completely contained
 *                  within \p r1
 */
static inline
bool
CGWRectContainsRect(
    CGWRect     r1,
    CGWRect     r2
)
{
    // Check that all corners of r2 are within r1:
    if ( ! CGWRectContainsPoint(r1, r2.origin) ) return false;
    if ( ! CGWRectContainsPoint(r1, CGWRectTopRight(r2)) ) return false;
    if ( ! CGWRectContainsPoint(r1, CGWRectBottomRight(r2)) ) return false;
    if ( ! CGWRectContainsPoint(r1, CGWRectBottomLeft(r2)) ) return false;
    return true;
}

/**
 * Is a circle completely contained inside a rectangle?
 * Determine whether \p c is completely contained inside \p r.
 * @param r         does this rectangle contain the circle?
 * @param c         the circle
 * @return          boolean true if \p c is completely contained
 *                  within \p r
 */
static inline
bool
CGWRectContainsCircle(
    CGWRect     r,
    CGWCircle   c
)
{
    int         min, max;
    
    // Origin must be interior to r:
    if ( ! CGWRectContainsPoint(r, c.origin) ) return false;
    // Shift x- and y-extents of r such that c is at origin:
    r.origin.x -= c.origin.x, r.origin.y -= c.origin.y;
    // If any extent of r is now less than the radius, the circle
    // is NOT contained:
    if ( -r.origin.x < c.radius ) return false;
    if ( r.origin.x + r.size.w < c.radius ) return false;
    if ( -r.origin.y < c.radius ) return false;
    if ( r.origin.y + r.size.h < c.radius ) return false;
    return true;
}

/**
 * Is one circle completely contained inside another?
 * Determine whether \p c2 is completely contained inside \p c1.
 * @param r1        does this circle contain the other?
 * @param r2        the other circle
 * @return          boolean true if \p c2 is completely contained
 *                  within \p c1
 */
static inline
bool
CGWCircleContainsCircle(
    CGWCircle   c1,
    CGWCircle   c2
)
{
    int64_t         dist, rem;
    int64_t         dx = c2.origin.x - c1.origin.x,
                    dy = c2.origin.y - c1.origin.y;
    dist = CGWSqrtInt64(dx * dx + dy * dy, &rem);
    if ( (dist + c2.radius < c1.radius) || ((dist + c2.radius == c1.radius) && (rem == 0)) ) return true;
    return false;
}

/**
 * Is a rectangle completely contained inside a circle?
 * Determine whether \p r is completely contained inside \p c.
 * @param r         does this circle contain the rectangle?
 * @param c         the rectangle
 * @return          boolean true if \p r is completely contained
 *                  within \p c
 */
static inline
bool
CGWCircleContainsRect(
    CGWCircle   c,
    CGWRect     r
)
{
    // Check that all corners or r are within c:
    if ( ! CGWCircleContainsPoint(c, r.origin) ) return false;
    if ( ! CGWCircleContainsPoint(c, CGWRectTopRight(r)) ) return false;
    if ( ! CGWCircleContainsPoint(c, CGWRectBottomRight(r)) ) return false;
    if ( ! CGWCircleContainsPoint(c, CGWRectBottomLeft(r)) ) return false;
    return true;
}


#endif /* __CGWGEOM_H__ */