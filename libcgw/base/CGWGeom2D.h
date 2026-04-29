/**
 * @file CGWGeom2D.h
 * @brief Curses Gaming Window, Geometrical support
 * 
 * Types and functions that implement basic geometric utilities
 * in two dimensions.
 *
 */

#ifndef __CGWGEOM2D_H__
#define __CGWGEOM2D_H__

#include "CGWConfig.h"
#include "CGWMath.h"
#include "CGWFixedPrec.h"

/**
 * Standard tolerance for "equal" distances
 */
#define F_DISTANCE_TOL  1e-6

/*
 * Standard tolerance for "equal" angles
 */
#define F_ANGLE_TOL     1e-6

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
 * Rounding mode when converting coordinate types
 * When a floating-point coordinate is converted to
 * an integer format, a constant from this enumeration
 * indicates how to round to the integer value.
 */
typedef enum {
    kCGWCoordConvRoundModeTrunc = 0,    /*!< drop the fractional portion */
    kCGWCoordConvRoundModeRound,        /*!< round up or down based on value relative to halfway
                                             between integer values */
    kCGWCoordConvRoundModeCeil,         /*!< apply the ceil() function */
    kCGWCoordConvRoundModeFloor,        /*!< apply the floor() function */
    kCGWCoordConvRoundModeTowardInf,    /*!< round towards ±∞ */
    kCGWCoordConvRoundModeTowardZero,   /*!< round towards zero */
    kCGWCoordConvRoundModeMax           /*!< the number of modes */
} CGWCoordConvRoundMode;

/**
 * The type of a floating-point rounding function
 * Provides a type for a rounding function that accepts and
 * returns a single-precison floating-point value.
 */
typedef float (*CGWCoordConvRoundingFn)(float v);

/**
 * The array of floating-point rounding functions
 * Client code invokes a rounding function by looking-up
 * the function pointer in this array.
 */
extern const CGWCoordConvRoundingFn *CGWCoordConvRoundingFns;

/**
 * Call a floating-point coordinate conversion rounding function
 * The rounding function associated with mode \p M is called
 * with the value \p V passed to it.
 * @param M         the rounding mode
 * @param V         the value to round
 * @return          the rounded value
 */
#define CGWCoordConvRound(M, V) (CGWCoordConvRoundingFns[(M)]((V)))

/**
 * The type of a fixed-precision rounding function
 * Provides a type for a rounding function that accepts and
 * returns a fixed-precision value.
 */
typedef CGWFixedPrec (*CGWCoordConvFPRoundingFn)(CGWFixedPrec v);

/**
 * The array of fixed-precision rounding functions
 * Client code invokes a rounding function by looking-up
 * the function pointer in this array.
 */
extern const CGWCoordConvFPRoundingFn *CGWCoordConvFPRoundingFns;

/**
 * Call a fixed-precision coordinate conversion rounding function
 * The rounding function associated with mode \p M is called
 * with the value \p V passed to it.
 * @param M         the rounding mode
 * @param V         the value to round
 * @return          the rounded value
 */
#define CGWCoordConvFPRound(M, V) (CGWCoordConvFPRoundingFns[(M)]((V)))


/*
 * A 2D point
 * A point in a two-dimensional coordinate system using
 * floating-point values.
 */
typedef struct {
    float       x;  /*!< the x-coordinate */
    float       y;  /*!< the y-coordinate */
} CGWPoint2D;

/**
 * The Cartesian origin
 * A constant holding the Cartesian origin, (0,0)
 */
extern const CGWPoint2D CGWPoint2DOrigin;

/**
 * Create a CGWPoint from component values
 * A convenience function that manufactures a CGWPoint2D
 * structure from component coordinates.
 * @param x     x-coordinate
 * @param y     y-coordinate
 * @return      A CGWPoint2D structure containing the values
 */
static inline
CGWPoint2D
CGWPoint2DMake(
    float       x,
    float       y
)
{
    CGWPoint2D  the_point = { .x = x, .y = y };
    return the_point;
}

/**
 * Returns the sum of two CGWPoint2D points
 * Sum two CGWPoint2D points.
 * @param p1    A CGWPoint2D, first addend
 * @param p2    A CGWPoint2D, second addend
 * @return      A CGWPoint2D structure containing the sum
 */
static inline
CGWPoint2D
CGWPoint2DSum(
    CGWPoint2D  p1,
    CGWPoint2D  p2
)
{
    CGWPoint2D  the_point = { .x = p1.x + p2.x, .y = p1.y + p2.y };
    return the_point;
}

/**
 * Returns the point times a scalar
 * Scale a CGWPoint2D.
 * @param p     A CGWPoint2D
 * @param s     A scalar
 * @return      A CGWPoint2D structure containing the result
 */
static inline
CGWPoint2D
CGWPoint2DScale(
    CGWPoint2D  p,
    float       s
)
{
    CGWPoint2D  the_point = { .x = s * p.x, .y = s * p.y };
    return the_point;
}

/**
 * Returns the scaled sum of two CGWPoint2D points
 * Scaled sum of two CGWPoint2D points, p1 + s * p2
 * @param p1    A CGWPoint2D, first addend
 * @param s     A scalar
 * @param p2    A CGWPoint2D, second addend
 * @return      A CGWPoint2D structure containing the sum
 */
static inline
CGWPoint2D
CGWPoint2DScaledSum(
    CGWPoint2D  p1,
    float       s,
    CGWPoint2D  p2
)
{
    CGWPoint2D  the_point = { .x = p1.x + s * p2.x, .y = p1.y + s * p2.y };
    return the_point;
}

#ifndef HAVE_APPLE_AARCH64

    /**
     * L1 norm of the 2D point (treated as a vector)
     * Calculates the L1 norm -- sum of absolute values of the
     * components -- of the vector (\\vec{\p p} - \\vec{0}).
     * @param p     A CGWPoint2D to act as the endpoint of
     *              the vector relative to the origin
     * @return      the computed norm
     */
    static inline
    float
    CGWPoint2DNormL1(
        CGWPoint2D  p
    )
    {
        return fabs(p.x) + fabs(p.y);
    }

    /**
     * L2 norm of the 2D point (treated as a vector)
     * Calculates the L2 norm -- Euclidean distance -- of
     * the vector (\\vec{\p p} - \\vec{0}).
     * @param p     A CGWPoint2D to act as the endpoint of
     *              the vector relative to the origin
     * @return      the computed norm
     */
    static inline
    float
    CGWPoint2DNormL2(
        CGWPoint2D  p
    )
    {
        return sqrt(p.x * p.x + p.y * p.y);
    }

    /**
     * L∞ norm of the 2D point (treated as a vector)
     * Calculates the L∞ norm -- largest magnitude element -- of
     * the vector (\\vec{\p p} - \\vec{0}).
     * @param p     A CGWPoint2D to act as the endpoint of
     *              the vector relative to the origin
     * @return      the computed norm
     */
    static inline
    float
    CGWPoint2DNormLInf(
        CGWPoint2D  p
    )
    {
        float       absx = fabs(p.x),
                    absy = fabs(p.y);
        return (absx > absy) ? absx : absy;
    }

#endif

/**
 * Distance between two points
 * Calculates the Euclidean distance between two points.
 * @param p1        first point
 * @param p2        second point
 * @return          the distance
 */
static inline
float
CGWPoint2DDist(
    CGWPoint2D  p1,
    CGWPoint2D  p2
)
{
    float       dx = p2.x - p1.x,
                dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

/**
 * Test equality of two 2D points
 * Evaluates to true if \p P1 and \p P2 are equivalent.
 * @param P1        first point
 * @param P2        second point
 * @return          true if the points are equivalent
 */
#define CGWPoint2DEqual(P1, P2) (CGWPoint2DDist((P1), (P2)) < F_DISTANCE_TOL)


/*
 * A 2D (integral) point
 * A point in a two-dimensional coordinate system using
 * integer values.
 */
typedef struct {
    int         x;  /*!< the x-coordinate */
    int         y;  /*!< the y-coordinate */
} CGWPointI2D;

/**
 * Test equality of two integral 2D points
 * Evaluates to true if \p P1 and \p P2 are equivalent.
 * @param P1        first point
 * @param P2        second point
 * @return          true if the points are equivalent
 */
#define CGWPointI2DEqual(P1, P2) (((P1).x == (P2).x) && ((P1).y == (P2).y))

/**
 * The Cartesian origin
 * A constant holding the Cartesian origin, (0,0)
 */
extern const CGWPointI2D CGWPointI2DOrigin;

/**
 * Create a CGWPointI2D from component values
 * A convenience function that manufactures a CGWPointI2D
 * structure from component coordinates.
 * @param x     x-coordinate
 * @param y     y-coordinate
 * @return      A CGWPointI2D structure containing the values
 */
static inline
CGWPointI2D
CGWPointI2DMake(
    int         x,
    int         y
)
{
    CGWPointI2D the_point = { .x = x, .y = y };
    return the_point;
}

/**
 * Returns the sum of two CGWPointI2D points
 * Sum two CGWPointI2D points.
 * @param p1    A CGWPointI2D, first addend
 * @param p2    A CGWPointI2D, second addend
 * @return      A CGWPointI2D structure containing the sum
 */
static inline
CGWPointI2D
CGWPointI2DSum(
    CGWPointI2D p1,
    CGWPointI2D p2
)
{
    CGWPointI2D the_point = { .x = p1.x + p2.x, .y = p1.y + p2.y };
    return the_point;
}

/**
 * Returns the scaled CGWPointI2D
 * Scaled a CGWPointI2D
 * @param p     A CGWPointI2D, first addend
 * @param s     A scalar
 * @return      A CGWPointI2D structure containing the result
 */
static inline
CGWPointI2D
CGWPointI2DScale(
    CGWPointI2D p,
    int         s
)
{
    CGWPointI2D the_point = { .x = s * p.x, .y = s * p.y };
    return the_point;
}

/**
 * Returns the scaled sum of two CGWPointI2D points
 * Scaled sum of two CGWPointI2D points, p1 + s * p2
 * @param p1    A CGWPointI2D, first addend
 * @param s     A scalar
 * @param p2    A CGWPointI2D, second addend
 * @return      A CGWPointI2D structure containing the sum
 */
static inline
CGWPointI2D
CGWPointI2DScaledSum(
    CGWPointI2D p1,
    int         s,
    CGWPointI2D p2
)
{
    CGWPointI2D the_point = { .x = p1.x + s * p2.x, .y = p1.y + s * p2.y };
    return the_point;
}

/**
 * Convert an integral Cartesian point to a floating-point point
 * A 2D floating-point Cartesian point is returned which correlates
 * with the 2D integral Cartesian point, \p p.
 * @param p             the integral point
 * @return              the floating-point point
 */
static inline
CGWPoint2D
CGWPointI2DToPoint2D(
    CGWPointI2D     p
)
{
    return CGWPoint2DMake((float)p.x, (float)p.y);
}

/**
 * Convert a floating-point Cartesian point to an integer point
 * A 2D integral Cartesian point is returned which correlates
 * with the 2D floating-point Cartesian point, \p p.
 *
 * The coordinates are rounded based upon the selected mode,
 * \p round_mode.
 * @param p             the floating-point point
 * @param round_mode    rounding mode for the coordinates
 * @return              the integral point
 */
static inline
CGWPointI2D
CGWPoint2DToPointI2D(
    CGWPoint2D              p,
    CGWCoordConvRoundMode   round_mode
)
{
    return CGWPointI2DMake(
                    CGWCoordConvRound(round_mode, p.x),
                    CGWCoordConvRound(round_mode, p.y));
}


/*
 * A 2D (fixed-precision) point
 * A point in a two-dimensional coordinate system using
 * fixed-precision values.
 */
typedef struct {
    CGWFixedPrec    x;  /*!< the x-coordinate */
    CGWFixedPrec    y;  /*!< the y-coordinate */
} CGWPointFP2D;

/**
 * The Cartesian origin
 * A constant holding the Cartesian origin, (0,0)
 */
extern const CGWPointFP2D CGWPointFP2DOrigin;

/**
 * Create a CGWPointFP2D from component values
 * A convenience function that manufactures a CGWPointFP2D
 * structure from component coordinates.
 * @param x     x-coordinate
 * @param y     y-coordinate
 * @return      A CGWPointI2D structure containing the values
 */
static inline
CGWPointFP2D
CGWPointFP2DMake(
    CGWFixedPrec    x,
    CGWFixedPrec    y
)
{
    CGWPointFP2D    the_point = { .x = x, .y = y };
    return the_point;
}

/**
 * Returns the sum of two CGWPointFP2D points
 * Sum two CGWPointFP2D points.
 * @param p1    A CGWPointFP2D, first addend
 * @param p2    A CGWPointFP2D, second addend
 * @return      A CGWPointFP2D structure containing the sum
 */
static inline
CGWPointFP2D
CGWPointFP2DSum(
    CGWPointFP2D  p1,
    CGWPointFP2D  p2
)
{
    CGWPointFP2D  the_point = { .x = CGWFixedPrecAdd(p1.x, p2.x),
                                .y = CGWFixedPrecAdd(p1.y, p2.y) };
    return the_point;
}

/**
 * Returns the scaled sum of two CGWPointFP2D points
 * Scaled sum of two CGWPointFP2D points, p1 + s * p2
 * @param p1    A CGWPointFP2D, first addend
 * @param s     A scalar
 * @param p2    A CGWPointFP2D, second addend
 * @return      A CGWPointFP2D structure containing the sum
 */
static inline
CGWPointFP2D
CGWPointFP2DScaledSum(
    CGWPointFP2D    p1,
    CGWFixedPrec    s,
    CGWPointFP2D    p2
)
{
    CGWPointFP2D    the_point = { .x = CGWFixedPrecMul(s, p2.x),
                                  .y = CGWFixedPrecMul(s, p2.y) };
    return CGWPointFP2DSum(p1, the_point);
}

/**
 * Convert a fixed-precision Cartesian point to an integer point
 * A 2D integral Cartesian point is returned which correlates
 * with the 2D fixed-precision Cartesian point, \p p.
 *
 * The coordinates are rounded based upon the selected mode,
 * \p round_mode.
 * @param p             the fixed-precision point
 * @param round_mode    rounding mode for the coordinates
 * @return              the integral point
 */
static inline
CGWPointI2D
CGWPointFP2DToPointI2D(
    CGWPointFP2D            p,
    CGWCoordConvRoundMode   round_mode
)
{
    return CGWPointI2DMake(
                    CGWCoordConvFPRound(round_mode, p.x),
                    CGWCoordConvFPRound(round_mode, p.y));
}

/**
 * Convert a integral Cartesian point to a fixed-precision point
 * A 2D fixed-precision Cartesian point is returned which correlates
 * with the 2D interal Cartesian point, \p p.
 *
 * The coordinates are rounded based upon the selected mode,
 * \p round_mode.
 * @param p             the integral point
 * @param round_mode    rounding mode for the coordinates
 * @return              the fixed-precision point
 */
static inline
CGWPointFP2D
CGWPointI2DToPointFP2D(
    CGWPointI2D             p,
    CGWCoordConvRoundMode   round_mode
)
{
    return CGWPointFP2DMake(
                    CGWFixedPrecCreateWithInt(p.x),
                    CGWFixedPrecCreateWithInt(p.y));
}

/**
 * Convert a fixed-precision Cartesian point to a floating-point point
 * A 2D floating-point Cartesian point is returned which correlates
 * with the 2D fixed-precision Cartesian point, \p p.
 *
 * The coordinates are rounded based upon the selected mode,
 * \p round_mode.
 * @param p             the fixed-precision point
 * @return              the floating-point point
 */
static inline
CGWPoint2D
CGWPointFP2DToPoint2D(
    CGWPointFP2D            p
)
{
    return CGWPoint2DMake(
                    CGWFixedPrecToFloat(p.x),
                    CGWFixedPrecToFloat(p.y));
}

/**
 * Convert a floating-point Cartesian point to a fixed-precision point
 * A 2D fixed-precision Cartesian point is returned which correlates
 * with the 2D floating-point Cartesian point, \p p.
 *
 * @param p             the floating-point point
 * @return              the fixed-precision point
 */
static inline
CGWPointFP2D
CGWPoint2DToPointFP2D(
    CGWPoint2D              p
)
{
    return CGWPointFP2DMake(
                    CGWFixedPrecCreateWithFloat(p.x),
                    CGWFixedPrecCreateWithFloat(p.y));
}


/*
 * A 2D polar point
 * A point in a two-dimensional polar coordiante system
 * using floating-point values.
 */
typedef struct {
    float       r;      /*!< the distance */
    float       theta;  /*!< the angle */
} CGWPolarPoint;

/**
 * The polar origin
 * A constant holding the polar origin, (0,0).
 * Note that the polar origin is not a unique
 * point — (0,𝛉) for any angle 𝛉 is also the
 * origin.
 */
extern const CGWPolarPoint CGWPolarPointOrigin;

/**
 * Create a CGWPolarPoint from component values
 * A convenience function that manufactures a CGWPolarPoint
 * structure from component values.
 * @param r     distance from origin
 * @param theta angle with respect to origin
 * @return      A CGWPolarPoint structure containing the
 *              values
 */
static inline
CGWPolarPoint
CGWPolarPointMake(
    float           r,
    float           theta
)
{
    CGWPolarPoint   the_point = { .r = r, .theta = theta };
    return the_point;
}

/**
 * Returns the sum of two CGWPolarPoint points
 * Sum two CGWPolarPoint points.
 * @param p1    A CGWPolarPoint, first addend
 * @param p2    A CGWPolarPoint, second addend
 * @return      A CGWPolarPoint structure containing the sum
 */ 
CGWPolarPoint CGWPolarPointSum(CGWPolarPoint p1, CGWPolarPoint p2);

#ifdef HAVE_APPLE_AARCH64
    //
    // Declare the assembly prototypes and alias to the C function names:
    //
    extern CGWPolarPoint* CGWPolarPointStandardizeAsm(CGWPolarPoint *out_p, CGWPolarPoint *p,
                                                float epsilon);
    #define CGWPolarPointStandardize CGWPolarPointStandardizeAsm
#else

    /**
     * Fixup a polar point to be within the standard angle range
     * Polar points can be standardized:  the distance r can be
     * forced to be positive by rotating theta through π radians,
     * and the angle theta can be force into the range [-π, +π).
     * @param p_out     the fixed-up polar point (can be the same
     *                  as pointer \p p_in)
     * @param p_in      the polar point to fixup
     * @param epsilon   values this close to ±πn will be forced
     *                  to that angle
     * @return          the \p p_out pointer or NULL on failure
     */
    CGWPolarPoint* CGWPolarPointStandardize(CGWPolarPoint *p_out, CGWPolarPoint *p_in,
                                                float epsilon);

#endif

/**
 * Default minimum representable angle w.r.t. the x-axis
 * This value serves as the threshold for snapping polar coordinate
 * to angles of 0, -π.
 */
extern float CGWPolarPointThetaMinX;

/**
 * Default minimum representable angle w.r.t. the y-axis
 * This value serves as the threshold for snapping polar coordinate
 * to angles of ±π/2.
 */
extern float CGWPolarPointThetaMinY;

/**
 * Calculate minimum angle thresholds based on a visible region
 * Given a Cartesian coordinate system with an origin at (0,0)
 * and dimensions \p w by \p h, the minimum angles w.r.t.
 * the x- and y-axes are calculated.
 *
 * The implied coordinate plane ranges from [-w/2,+w/2] in the
 * x-direction and [-h/2,+h/2] in the y-direction.
 *
 * The angles are chosen such that a line drawn from the origin
 * to the edge of the boundary will shift away from the axis by
 * at least one unit.
 * @param w                     width of the visible region
 * @param h                     height of the visible region
 * @param theta_min_x           pointer to the variable to set
 *                              to the minimum angle w.r.t. the
 *                              x-axis
 * @param theta_min_y           pointer to the variable to set
 *                              to the minimum angle w.r.t. the
 *                              y-axis
 */
void CGWPolarPointCalculateThetaMin(int w, int h, float *theta_min_x, float *theta_min_y);

/**
 * Calculates the minimum angles and sets the global thresholds
 * Calls \ref CGWPolarPointCalculateThetaMin() to calculate the
 * minimum thetas for \p w and \p h and sets \ref CGWPolarPointThetaMinX
 * and \ref CGWPolarPointThetaMinY accordingly.
 *
 * See \ref CGWPolarPointCalculateThetaMin() for more information.
 * @param w                     width of the visible region
 * @param h                     height of the visible region
 */
void CGWPolarPointSetGlobalThetaMin(int w, int h);

#ifdef HAVE_APPLE_AARCH64

    // Declare the assembly prototype and alias the C function name to it:
    extern float CGWPolarPointThetaClampWithThresholdsAsm(float theta, float theta_min_x, float theta_min_y);
#   define CGWPolarPointThetaClampWithThresholds CGWPolarPointThetaClampWithThresholdsAsm

#else

    /**
     * Map an angle to the range [-π, π)
     * Ensures the angle \p theta is shifted into the range
     * adopted by the \ref CGWPolarPoint type.  The axis thresholds
     * \p theta_min_x and \p theta_min_y are used to "snap" the
     * angle exactly to {0, -π, ±π/2}.
     * @param theta         the angle to cleanup
     * @param theta_min_x   minimum threshold to use w.r.t. x-axis
     * @param theta_min_y   minimum threshold to use w.r.t. y-axis
     * @return              the in-range angle
     */
    float CGWPolarPointThetaClampWithThresholds(float theta, float theta_min_x, float theta_min_y);

#endif
    ;

/**
 * Map an angle to the range [-π, π) with default thresholds
 * Calls the \ref CGWPolarPointThetaClampWithThresholds() function with
 * the \ref CGWPolarPointThetaMinX and \ref CGWPolarPointThetaMinY constants
 * for the thresholds.
 * @param THETA         the angle to cleanup
 * @return              the in-range angle
 */
#define CGWPolarPointThetaClamp(THETA) (CGWPolarPointThetaClampWithThresholds((THETA), \
                                        CGWPolarPointThetaMinX, CGWPolarPointThetaMinY))

/**
 * Convert a polar point to a Cartesian point
 * Returns a 2D floating-point Cartesian CGWPoint2D that correlates
 * with the polar point, \p p.
 * @param p         polar point
 * @return          2D floating-point Cartesian point
 */
static inline
CGWPoint2D
CGWPolarPointToPoint2D(
    CGWPolarPoint   p
)
{
    return CGWPoint2DMake(p.r * cos(p.theta), p.r * sin(p.theta));
}

/**
 * Convert a Cartesian point to polar point
 * Returns a polar CGWPolarPoint that correlates with the
 * 2D floating-point Cartesian, \p p.
 * @param p         2D floating-point Cartesian point
 * @return          polar point
 */
static inline
CGWPolarPoint
CGWPoint2DToPolarPoint(
    CGWPoint2D      p
)
{
    return CGWPolarPointMake(sqrt(p.x * p.x + p.y * p.y), atan2(p.y, p.x));
}

/**
 * Convert a polar point to an integral Cartesian point
 * A 2D integral Cartesian point is returned which correlates
 * with the polar point, \p p.
 *
 * The coordinates are rounded based upon the selected mode,
 * \p round_mode.
 * @param p             the polar point
 * @param round_mode    rounding mode for the coordinates
 * @return              the integral point
 */
static inline
CGWPointI2D
CGWPolarPointToPointI2D(
    CGWPolarPoint           p,
    CGWCoordConvRoundMode   round_mode
)
{
    return CGWPointI2DMake(
                    CGWCoordConvRound(round_mode, p.r * cos(p.theta)),
                    CGWCoordConvRound(round_mode, p.r * sin(p.theta)));
}

/**
 * Convert an integral Cartesian point to polar point
 * Returns a polar CGWPolarPoint that correlates with the
 * 2D integral Cartesian point, \p p.
 * @param p         polar point
 * @return          2D integral Cartesian point
 */
CGWPolarPoint CGWPointI2DToPolarPoint(CGWPointI2D p);


/**
 * A 2D matrix (for coordinate transforms)
 * We only need six matrix elements, but for the sake of alignment
 * we'll make it eight.
 */
typedef struct {
    float       M[8];
} CGWXFormMatrix2D;

/**
 * Matrix element accessor
 * References element (\p I, \p J) of matrix \p X.
 *
 * The reference can be used in basic expressions to get or set the
 * value of the element, e.g.
 *
 *     CGWXFormMatrix2D_IJ(X, 0, 1) = 0.5f
 *
 * or
 *
 *     v = CGWXFormMatrix2D_IJ(X, 1, 2)
 *
 * @param X         the matrix
 * @param I         the row index
 * @param J         the column index
 * @return          reference to the element
 */
#define CGWXFormMatrix2D_IJ(X,I,J) ((X)[((I)<<2)+(J)])

/**
 * Set the rotational elements of a 2D transform matrix
 * Upper 2x2 submatrix is initialized with the representation of
 * a rotation of \p theta radians.
 * @param M     pointer to the matrix to modify
 * @param theta rotation though this many radians
 * @return      the pointer, \p M
 */
static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DSetRotation(
    CGWXFormMatrix2D    *M,
    float               theta
)
{
    CGWXFormMatrix2D_IJ(M->M, 0, 0) =   CGWXFormMatrix2D_IJ(M->M, 1, 1) = cos(theta);
    CGWXFormMatrix2D_IJ(M->M, 0, 1) = -(CGWXFormMatrix2D_IJ(M->M, 1, 0) = sin(theta));
    return M;
}

/**
 * Accumulate a rotation of a 2D transform matrix
 * Rotate the transformation matrix, \p M, by \p theta
 * radians.  This is a shortcut for creating two matrices and
 * multiplying them.
 * @param M     pointer to the matrix to modify
 * @param theta rotation through this many radians
 * @return      the pointer, \p M
 */
CGWXFormMatrix2D* CGWXFormMatrix2DRotate(CGWXFormMatrix2D *M, float theta);

#ifndef HAVE_APPLE_AARCH64

    /**
     * Set a 2D transform matrix to all zeroes
     * The entire matrix is initialized to zeroes.
     * @param M     pointer to the matrix to modify
     * @return      the pointer, \p M
     */
    static inline
    CGWXFormMatrix2D*
    CGWXFormMatrix2DSetZero(
        CGWXFormMatrix2D    *M
    )
    {
        memset(M, 0, sizeof(*M));
        return M;
    }

    /**
     * Set a 2D transform matrix to the identity
     * The entire matrix is initialized to zeroes and ones are
     * placed on the diagonal.
     * @param M     pointer to the matrix to modify
     * @return      the pointer, \p M
     */
    static inline
    CGWXFormMatrix2D*
    CGWXFormMatrix2DSetIdentity(
        CGWXFormMatrix2D    *M
    )
    {
        memset(M, 0, sizeof(*M));
        CGWXFormMatrix2D_IJ(M->M, 0, 0) = CGWXFormMatrix2D_IJ(M->M, 1, 1) = 1.0f;
        return M;
    }

    /**
     * Set the translation elements of a 2D transform matrix
     * The right-most elements are set to \p dx in the first row and
     * \p dy in the second row.
     * @param M     pointer to the matrix to modify
     * @param dx    translation in the first coordinate, x
     * @param dy    translation in the second coordinate, y
     * @return      the pointer, \p M
     */
    static inline
    CGWXFormMatrix2D*
    CGWXFormMatrix2DSetTranslation(
        CGWXFormMatrix2D    *M,
        float               dx,
        float               dy
    )
    {
        CGWXFormMatrix2D_IJ(M->M, 0, 2) = dx;
        CGWXFormMatrix2D_IJ(M->M, 1, 2) = dy;
        return M;
    }
    
    /**
     * Accumulate a translation of a 2D transform matrix
     * Translate the transformation matrix, \p M, by \p dx and
     * \p dy.  This is a shortcut for creating two matrices and
     * multiplying them.
     * @param M     pointer to the matrix to modify
     * @param dx    translation in the first coordinate, x
     * @param dy    translation in the second coordinate, y
     * @return      the pointer, \p M
     */
    static inline
    CGWXFormMatrix2D*
    CGWXFormMatrix2DTranslate(
        CGWXFormMatrix2D    *M,
        float               dx,
        float               dy
    )
    {
        CGWXFormMatrix2D_IJ(M->M, 0, 2) += dx;
        CGWXFormMatrix2D_IJ(M->M, 1, 2) += dy;
        return M;
    }

    /**
     * Accumulate a scaling of a 2D transform matrix
     * Scale the transformation matrix, \p M, by \p sx and
     * \p ds.  This is a shortcut for creating two matrices and
     * multiplying them.
     * @param M     pointer to the matrix to modify
     * @param dx    scaling of the first coordinate, x
     * @param dy    scaling of the second coordinate, y
     * @return      the pointer, \p M
     */
    static inline
    CGWXFormMatrix2D*
    CGWXFormMatrix2DScale(
        CGWXFormMatrix2D    *M,
        float               sx,
        float               sy
    )
    {
        CGWXFormMatrix2D_IJ(M->M, 0, 0) *= sx, CGWXFormMatrix2D_IJ(M->M, 0, 1) *= sx, CGWXFormMatrix2D_IJ(M->M, 0, 2) *= sx;
        CGWXFormMatrix2D_IJ(M->M, 1, 0) *= sy, CGWXFormMatrix2D_IJ(M->M, 1, 1) *= sy, CGWXFormMatrix2D_IJ(M->M, 1, 2) *= sy;
        return M;
    }

    /**
     * Generic matrix sum
     * Scale matrix \p X by \p A and add it to matrix \p Y, placing the sum in
     * matrix \p out_M:
     *
     *     out_M_i,j = A * X_i,j + Y_i,j
     *
     * Three values of \p A are treated specially:
     *
     *     - 0.0:  Y is copied to out_M
     *     - 1.0:  out_M_i,j = X_i,j + Y_i,j
     *     - -1.0: out_M_i,j = Y_i,j - X_i,j
     *
     * @param out_M     result matrix
     * @param A         the scalar
     * @param X         first addend matrix
     * @param Y         second addend matrix
     * @return          the pointer out_M
     */
    CGWXFormMatrix2D* CGWXFormMatrix2DScaledSum(CGWXFormMatrix2D *out_M, float A,
                                            CGWXFormMatrix2D *X, CGWXFormMatrix2D *Y);
    /**
     * Transform a 2D floating-point Cartesian point
     * Apply the transform represented by \p X to the point \p p.
     * @param out_p     pointer to the result coordinate
     * @param X         pointer to the transform matrix
     * @param p         pointer to the original point
     * @return          the pointer out_p
     */
    CGWPoint2D* CGWXFormMatrix2DDotPoint2D(CGWPoint2D *out_p, CGWXFormMatrix2D *X,
                                            CGWPoint2D *p);

    /**
     * Generic matrix multiply
     * Matrix \p X multiplies matrix \p Y, with the result placed in
     * matrix \p out_M.
     *
     * If \p out_M is unique from \p X or \p Y, the result is written directly
     * to it; otherwise, a temporary matrix is allocated on the stack and then
     * copied to out_M for return.
     *
     * @param X         left matrix
     * @param Y         right matrix
     * @param out_M      result matrix
     */
    CGWXFormMatrix2D* CGWXFormMatrix2DMultiply(CGWXFormMatrix2D *out_M, CGWXFormMatrix2D *X,
                                            CGWXFormMatrix2D *Y);
    
    /**
     * Invert a matrix
     * The matrix is inverted using the determinant and adjoint.
     * @param A_inv     pointer to a CGWXFormMatrix2D to hold the inverse; can be
     *                  the same as \p A
     * @param A         pointer to the CGWXFormMatrix2D to invert
     * @return          the pointer \p A_inv
     */
    CGWXFormMatrix2D* CGWXFormMatrix2DInvert(CGWXFormMatrix2D *A_inv, CGWXFormMatrix2D *A);

    /**
     * Invert a matrix
     * The matrix is inverted using Gaussian elimination.
     * @param A_inv     pointer to a CGWXFormMatrix2D to hold the inverse; can be
     *                  the same as \p A
     * @param A         pointer to the CGWXFormMatrix2D to invert
     * @return          the pointer \p A_inv
     */

    CGWXFormMatrix2D* CGWXFormMatrix2DInvertGE(CGWXFormMatrix2D *A_inv, CGWXFormMatrix2D *A);
#endif


/**
 * Integral dimensions in 2D
 * A width and height
 */
typedef struct {
    unsigned int    w;      /*!< the x-distance */
    unsigned int    h;      /*!< the y-distance */
} CGWSizeI2D;

/**
 * Create a CGWSizeI2D from component values
 * A convenience function that manufactures a CGWSizeI2D
 * structure from component coordinates.
 * @param w     x-distance
 * @param h     y-distance
 * @return      A CGWSizeI2D structure containing the values
 */
static inline
CGWSizeI2D
CGWSizeI2DMake(
    unsigned int    w,
    unsigned int    h
)
{
    CGWSizeI2D      the_size = { .w = w, .h = h };
    return the_size;
}

/**
 * A 2D rectangle of integral dimensions and position
 * A rectangle consists of a point of origin and distances in
 * the two coordinate directions (size).  The size should be
 * kept positive, with the origin always chosen to fulfill that
 * condition.
 */
typedef struct {
    CGWPointI2D     origin; /*!< the origin of the rectangle */
    CGWSizeI2D      size;   /*!< the width and height */
} CGWRectI2D;

/**
 * Get the top-left corner of the rectangle
 * The coordinate system's being the screen, the top-left is
 * the origin of rectangle \p r.
 * @param r         a rectangle
 * @return          the top-left coordinate (in screen orientation)
 */
static inline
CGWPointI2D
CGWRectI2DTopLeft(
    CGWRectI2D     r
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
CGWPointI2D
CGWRectI2DTopRight(
    CGWRectI2D     r
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
CGWPointI2D
CGWRectI2DBottomLeft(
    CGWRectI2D     r
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
CGWPointI2D
CGWRectI2DBottomRight(
    CGWRectI2D     r
)
{
    r.origin.x += r.size.w;
    r.origin.y += r.size.h;
    return r.origin;
}


/**
 * Create a CGWRectI2D from component values
 * A convenience function that manufactures a CGWRectI2D
 * structure from component coordinates and distances.
 * @param x     x-coordinate of origin
 * @param y     y-coordinate of origin
 * @param w     width in the x-direction
 * @param h     height in the y-direction
 * @return      A CGWRectI2D structure containing the values
 */
static inline
CGWRectI2D
CGWRectI2DMake(
    int             x,
    int             y,
    unsigned int    w,
    unsigned int    h
)
{
    CGWRectI2D     the_rect = { .origin.x = x, .origin.y = y,
                             .size.w = w, .size.h = h };
    return the_rect;
}

/**
 * Create a CGWRectI2D from two points
 * A convenience function that manufactures a CGWRectI2D
 * structure according to the coordinates of two
 * opposing corners of the rectangle.
 * @param p1    First corner
 * @param p2    Opposing corner to p1
 * @return      A CGWRectI2D structure containing the
 *              rectangle's origin and size
 */
static inline
CGWRectI2D
CGWRectI2DMakeWithPoints(
    CGWPointI2D p1,
    CGWPointI2D p2
)
{
    int         dx = p2.x - p1.x;
    int         dy = p2.y - p1.y;
    
    return CGWRectI2DMake(
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
CGWRectI2DContainsPoint(
    CGWRectI2D  r,
    CGWPointI2D p
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
bool CGWRectI2DOverlapsRect(CGWRectI2D r1, CGWRectI2D r2);


/**
 * A circle of integral dimensions
 * A circle consists of an origin point and a radius.  The radius
 * must be positive.
 */
typedef struct {
    CGWPointI2D     origin;     /*!< the point at which the circle is centered */
    unsigned int    radius;     /*!< the radius of the circle */
} CGWCircleI;

/**
 * Create a CGWCircleI from component values
 * A convenience function that manufactures a CGWCircleI
 * structure from component coordinates and radius.
 * @param x     x-coordinate of origin
 * @param y     y-coordinate of origin
 * @param r     radius
 * @return      A CGWCircleI structure containing the values
 */
static inline
CGWCircleI
CGWCircleIMake(
    int             x,
    int             y,
    unsigned int    r
)
{
    CGWCircleI      new_circle = {
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
CGWCircleIContainsPoint(
    CGWCircleI      c,
    CGWPointI2D     p
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
CGWCircleIOverlapsCircle(
    CGWCircleI      c1,
    CGWCircleI      c2
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
bool CGWRectI2DOverlapsCircle(CGWRectI2D r, CGWCircleI c);

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
#define CGWCircleIOverlapsRect(C, R) CGWRectI2DOverlapsCircle((R), (C))

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
CGWRectI2DContainsRect(
    CGWRectI2D  r1,
    CGWRectI2D  r2
)
{
    // Check that all corners of r2 are within r1:
    if ( ! CGWRectI2DContainsPoint(r1, r2.origin) ) return false;
    if ( ! CGWRectI2DContainsPoint(r1, CGWRectI2DTopRight(r2)) ) return false;
    if ( ! CGWRectI2DContainsPoint(r1, CGWRectI2DBottomRight(r2)) ) return false;
    if ( ! CGWRectI2DContainsPoint(r1, CGWRectI2DBottomLeft(r2)) ) return false;
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
CGWRectI2DContainsCircle(
    CGWRectI2D  r,
    CGWCircleI  c
)
{
    // Origin must be interior to r:
    if ( ! CGWRectI2DContainsPoint(r, c.origin) ) return false;
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
CGWCircleIContainsCircle(
    CGWCircleI      c1,
    CGWCircleI      c2
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
CGWCircleIContainsRect(
    CGWCircleI  c,
    CGWRectI2D  r
)
{
    // Check that all corners or r are within c:
    if ( ! CGWCircleIContainsPoint(c, r.origin) ) return false;
    if ( ! CGWCircleIContainsPoint(c, CGWRectI2DTopRight(r)) ) return false;
    if ( ! CGWCircleIContainsPoint(c, CGWRectI2DBottomRight(r)) ) return false;
    if ( ! CGWCircleIContainsPoint(c, CGWRectI2DBottomLeft(r)) ) return false;
    return true;
}





/**
 * Dimensions in 2D
 * A width and height
 */
typedef struct {
    float       w;      /*!< the x-distance */
    float       h;      /*!< the y-distance */
} CGWSize2D;

/**
 * Create a CGWSize2D from component values
 * A convenience function that manufactures a CGWSize2D
 * structure from component coordinates.
 * @param w     x-distance
 * @param h     y-distance
 * @return      A CGWSize2D structure containing the values
 */
static inline
CGWSize2D
CCGWSize2DMake(
    float           w,
    float           h
)
{
    CGWSize2D       the_size = { .w = fabs(w), .h = fabs(h) };
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
    CGWPoint2D      origin; /*!< the origin of the rectangle */
    CGWSize2D       size;   /*!< the width and height */
} CGWRect2D;

/**
 * Get the top-left corner of the rectangle
 * The coordinate system's being the screen, the top-left is
 * the origin of rectangle \p r.
 * @param r         a rectangle
 * @return          the top-left coordinate (in screen orientation)
 */
static inline
CGWPoint2D
CGWRect2DTopLeft(
    CGWRect2D      r
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
CGWPoint2D
CGWRect2DTopRight(
    CGWRect2D      r
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
CGWPoint2D
CGWRect2DBottomLeft(
    CGWRect2D      r
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
CGWPoint2D
CGWRect2DBottomRight(
    CGWRect2D      r
)
{
    r.origin.x += r.size.w;
    r.origin.y += r.size.h;
    return r.origin;
}


/**
 * Create a CGWRect2D from component values
 * A convenience function that manufactures a CGWRect2D
 * structure from component coordinates and distances.
 * @param x     x-coordinate of origin
 * @param y     y-coordinate of origin
 * @param w     width in the x-direction
 * @param h     height in the y-direction
 * @return      A CGWRect2D structure containing the values
 */
static inline
CGWRect2D
CGWRect2DMake(
    float       x,
    float       y,
    float       w,
    float       h
)
{
    CGWRect2D   the_rect = { .origin.x = x, .origin.y = y,
                             .size.w = fabs(w), .size.h = fabs(h) };
    return the_rect;
}

/**
 * Create a CGWRect2D from two points
 * A convenience function that manufactures a CGWRect2D
 * structure according to the coordinates of two
 * opposing corners of the rectangle.
 * @param p1    First corner
 * @param p2    Opposing corner to p1
 * @return      A CGWRect2D structure containing the
 *              rectangle's origin and size
 */
static inline
CGWRect2D
CGWRect2DMakeWithPoints(
    CGWPoint2D  p1,
    CGWPoint2D  p2
)
{
    float       dx = p2.x - p1.x;
    float       dy = p2.y - p1.y;
    
    return CGWRect2DMake(
        ( dx < 0.0f ) ? p2.x : p1.x,
        ( dy < 0.0f ) ? p2.y : p1.y,
        ( dx < 0.0f ) ? -dx : dx,
        ( dy < 0.0f ) ? -dy : dy);
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
CGWRect2DContainsPoint(
    CGWRect2D   r,
    CGWPoint2D  p
)
{
    float       dx = p.x - r.origin.x,
                dy = p.y - r.origin.y;
    return ((dx >= 0.0f) && (dx <= r.size.w) && (dy >= 0.0f) && (dy <= r.size.h)) ? true : false;
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
bool CGWRect2DOverlapsRect(CGWRect2D r1, CGWRect2D r2);




/**
 * A circle
 * A circle consists of an origin point and a radius.  The radius
 * must be positive.
 */
typedef struct {
    CGWPoint2D      origin;     /*!< the point at which the circle is centered */
    float           radius;     /*!< the radius of the circle */
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
    float           x,
    float           y,
    float           r
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
    CGWPoint2D      p
)
{
    float           dx = c.origin.x - p.x,
                    dy = c.origin.y - p.y;
    float           dist = sqrt(dx * dx + dy * dy);
    
    return ( dist <= c.radius );
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
    float           dx = c2.origin.x - c1.origin.x,
                    dy = c2.origin.y - c1.origin.y;
    float           dist = sqrt(dx * dx + dy * dy);
    
    return (dist <= c1.radius + c2.radius);
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
bool CGWRect2DOverlapsCircle(CGWRect2D r, CGWCircle c);

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
#define CGWCircleOverlapsRect(C, R) CGWRect2DOverlapsCircle((R), (C))

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
CGWRect2DContainsRect(
    CGWRect2D   r1,
    CGWRect2D   r2
)
{
    // Check that all corners of r2 are within r1:
    if ( ! CGWRect2DContainsPoint(r1, r2.origin) ) return false;
    if ( ! CGWRect2DContainsPoint(r1, CGWRect2DTopRight(r2)) ) return false;
    if ( ! CGWRect2DContainsPoint(r1, CGWRect2DBottomRight(r2)) ) return false;
    if ( ! CGWRect2DContainsPoint(r1, CGWRect2DBottomLeft(r2)) ) return false;
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
CGWRect2DContainsCircle(
    CGWRect2D   r,
    CGWCircle   c
)
{
    // Origin must be interior to r:
    if ( ! CGWRect2DContainsPoint(r, c.origin) ) return false;
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
    CGWCircle       c1,
    CGWCircle       c2
)
{
    float           dx = c2.origin.x - c1.origin.x,
                    dy = c2.origin.y - c1.origin.y;
    float           dist = sqrt(dx * dx + dy * dy);
    
    if ( dist + c2.radius <= c1.radius  ) return true;
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
    CGWRect2D   r
)
{
    // Check that all corners or r are within c:
    if ( ! CGWCircleContainsPoint(c, r.origin) ) return false;
    if ( ! CGWCircleContainsPoint(c, CGWRect2DTopRight(r)) ) return false;
    if ( ! CGWCircleContainsPoint(c, CGWRect2DBottomRight(r)) ) return false;
    if ( ! CGWCircleContainsPoint(c, CGWRect2DBottomLeft(r)) ) return false;
    return true;
}

#ifdef HAVE_APPLE_AARCH64_NEON
#   include "CGWGeom2D.asm/apple-aarch64.c"
#endif

#endif /* __CGWGEOM2D_H__ */