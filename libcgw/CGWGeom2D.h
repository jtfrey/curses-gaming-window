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
 * Fixup a polar point to be within the standard angle range
 * Polar points can be standardized:  the distance r can be
 * forced to be positive by rotating theta through π radians,
 * and the angle theta can be force into the range [-π, +π).
 * @param p_in      the polar point to fixup
 * @param epsilon   values this close to ±πn will be forced
 *                  to that angle
 * @param p_out     the fixed-up polar point (can be the same
 *                  as pointer \p p_in)
 * @return          the \p p_out pointer or NULL on failure
 */
CGWPolarPoint* CGWPolarPointStandardize(CGWPolarPoint *p_in, float epsilon, CGWPolarPoint *p_out);

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
 * Accumulate a rotation of a 2D transform matrix
 * Rotate the transformation matrix, \p M, by \p theta
 * radians.  This is a shortcut for creating two matrices and
 * multiplying them.
 * @param M     pointer to the matrix to modify
 * @param theta rotation through this many radians
 * @return      the pointer, \p M
 */
CGWXFormMatrix2D* CGWXFormMatrix2DRotate(CGWXFormMatrix2D *M, float theta);

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
CGWXFormMatrix2D* CGWXFormMatrix2DMultiply(CGWXFormMatrix2D *out_M, CGWXFormMatrix2D *X, CGWXFormMatrix2D *Y);

#ifdef HAVE_APPLE_AARCH64_NEON
    //
    // Declare the assembly prototypes and alias to the C function names:
    //
    extern CGWXFormMatrix2D* CGWXFormMatrix2DScaledSumAsm(CGWXFormMatrix2D *out_M, float A,
                                            CGWXFormMatrix2D *X, CGWXFormMatrix2D *Y);
    #define CGWXFormMatrix2DScaledSum CGWXFormMatrix2DScaledSumAsm
    //
    extern CGWPoint2D* CGWXFormMatrix2DDotPoint2DAsm(CGWPoint2D *out_p, CGWXFormMatrix2D *X,
                                            CGWPoint2D *p);
    #define CGWXFormMatrix2DDotPoint2D CGWXFormMatrix2DDotPoint2DAsm
#else

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
    CGWPoint2D* CGWXFormMatrix2DDotPoint2D(CGWPoint2D *out_p, CGWXFormMatrix2D *X, CGWPoint2D *p);

#endif

#endif /* __CGWGEOM2D_H__ */