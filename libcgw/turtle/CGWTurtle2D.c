/**
 * @file CGWTurtle2D.c
 * @brief Curses Gaming Window, Turtle graphics
 * 
 * Source to accompany the header.
 *
 */

#include "CGWTurtle2D.h"
#include "CGWTiming.h"

//

#ifdef CGW_TURTLE_DEBUG
static
FILE*
__CGWTurtleDebugFPtr(void)
{
    static FILE *__debug_file = NULL;
    if ( __debug_file == NULL ) __debug_file = fopen("turtle.debug", "a");
    return __debug_file;
}
#   define CGWTDBG(...) fprintf(__CGWTurtleDebugFPtr(), ##__VA_ARGS__)
#else
#   define CGWTDBG(...)
#endif

#ifndef CGW_TURTLE_NAME_LENGTH_MAX
#   define CGW_TURTLE_NAME_LENGTH_MAX 32
#endif

//

static CGWRect2D    CGWTurtleRegion = { .origin = { .x=0.0f, .y=0.0f },
                                        .size = { .w=1.0f, .h=1.0f } };

//

typedef struct {
    float           x_one, x_one_plus, x_zero, x_zero_minus;
    float           y_one, y_one_plus, y_zero, y_zero_minus;
} CGWTurtleEffectiveRanges;

//

/*
 * Private bits in the options/flags bit vector:
 */
enum {
    kCGWTurtleFlagsStylusIsOn                   = 16,
    kCGWTurtleFlagsHasDrawingDelay              = 17,
    
    kCGWTurtleFlagsStylusIsOnMask               = CGWBitvectorMask(kCGWTurtleFlagsStylusIsOn),
    kCGWTurtleFlagsHasDrawingDelayMask          = CGWBitvectorMask(kCGWTurtleFlagsHasDrawingDelay),
    
    kCGWTurtleFlagsMask                         = kCGWTurtleFlagsStylusIsOnMask
                                                    | kCGWTurtleFlagsHasDrawingDelayMask
};

//

#define CGWTurtleShouldLog(BV) (CGWBitvectorGetBitIndex((BV), kGGWTurtleOptionsEnableLogging))

#define CGWTurtleBoundsType(BV, AXIS) \
            (CGWBitvectorGetBits((BV), kCGWTurtleOptions ## AXIS ## BoundsMask))

#define CGWTurtleBoundsIsPeriodic(BV, AXIS) \
            ((CGWBitvectorGetBits((BV), kCGWTurtleOptions ## AXIS ## BoundsMask) == kCGWTurtleOptions ## AXIS ## BoundsPeriodic) ? true : false)

//

typedef enum {
    kCGWTurtleEdgeDetectNone = 0,
    
    kCGWTurtleEdgeDetectLeft = 0b001,
    kCGWTurtleEdgeDetectRight = 0b011,
    
    kCGWTurtleEdgeDetectTop = 0b101,
    kCGWTurtleEdgeDetectBottom = 0b111,
    
    kCGWTurtleEdgeDetectTypeMask = 0b100,
    kCGWTurtleEdgeDetectDirectionMask = 0b010
} CGWTurtleEdgeDetect;

#define CGWTurtleEdgeDetectIsLeftRight(X) (((X) & kCGWTurtleEdgeDetectTypeMask) == 0)
#define CGWTurtleEdgeDetectIsTopBottom(X) (((X) & kCGWTurtleEdgeDetectTypeMask) != 0)

//

static
CGWPoint2D
__CGWTurtleClampPointToBoundsEdges(
    CGWPoint2D                  *p1,
    CGWPoint2D                  *p2,
    CGWTurtleEffectiveRanges    *eranges,
    CGWTurtleEdgeDetect         *edge_detected
)
{   
    CGWPoint2D          p2_prime;
    float               m;
    
    // Pre-condition:  p1 is interior to bounds, p1 != p2
    *edge_detected = kCGWTurtleEdgeDetectNone;
    
    // Is p2 interior to the bounds?
    if ( CGWRect2DContainsPoint(CGWTurtleRegion, *p2) ) return *p2;
    
    // If the line connecting p1 to p2 is vertical,
    // we need only test the top/bottom edges:
    if ( fabs(p1->x - p2->x) < eranges->x_zero ) {
        p2_prime.x = p2->x;
        if ( p2->y > eranges->y_one ) {
            p2_prime.y = 1.0f;
            *edge_detected = kCGWTurtleEdgeDetectTop;
        }
        else if ( p2->y < eranges->y_zero ) {
            p2_prime.y = 0.0f;
            *edge_detected = kCGWTurtleEdgeDetectBottom;
        }
        return p2_prime;
    }
    
    // If the line connecting p1 to p2 is horizontal,
    // we need only test the left/right edges:
    if ( fabs(p1->y - p2->y) < eranges->y_zero ) {
        p2_prime.y = p2->y;
        if ( p2->x > eranges->x_one ) {
            p2_prime.x = 1.0f;
            *edge_detected = kCGWTurtleEdgeDetectRight;
        }
        else if ( p2->x < eranges->x_zero ) {
            p2_prime.x = 0.0f;
            *edge_detected = kCGWTurtleEdgeDetectLeft;
        }
        return p2_prime;
    }
    
    // Calculate the slope:
    m = (p2->y - p1->y) / (p2->x - p1->x);
    
    // Does p2 lie beyond the right edge?
    if ( p2->x > eranges->x_one ) {
        //
        // Solve y = m(1 - p1->x) + p1->y
        //
        float   y = m * (1.0f - p1->x) + p1->y;
        
        // If y is between the top/bottom bounds, we're done:
        if ( (y > eranges->y_zero_minus) && (y < eranges->y_one_plus) ) {
            p2_prime.x = 1.0f;
            p2_prime.y = (y < eranges->y_zero) ? 0.0f : ((y > eranges->y_one) ? 1.0f : y);
            *edge_detected = kCGWTurtleEdgeDetectRight;
            return p2_prime;
        }
        // y is outside the top/bottom limit, so the line intersects
        // the top/bottom edge and not the right edge
    }
    // Does p2 lie beyond the left edge?
    else if ( p2->x < eranges->x_zero ) {
        //
        // Solve y = m(0 - p1->x) + p1->y
        //
        float   y = -m * p1->x + p1->y;
        
        // If y is between the top/bottom bounds, we're done:
        if ( (y > eranges->y_zero_minus) && (y < eranges->y_one_plus) ) {
            p2_prime.x = 0.0f;
            p2_prime.y = (y < eranges->y_zero) ? 0.0f : ((y > eranges->y_one) ? 1.0f : y);
            *edge_detected = kCGWTurtleEdgeDetectLeft;
            return p2_prime;
        }
        // y is outside the top/bottom limit, so the line intersects
        // the top/bottom edge and not the right edge
    }
    
    // We need the inverted slope for the rest of the calculations:
    m = 1.0 / m;
    
    // Does p2 lie beyond the top edge?
    if ( p2->y > eranges->y_one ) {
        //
        // Solve x = (1/m)(1 - p1->y) + p1->x
        //
        float   x = m * (1.0f - p1->y) + p1->x;
        
        // If x is between the left/right bounds, we're done:
        if ( (x > eranges->x_zero_minus) && (x < eranges->x_one_plus) ) {
            p2_prime.y = 1.0f;
            p2_prime.x = (x < eranges->x_zero) ? 0.0f : ((x > eranges->x_one) ? 1.0f : x);
            *edge_detected = kCGWTurtleEdgeDetectTop;
            return p2_prime;
        }
        // x is outside the left/right limit, so the line intersects...
        // what?
    }
    // Does p2 lie beyond the bottom edge?
    if ( p2->y < eranges->y_zero ) {
        //
        // Solve x = (1/m)(0 - p1->y) + p1->x
        //
        float   x = -m * p1->y + p1->x;
        
        // If x is between the left/right bounds, we're done:
        if ( (x > eranges->x_zero_minus) && (x < eranges->x_one_plus) ) {
            p2_prime.y = 0.0f;
            p2_prime.x = (x < eranges->x_zero) ? 0.0f : ((x > eranges->x_one) ? 1.0f : x);
            *edge_detected = kCGWTurtleEdgeDetectBottom;
            return p2_prime;
        }
        // x is outside the left/right limit, so the line intersects...
        // what?
    }
    fprintf(stderr, "This is embarrassing...\n");
    exit(-1);
}

//

/**
 * A turtle graphics context
 */
typedef struct CGWTurtle {
    char                        name[CGW_TURTLE_NAME_LENGTH_MAX]; /*!< the name of this turtle */
    CGWBitvector                flags;                      /*!< behavioral-control flags */
    
    /*
     * The turtle view is a Cartesian coordinate system
     * running from [0,1] on each axis:
     *
     *   y ^
     *   1 |_______
     *     |       |
     *     |       |
     *   0 |_______|_\ x
     *      0      1 /
     *
     * This region maps to a screen coordinate system by
     * means of a transformation — rotation, translation, and
     * independent scaling.  For a screen with the origin fixed
     * at the top-left, for example:
     *     
     *             _____________________\
     *    (x0,y0) |                 |   /  x
     *            |                 |
     *            |                 |
     *            |                 |
     *            |_________________| (x0+w,y0+h)
     *            |
     *         y  V 
     *
     * The transformation involves translations, a scaled-reflection
     * of the y-coordinate, and a scaling of the x-coordinate.  The
     * transform matrix (turtle view to screen coordinates) for
     * this is a reflection through the x-axis, a translation,
     * a scaling, and a final translation:
     *
     *         A            B           C           D
     *    [[1  0  x0]  [[w  0  0]  [[1  0  0]  [[1  0  0]
     *     [0  1  y0] • [0  h  0] • [0  1  1] • [0 -1  0] 
     *     [0  0  1 ]   [0  0  1]]  [0  0  1]]  [0  0  1]]
     *
     *       [[w  0  x0  ]  [[x_t]    [[wx_t + x0     ]
     *     =  [0 -h  h+y0] • [y_t]  =  [-hy_t + h + y0]
     *        [0  0  1   ]]  [1  ]]    [1             ]]:
     *
     *     y_s = -h * (y_t - 1) + y0
     *     x_s = w * x_t + x0
     * 
     * Going the other direction:
     *
     *         D^-1        C^-1          B^-1          A^-1
     *    [[1  0  0]  [[1  0  0]  [[1/w  0    0]  [[1  0  -x0]
     *     [0 -1  0] • [0  1 -1] • [0    1/h  0] • [0  1  -y0]
     *     [0  0  1]]  [0  0  1]]  [0    0    1]]  [0  0   1 ]]
     *
     *       [[1/w  0   -x0/w    ]  [[x_s]    [[(1/w)x_s - x0/w     ]
     *     =  [0   -1/h -y0/h + 1] • [y_s]  =  [(-1/h)y_s - y0/h + 1]
     *        [0    0    1       ]]  [1  ]]    [1                   ]]
     *
     *     y_t = (-1/h)(y_s + y0) + 1
     *     x_t = (1/w)(x_s - x0)
     *
     * The CGWXformMatrix2D generic transformation includes all six
     * matrix elements from rows 1 and 2; but as our coordinate
     * systems are not rotated arbitrarily, two of the matrix
     * elements will always be zero.  Using the explicit equations
     * for transformations omits the unnecessary 1/3 of the FLOPs.
     *
     * For the sake of drawing, it is important to know
     * what ∆x and ∆y in the turtle coordinate system
     * equates with a single-pixel ∆x and ∆y in the
     * screen coordinate system:
     *
     *     x_t,1 - x_t,0 = ±|(1/w)(x_s,1 - x0) - (1/w)(x_s,0 - x0)|
     *     ∆x_t = ±(1/w)|∆x_s|
     *     ∆x_t,pixel = ±(1/w)
     *
     *     y_t,1 - y_t,0 = ±|[(-1/h)(y_s,1 + y0) + 1] - [(-1/h)(y_s,0 + y0) + 1]|
     *     ∆y_t = ±(1/h)|∆y_s|
     *     ∆y_t,pixel = ±(1/h)
     *
     * When drawing a line the independent variable chosen
     * will be the axis on which the largest distance is traversed.
     * For example, a horizontal line would choose x while a
     * vertical line would choose y.
     * 
     */
    struct {
        CGWRectI2D              screen_region;  /*!< the screen coordinate region */
                                                /*   the turtle coordinate region is implicitly
                                                     {o={0.0, 0.0}, s={1.0, 1.0}} */
        CGWPoint2D              resolution;     /*!< the turtle distances that equate with one
                                                     pixel in screen coordinates; these are also the
                                                     multipliers for converting screen to turtle
                                                     coordinates  */
        CGWTurtleEffectiveRanges ranges;        /*!< the effective range of turtle coordinates
                                                     inside the bounds given the resolution */
    } coordinates;
    
    struct {
        CGWPoint2D              turtle;         /*!< the position of the turtle in turtle
                                                     coordinates */
        CGWPointI2D             screen;         /*!< the last-occupied position of the turtle in
                                                     screen coordinates */
    } position;                                 /*!< position of the turtle */
    
    struct {
        CGWPoint2D              vector;         /*!< direction the turtle will move as a normalized
                                                     vector in turtle-coordinate space */
        float                   angle;          /*!< direction the turtle will move as an angle
                                                     (in radians) w.r.t. the +x-axis */
    } orientation;                              /*!< orientation of the turtle */
    
    CGWTimingInterval           drawing_timer;  /*!< time to sleep between drawn pixels */
    
    struct {
        CGWTurtleEventObserver  observer;       /*!< event observer callback */
        const void              *context;       /*!< event observer user-provided context */
    } event_observers[kCGWTurtleEventIdMax];    /*!< the array of event callbacks */
} CGWTurtle;

//

static inline
void
__CGWTurtleCalculateResolutionAndRanges(
    CGWTurtle       *Yertle
)
{
    float           res_x = 1.0f / (float)Yertle->coordinates.screen_region.size.w;
    float           res_y = 1.0f / (float)Yertle->coordinates.screen_region.size.h;
    
    Yertle->coordinates.resolution.x = res_x;
    Yertle->coordinates.resolution.y = res_y;
    
    res_x *= 0.5f;
    res_y *= 0.5f;
    Yertle->coordinates.ranges.x_zero = res_x;
    Yertle->coordinates.ranges.y_zero = res_y;
    Yertle->coordinates.ranges.x_zero_minus = -res_x;
    Yertle->coordinates.ranges.y_zero_minus = -res_y;
    Yertle->coordinates.ranges.x_one = 1.0f - res_x;
    Yertle->coordinates.ranges.y_one = 1.0f - res_y;
    Yertle->coordinates.ranges.x_one_plus = 1.0f + res_x;
    Yertle->coordinates.ranges.y_one_plus = 1.0f + res_y;
}

//

static inline
void
__CGWTurtleCalculateOrientationVector(
    CGWTurtle       *Yertle
)
{
    Yertle->orientation.vector.x = cos(Yertle->orientation.angle);
    Yertle->orientation.vector.y = sin(Yertle->orientation.angle);
}

//

static inline
CGWPointI2D
__CGWTurtleScreenCoordinateApplyPeriodicity(
    CGWRectI2D      bounds,
    CGWPointI2D     p
)
{
    CGWPointI2D     p_out;
    int             c;
    
    c = p.x - bounds.origin.x;
    if ( c < 0 )
        p_out.x = bounds.origin.x + bounds.size.w + c;
    else if ( c > bounds.size.w )
        p_out.x = bounds.origin.x + (c - bounds.size.w);
    else
        p_out.x = p.x;
    c = p.y - bounds.origin.y;
    if ( c < 0 )
        p_out.y = bounds.origin.y + bounds.size.h + c;
    else if ( c > bounds.size.h )
        p_out.y = bounds.origin.y + (c - bounds.size.h);
    else
        p_out.y = p.y;
    return p_out;
}

//

static inline
CGWPoint2D
__CGWTurtleCoordinateScreenToTurtle(
    CGWTurtle       *Yertle,
    CGWPointI2D     s
)
{
    CGWPoint2D      t = {
        .x = Yertle->coordinates.resolution.x * (float)(s.x - Yertle->coordinates.screen_region.origin.x),
        .y = 1.0f - Yertle->coordinates.resolution.y * (float)(s.y + Yertle->coordinates.screen_region.origin.y)
    };
    return t;
}

//

static inline
CGWPointI2D
__CGWTurtleCoordinateTurtleToScreen(
    CGWTurtle       *Yertle,
    CGWPoint2D      t
)
{
    CGWPointI2D     s = {
        .x = roundf((float)Yertle->coordinates.screen_region.size.w * (float)t.x + \
                                        (float)Yertle->coordinates.screen_region.origin.x),
        .y = roundf((float)Yertle->coordinates.screen_region.size.h * (1.0f - (float)t.y) + \
                                        (float)Yertle->coordinates.screen_region.origin.y)
    };
    return s;
}

//

float
__CGWTurtleStandardizeAngle(
    float       angle
)
{
    if ( angle > M_PI ) {
        angle -= M_2xPI * floor(angle / M_2xPI);
        if ( fabs(angle - M_PI) < 1e-6 ) angle = -M_PI;
    }
    else if ( angle < -M_PI ) {
        angle += M_2xPI * ceil(angle / M_2xPI);
    }
    return angle;
}

//

float
__CGWTurtleAngleForPoints(
    CGWPoint2D      p1,
    CGWPoint2D      p2
)
{
    float           dx = (p2.x - p1.x),
                    dy = (p2.y - p1.y);
    float           theta = atan2(dy, dx);
    
    if ( fabs(theta - M_PI) < 1e-6 ) return -M_PI;
    return theta;
}

//

typedef void (*CGWTurtleDrawPointFn)(CGWTurtle*, CGWPointI2D);

static
void
__CGWTurtleDrawPoint(
    CGWTurtle       *Yertle,
    CGWPointI2D     s
)
{
    if ( ! CGWPointI2DEqual(Yertle->position.screen, s) ) {
        if ( CGWBitvectorGetBitIndex(Yertle->flags, kCGWTurtleFlagsHasDrawingDelay) ) {
            CGWTimingIntervalSleep(&Yertle->drawing_timer);
        }
        Yertle->position.screen = s;
        CGWTDBG("%s(%d, %d)\n", (Yertle->flags & kCGWTurtleFlagsStylusIsOn) ? "DRAW" : "SKIP", s.x, s.y);
    }
}

static
void
__CGWTurtleDrawPointAndSendEvent(
    CGWTurtle       *Yertle,
    CGWPointI2D     s
)
{
    if ( ! CGWPointI2DEqual(Yertle->position.screen, s) ) {
        if ( CGWBitvectorGetBitIndex(Yertle->flags, kCGWTurtleFlagsHasDrawingDelay) ) {
            CGWTimingIntervalSleep(&Yertle->drawing_timer);
        }
        Yertle->position.screen = s;
        CGWTDBG("%s(%d, %d)\n", (Yertle->flags & kCGWTurtleFlagsStylusIsOn) ? "DRAW" : "SKIP", s.x, s.y);
        if ( CGWBitvectorGetBitIndex(Yertle->flags, kCGWTurtleFlagsStylusIsOn) ) {
            CGWTurtleEventDrawPixel event = {
                    .base.from_turtle = Yertle,
                    .base.event_id = kCGWTurtleEventDrawPixel,
                    .position = __CGWTurtleScreenCoordinateApplyPeriodicity(Yertle->coordinates.screen_region, s)
                };
            Yertle->event_observers[kCGWTurtleEventDrawPixel].observer((CGWTurtleEvent*)&event,
                                        Yertle->event_observers[kCGWTurtleEventDrawPixel].context);
        }
    }
}

//

static
CGWPoint2D
__CGWTurtleEmitLine(
    CGWTurtle               *Yertle,
    CGWPoint2D              p0,
    CGWPoint2D              p1
)
{
    CGWTurtleDrawPointFn    draw_fn;
    float                   dx = p1.x - p0.x,
                            dy = p1.y - p0.y;
    
    draw_fn = Yertle->event_observers[kCGWTurtleEventDrawPixel].observer ? __CGWTurtleDrawPointAndSendEvent : __CGWTurtleDrawPoint;
    
    CGWTDBG("EMIT LINE (%g, %g) -> (%g, %g)\n", p0.x, p0.y, p1.x, p1.y);
    
    // If p0 hasn't yet been drawn, better do so now:
    draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
    
    // If dy = 0, it's a horizontal line and we just move through
    // each coordinate p0.x through p1.x:
    if ( fabs(dy) < Yertle->coordinates.resolution.y ) {
        if ( dx < 0.0f ) {
            p0.x -= Yertle->coordinates.resolution.x;
            while ( p0.x >= p1.x ) {
                CGWTDBG("[HX-] %g >= %g :: <%g, %g>  ", p1.x, p0.x, p0.x, p0.y);
                draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                p0.x -= Yertle->coordinates.resolution.x;
            }
        } else {
            p0.x += Yertle->coordinates.resolution.x;
            while ( p1.x >= p0.x ) {
                CGWTDBG("[HX+] %g >= %g :: <%g, %g>  ", p1.x, p0.x, p0.x, p0.y);
                draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                p0.x += Yertle->coordinates.resolution.x;
            }
        }
    }
    // If dx = 0, it's a vertical line and we just move through
    // each coordinate p0.y through p1.y:
    else if ( fabs(dx) < Yertle->coordinates.resolution.x ) {
        if ( dy < 0.0f ) {
            p0.y -= Yertle->coordinates.resolution.y;
            while ( p0.y >= p1.y ) {
                CGWTDBG("[VY-] %g >= %g :: <%g, %g>  ", p1.y, p0.y, p0.x, p0.y);
                draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                p0.y -= Yertle->coordinates.resolution.y;
            }
        } else {
            p0.y += Yertle->coordinates.resolution.y;
            while ( p1.y >= p0.y ) {
                CGWTDBG("[VY+] %g >= %g :: <%g, %g>  ", p1.y, p0.y, p0.x, p0.y);
                draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                p0.y += Yertle->coordinates.resolution.y;
            }
        }
    }
    else {
        // The turtle needs to move dx pixels horizontally and dy pixels
        // vertically.  The one with the smaller absolute value will be
        // placed in the numerator of the slope and the x- or y-direction
        // traversed in relation to that slope.
        if ( fabs(dx) >= fabs(dy) ) {
            // Y is smaller:
            float   ddy = Yertle->coordinates.resolution.x * (dy / dx);
            if ( dx < 0 ) {
                p0.x -= Yertle->coordinates.resolution.x;
                p0.y -= ddy;
                 while ( p0.x >= p1.x ) {
                    CGWTDBG("[ X-] %g >= %g :: <%g, %g>  ", p1.x, p0.x, p0.x, p0.y);
                    draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                    p0.x -= Yertle->coordinates.resolution.x;
                    p0.y -= ddy;
                }
            } else {
                p0.x += Yertle->coordinates.resolution.x;
                p0.y += ddy;
                while ( p1.x >= p0.x ) {
                    CGWTDBG("[ X+] %g >= %g :: <%g, %g>  ", p1.x, p0.x, p0.x, p0.y);
                    draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                    p0.x += Yertle->coordinates.resolution.x;
                    p0.y += ddy;
                }
            }
        } else {
            // X is smaller:
            float   ddx = Yertle->coordinates.resolution.y * (dx / dy);
            if ( dy < 0 ) {
                p0.y -= Yertle->coordinates.resolution.y;
                p0.x -= ddx;
                while ( p0.y >= p1.y ) {
                    CGWTDBG("[ Y-] %g >= %g :: <%g, %g>  ", p1.y, p0.y, p0.x, p0.y);
                    draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                    p0.y -= Yertle->coordinates.resolution.y;
                    p0.x -= ddx;
                }
            } else {
                p0.y += Yertle->coordinates.resolution.y;
                p0.x += ddx;
                while ( p1.y >= p0.y ) {
                    CGWTDBG("[ Y+] %g >= %g :: <%g, %g>  ", p1.y, p0.y, p0.x, p0.y);
                    draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p0));
                    p0.y += Yertle->coordinates.resolution.y;
                    p0.x += ddx;
                }
            }
        }
    }
    CGWTDBG("DONE LINE <%g, %g>\n", p1.x, p1.y);
    draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, p1));
    return p1;
}

//

#define CGWBeginEvent(T)    if ( Yertle->event_observers[kCGWTurtleEventPathFinderStep].observer ) { \
                                event.base.from_turtle = Yertle, \
                                event.base.event_id = kCGWTurtleEventPathFinderStep, \
                                event.step_type = (T);
#define CGWEndEvent()           Yertle->event_observers[kCGWTurtleEventPathFinderStep].observer((CGWTurtleEvent*)&event, \
                                        Yertle->event_observers[kCGWTurtleEventPathFinderStep].context); \
                            }

#define CGWTurtlePosEq(P0, P1) \
            ((fabs((P1).x - (P0).x) < Yertle->coordinates.ranges.x_zero) && \
             (fabs((P1).y - (P0).y) < Yertle->coordinates.ranges.y_zero))

void
__CGWTurtleGeneratePath(
    CGWTurtleRef        Yertle,
    CGWPoint2D          destination
)
{
    CGWPoint2D                      current_loc = Yertle->position.turtle;
    CGWTurtleEdgeDetect             edge_found;
    bool                            is_path_complete = CGWTurtlePosEq(current_loc, destination);
    float                           angle;
    CGWTurtleEventPathFinderStep    event;
    CGWTurtleDrawPointFn            draw_fn;\
    
    draw_fn = Yertle->event_observers[kCGWTurtleEventDrawPixel].observer ? __CGWTurtleDrawPointAndSendEvent : __CGWTurtleDrawPoint;
    
    CGWBeginEvent(kCGWTurtlePathFinderStepBegin);
        event.step_data.begin.start = current_loc;
        event.step_data.begin.destination = destination;
    CGWEndEvent();
    
    // Make sure the point we're sitting at has been drawn:
    draw_fn(Yertle, __CGWTurtleCoordinateTurtleToScreen(Yertle, current_loc));
    
    while ( ! is_path_complete ) {
        // Is the destination inside the bounds?
        angle = __CGWTurtleAngleForPoints(current_loc, destination);
        if ( CGWRect2DContainsPoint(CGWTurtleRegion, destination) ) {
            //
            // Emit line segment current_loc -> destination
            //
            CGWBeginEvent(kCGWTurtlePathFinderStepLine);
                event.step_data.line.start = current_loc;
                event.step_data.line.end = destination;
            CGWEndEvent();
            
            current_loc= __CGWTurtleEmitLine(Yertle, current_loc, destination);
            is_path_complete = true;
        } else {
            CGWPoint2D  bounds_hit = __CGWTurtleClampPointToBoundsEdges(
                                                &current_loc,
                                                &destination,
                                                &Yertle->coordinates.ranges,
                                                &edge_found);
            //
            // Emit line segment current_loc -> bounds_hit
            // Yertle->position and current_loc should be at
            // bounds_hit after this procedure
            //
            CGWBeginEvent(kCGWTurtlePathFinderStepLine);
                event.step_data.line.start = current_loc;
                event.step_data.line.end = bounds_hit;
            CGWEndEvent();
            
            current_loc = __CGWTurtleEmitLine(Yertle, current_loc, bounds_hit);
            
            // Did we hit an edge?
            if ( edge_found == kCGWTurtleEdgeDetectNone ) {
                // Nope, didn't hit an edge, we must be done:
                is_path_complete = true;
            } else {
                //
                // Which wall did we hit?
                //
                if ( CGWTurtleEdgeDetectIsLeftRight(edge_found) ) {
                    //
                    // We're looking at behavior at the X bounds
                    //
                    if ( edge_found == kCGWTurtleEdgeDetectLeft ) {
                        CGWBeginEvent(kCGWTurtlePathFinderStepHitLeftWall);
                            event.step_data.hit_wall.hit_point = current_loc;
                        CGWEndEvent();
                    } else {
                        CGWBeginEvent(kCGWTurtlePathFinderStepHitRightWall);
                            event.step_data.hit_wall.hit_point = current_loc;
                        CGWEndEvent();
                    }
                    switch ( CGWTurtleBoundsType(Yertle->flags, X) ) {
                        case kCGWTurtleOptionsXBoundsHardWall: {
                            // The turtle has hit a left-right edge.  It
                            // will now either stop (incident angle {0,-π})
                            // or slide along the wall
                            if ( fabs(angle) < 1e-6 || fabs(angle + M_PI) < 1e-6 ) {
                                // Movement stops here:
                                is_path_complete = true;
                            } else {
                                if ( angle > 0 ) {
                                    angle = M_PI_2;
                                    destination.y = 1.0f;
                                } else {
                                    angle = -M_PI_2;
                                    destination.y = 0.0f;
                                }
                                destination.x = current_loc.x;
                                
                                CGWBeginEvent((edge_found == kCGWTurtleEdgeDetectLeft) ? kCGWTurtlePathFinderSlideOnLeftWall : kCGWTurtlePathFinderSlideOnRightWall)
                                    event.step_data.slide_on_wall.hit_point = bounds_hit;
                                    event.step_data.slide_on_wall.incident_angle = angle;
                                    event.step_data.slide_on_wall.new_destination = destination;
                                CGWEndEvent();
                            }
                            break;
                        }
                        case kCGWTurtleOptionsXBoundsPeriodic: {
                            // Angle of movement is preserved, turtle
                            // teleports to the opposite edge; the
                            // destination needs to be translated the
                            // same distance:
                            destination.x += (edge_found == kCGWTurtleEdgeDetectLeft) ? 1.0f : -1.0f;
                            current_loc.x = (edge_found == kCGWTurtleEdgeDetectLeft) ? 1.0f : 0.0f;
                            
                            CGWBeginEvent(kCGWTurtlePathFinderStepTeleport);
                                event.step_data.teleport.hit_point = bounds_hit;
                                event.step_data.teleport.teleport_to = current_loc;
                                event.step_data.teleport.new_destination = destination;
                            CGWEndEvent();
                            break;
                        }
                        case kCGWTurtleOptionsXBoundsElastic: {
                            // Depending which wall -- left or right -- we need
                            // to reflect the destination across that axis.  This
                            // will also alter the angle of the turtle:
                            float       orig_angle = angle;
                            
                            angle = M_PI - angle;
                            destination.x = current_loc.x - (destination.x - current_loc.x);
                            
                            CGWBeginEvent(kCGWTurtlePathFinderStepReflect);
                                event.step_data.reflect.hit_point = bounds_hit;
                                event.step_data.reflect.incident_angle = orig_angle;
                                event.step_data.reflect.reflected_angle = angle;
                                event.step_data.reflect.new_destination = destination;
                            CGWEndEvent();
                            break;
                        }
                    }
                }
                else {
                    //
                    // We're looking at behavior at the Y bounds
                    //
                    if ( edge_found == kCGWTurtleEdgeDetectTop ) {
                        CGWBeginEvent(kCGWTurtlePathFinderStepHitTopWall);
                            event.step_data.hit_wall.hit_point = current_loc;
                        CGWEndEvent();
                    } else {
                        CGWBeginEvent(kCGWTurtlePathFinderStepHitBottomWall);
                            event.step_data.hit_wall.hit_point = current_loc;
                        CGWEndEvent();
                    }
                    switch ( CGWTurtleBoundsType(Yertle->flags, Y) ) {
                        case kCGWTurtleOptionsYBoundsHardWall: {
                            // The turtle has hit a top-bottom edge.  It
                            // will now either stop (incident angle ±π/2)
                            // or slide along the wall
                            if ( fabs(angle - M_PI_2) < 1e-6 || fabs(angle + M_PI_2) < 1e-6 ) {
                                // Movement stops here:
                                is_path_complete = true;
                            } else {
                                if ( fabs(angle) > M_PI_2 ) {
                                    angle = -M_PI;
                                    destination.x = 0.0f;
                                } else {
                                    angle = 0;
                                    destination.x = 1.0f;
                                }
                                destination.y = current_loc.y;
                                
                                CGWBeginEvent((edge_found == kCGWTurtleEdgeDetectTop) ? kCGWTurtlePathFinderSlideOnTopWall : kCGWTurtlePathFinderSlideOnBottomWall)
                                    event.step_data.slide_on_wall.hit_point = bounds_hit;
                                    event.step_data.slide_on_wall.incident_angle = angle;
                                    event.step_data.slide_on_wall.new_destination = destination;
                                CGWEndEvent();
                            }
                            break;
                        }
                        case kCGWTurtleOptionsYBoundsPeriodic: {
                            // Angle of movement is preserved, turtle
                            // teleports to the opposite edge; the
                            // destination needs to be translated the
                            // same distance:
                            destination.y += (edge_found == kCGWTurtleEdgeDetectBottom) ? 1.0f : -1.0f;
                            current_loc.y = (edge_found == kCGWTurtleEdgeDetectBottom) ? 1.0f : 0.0f;
                            
                            CGWBeginEvent(kCGWTurtlePathFinderStepTeleport);
                                event.step_data.teleport.hit_point = bounds_hit;
                                event.step_data.teleport.teleport_to = current_loc;
                                event.step_data.teleport.new_destination = destination;
                            CGWEndEvent();
                            break;
                        }
                        case kCGWTurtleOptionsYBoundsElastic: {
                            // Depending which wall -- top or bottom -- we need
                            // to reflect the destination across that axis.  This
                            // will also alter the angle of the turtle:
                            float       orig_angle = angle;
                            
                            angle = -angle;
                            destination.y = current_loc.y - (destination.y - current_loc.y);
                            
                            CGWBeginEvent(kCGWTurtlePathFinderStepReflect);
                                event.step_data.reflect.hit_point = bounds_hit;
                                event.step_data.reflect.incident_angle = orig_angle;
                                event.step_data.reflect.reflected_angle = angle;
                                event.step_data.reflect.new_destination = destination;
                            CGWEndEvent();
                            break;
                        }
                    }
                }
            }
        }
    }
    CGWTurtleStateSetPosition(Yertle, current_loc);
    CGWTurtleStateSetAngle(Yertle, angle);
    CGWBeginEvent(kCGWTurtlePathFinderStepEnd);
        event.step_data.end.end = current_loc;
    CGWEndEvent();
}

#undef CGWTurtlePosEq
#undef CGWBeginEvent
#undef CGWEndEvent

//

CGWTurtleRef
CGWTurtleCreate(
    const char              *name,
    CGWRectI2D              screen_region,
    CGWBitvector            options
)
{
    CGWTurtle                   *Yertle = (CGWTurtle*)calloc(1, sizeof(CGWTurtle));
    static unsigned int         n_turtles = 0;
    
    if ( Yertle ) {
        if ( ! name ) {
            snprintf(Yertle->name, CGW_TURTLE_NAME_LENGTH_MAX, "turtle_%u", n_turtles++);
        } else {
            strncpy(Yertle->name, name , CGW_TURTLE_NAME_LENGTH_MAX);
        }
        CGWTurtleStateResetWithChanges(Yertle, screen_region, options);
    }
    return Yertle;
}

//

void
CGWTurtleDestroy(
    CGWTurtleRef    Yertle
)
{
    if ( Yertle->event_observers[kCGWTurtleEventIdWillDestroy].observer ) {
        CGWTurtleEvent  event = {
                .from_turtle = Yertle,
                .event_id = kCGWTurtleEventIdWillDestroy };
        // Return of false = do not destroy!
        if ( ! Yertle->event_observers[kCGWTurtleEventIdWillDestroy].observer(&event,
                    Yertle->event_observers[kCGWTurtleEventIdWillDestroy].context) ) return;
    }
    free((void*)Yertle);
    if ( Yertle->event_observers[kCGWTurtleEventIdDidDestroy].observer ) {
        CGWTurtleEvent  event = {
                .from_turtle = Yertle,
                .event_id = kCGWTurtleEventIdDidDestroy };
        Yertle->event_observers[kCGWTurtleEventIdDidDestroy].observer(&event,
                    Yertle->event_observers[kCGWTurtleEventIdDidDestroy].context);
    }
}

//

void
CGWTurtleSummarize(
    CGWTurtleRef    Yertle
)
{
    CGWPointI2D     s = CGWTurtleStateGetScreenPosition(Yertle);
    
    fprintf(stderr,
        "CGWTurtle@%p \"%s\" {\n"
        "   coordinates.screen_region = { origin=(%d, %d), size=(%u, %u) }\n"
        "   coordinates.resolution = (%g, %g)\n"
        "   coordinates.ranges = {\n"
        "       x = { (%g, %g) .. (%g, %g) }\n"
        "       y = { (%g, %g) .. (%g, %g) }\n"
        "   }\n"
        "   flags = " CGW_BITVECTOR_FMT "\n"
        "   position.turtle = (%g, %g)\n"
        "   position.screen = (%d, %d)\n"
        "   orientation.angle = %g\n"
        "   orientation.vector = <%g, %g>\n"
        "}\n",
        Yertle, Yertle->name,
        Yertle->coordinates.screen_region.origin.x, Yertle->coordinates.screen_region.origin.y, 
        Yertle->coordinates.screen_region.size.w, Yertle->coordinates.screen_region.size.h,
        Yertle->coordinates.resolution.x, Yertle->coordinates.resolution.y,
        Yertle->coordinates.ranges.x_zero_minus, Yertle->coordinates.ranges.x_zero,
            Yertle->coordinates.ranges.x_one, Yertle->coordinates.ranges.x_one_plus,
        Yertle->coordinates.ranges.y_zero_minus, Yertle->coordinates.ranges.y_zero,
            Yertle->coordinates.ranges.y_one, Yertle->coordinates.ranges.y_one_plus,
        Yertle->flags,
        Yertle->position.turtle.x, Yertle->position.turtle.y,
        s.x, s.y,
        Yertle->orientation.angle,
        Yertle->orientation.vector.x, Yertle->orientation.vector.y);
}

//

CGWTurtleRef
CGWTurtleStateReset(
    CGWTurtleRef    Yertle
)
{
    return CGWTurtleStateResetWithChanges(Yertle, Yertle->coordinates.screen_region, Yertle->flags);
}

//

CGWTurtleRef
CGWTurtleStateResetWithChanges(
    CGWTurtleRef    Yertle,
    CGWRectI2D      screen_region,
    CGWBitvector    options
)
{
    // Update the screen coordinates:
    Yertle->coordinates.screen_region = screen_region;
    __CGWTurtleCalculateResolutionAndRanges(Yertle);
    
    // Extract the user-facing flags and merge-in our private defaults:
    Yertle->flags = (options & kCGWTurtleOptionsMask);

    if ( Yertle->event_observers[kCGWTurtleEventIdInit].observer ) {
        CGWTurtleEventInit  event = {
                                    .base.from_turtle = Yertle,
                                    .base.event_id = kCGWTurtleEventIdInit,
                                    .options = Yertle->flags & kCGWTurtleOptionsBoundsMask,
                                    .screen_region = screen_region
                                };
        Yertle->event_observers[kCGWTurtleEventIdInit].observer((CGWTurtleEvent*)&event,
                    Yertle->event_observers[kCGWTurtleEventIdInit].context);
    }
    
    if ( CGWTurtleShouldLog(Yertle->flags) ) {
        fprintf(stderr,
            "CGWTurtle@%p[%s]:  bounds set to { origin=(%d, %d), size=(%u, %u) }\n",
            Yertle, Yertle->name,
            Yertle->coordinates.screen_region.origin.x, Yertle->coordinates.screen_region.origin.y,
            Yertle->coordinates.screen_region.size.w, Yertle->coordinates.screen_region.size.h);
    }
    
    CGWTurtleStateSetDrawingDelay(Yertle, 0.005);
    CGWTurtleStateSetPosition(Yertle, CGWPoint2DMake(0.5f, 0.5f));
    CGWTurtleStateSetAngle(Yertle, 0.0f);
    
    return Yertle;
}

//

bool
CGWTurtleStateGetStylusIsOn(
    CGWTurtleRef    Yertle
)
{
    return CGWBitvectorGetBitIndex(Yertle->flags, kCGWTurtleFlagsStylusIsOn) ? true : false;
}
CGWTurtleRef
CGWTurtleStateSetStylusIsOn(
    CGWTurtleRef    Yertle,
    bool            is_on
)
{
    if ( Yertle->event_observers[kCGWTurtleEventIdStylusStateChange].observer ) {
        CGWTurtleEventStylusStateChange event = {
                                            .base.from_turtle = Yertle,
                                            .base.event_id = kCGWTurtleEventIdStylusStateChange,
                                            .is_on = is_on
                                        };
        // Return of false = do not allow state change:
        if ( ! Yertle->event_observers[kCGWTurtleEventIdStylusStateChange].observer((CGWTurtleEvent*)&event,
                    Yertle->event_observers[kCGWTurtleEventIdStylusStateChange].context) )
            return Yertle;
    }
    CGWBitvectorSetBitIndex(Yertle->flags, kCGWTurtleFlagsStylusIsOn, is_on);
    if ( CGWTurtleShouldLog(Yertle->flags) ) {
        fprintf(stderr, "CGWTurtle@%p[%s]:  stylus is %s\n",
            Yertle, Yertle->name, is_on ? "on" : "off");
    }
    return Yertle;
}

//

CGWTurtleRef
CGWTurtleStateToggleStylusIsOn(
    CGWTurtleRef    Yertle
)
{
    return CGWTurtleStateSetStylusIsOn(Yertle,
                ! CGWBitvectorGetBitIndex(Yertle->flags, kCGWTurtleFlagsStylusIsOn));
}

//

double
CGWTurtleStateGetDrawingDelay(
    CGWTurtleRef    Yertle
)
{
    double          dt = 0.0;
    
    if ( CGWBitvectorGetBitIndex(Yertle->flags, kCGWTurtleFlagsHasDrawingDelay) ) {
        dt = Yertle->drawing_timer.cycle_time * 1e6;
    }
    return dt;
}

//

void
CGWTurtleStateSetDrawingDelay(
    CGWTurtleRef    Yertle,
    double          delay_in_seconds
)
{
    if ( delay_in_seconds > 0.0 ) {
        Yertle->drawing_timer = CGWTimingIntervalMake(1e6 * delay_in_seconds);
        CGWTimingIntervalStart(Yertle->drawing_timer);
        CGWBitvectorSetBitIndexOne(Yertle->flags, kCGWTurtleFlagsHasDrawingDelay);
    } else {
        CGWTimingIntervalStopNow(Yertle->drawing_timer);
        CGWBitvectorSetBitIndexZero(Yertle->flags, kCGWTurtleFlagsHasDrawingDelay);
    }
}

//

CGWPoint2D
CGWTurtleStateGetPosition(
    CGWTurtleRef    Yertle
)
{
    return Yertle->position.turtle;
}
CGWTurtleRef
CGWTurtleStateSetPosition(
    CGWTurtleRef    Yertle,
    CGWPoint2D      position
)
{   
    // Check for the x-coordinate's being outside the bounds:
    if ( position.x < Yertle->coordinates.ranges.x_zero || position.x > Yertle->coordinates.ranges.x_one ) {
        // For periodic boundary in x, map the coordinate back into the box
        if ( CGWTurtleBoundsIsPeriodic(Yertle->flags, X) ) {
            float       whole, frac = modff(position.x, &whole);
            
            position.x = ( position.x > Yertle->coordinates.ranges.x_one ) ? frac : (1.0f + frac);
        } else {
            // For hard-wall and elastic boundaries, just project
            // the coordinate to the proper edge:
            position.x = (position.x < Yertle->coordinates.ranges.x_zero) ? 0.0f : 1.0f;
        }
    }
    // Check for the y-coordinate's being outside the bounds:
    if ( position.y < Yertle->coordinates.ranges.y_zero || position.y > Yertle->coordinates.ranges.y_one ) {
        // For periodic boundary in y, map the coordinate back into the box
        if ( CGWTurtleBoundsIsPeriodic(Yertle->flags, Y) ) {
            float       whole, frac = modff(position.y, &whole);
            
            position.y = ( position.y > Yertle->coordinates.ranges.y_one ) ? frac : (1.0f + frac);
        } else {
            // For hard-wall and elastic boundaries, just project
            // the coordinate to the proper edge:
            position.y = (position.y < Yertle->coordinates.ranges.y_zero) ? 0.0f : 1.0f;
        }
    }
    
    if ( Yertle->event_observers[kCGWTurtleEventIdSetPosition].observer ) {
        CGWTurtleEventSetPosition   event = {
                                        .base.from_turtle = Yertle,
                                        .base.event_id = kCGWTurtleEventIdSetPosition,
                                        .position = position
                                    };
        int             result = Yertle->event_observers[kCGWTurtleEventIdSetPosition].observer((CGWTurtleEvent*)&event,
                                        Yertle->event_observers[kCGWTurtleEventIdSetPosition].context);
        switch ( result ) {
            case kCGWTurtleEventSetReturnCodeDoNotAlter:
                return Yertle;
            case kCGWTurtleEventSetAlteredValue:
                position = event.position;
                break;
            default:
                break;
        }
    }
    
    Yertle->position.turtle = position;
    
    if ( CGWTurtleShouldLog(Yertle->flags) ) {
        fprintf(stderr,
            "CGWTurtle@%p[%s]:  position.turtle set to (%g, %g)\n",
            Yertle, Yertle->name,
            position.x, position.y);
    }
    return Yertle;
}

//

CGWPointI2D
CGWTurtleStateGetScreenPosition(
    CGWTurtleRef    Yertle
)
{
    return __CGWTurtleCoordinateTurtleToScreen(Yertle, Yertle->position.turtle);
}
CGWTurtleRef
CGWTurtleStateSetScreenPosition(
    CGWTurtleRef    Yertle,
    CGWPointI2D     position
)
{
    return CGWTurtleStateSetPosition(Yertle, __CGWTurtleCoordinateScreenToTurtle(Yertle, position));
}

//

float
CGWTurtleStateGetAngle(
    CGWTurtleRef    Yertle
)
{
    return Yertle->orientation.angle;
}
CGWTurtleRef
CGWTurtleStateSetAngle(
    CGWTurtleRef    Yertle,
    float           angle
)
{
    angle = __CGWTurtleStandardizeAngle(angle);

    if ( Yertle->event_observers[kCGWTurtleEventIdSetAngle].observer ) {
        CGWTurtleEventSetAngle  event = {
                                    .base.from_turtle = Yertle,
                                    .base.event_id = kCGWTurtleEventIdSetAngle,
                                    .theta = angle };
        int             result = Yertle->event_observers[kCGWTurtleEventIdSetAngle].observer((CGWTurtleEvent*)&event,
                                        Yertle->event_observers[kCGWTurtleEventIdSetAngle].context);
        switch ( result ) {
            case kCGWTurtleEventSetReturnCodeDoNotAlter:
                return Yertle;
            case kCGWTurtleEventSetAlteredValue:
                angle = event.theta;
                break;
            default:
                break;
        }
    }
    
    Yertle->orientation.angle = angle;
    __CGWTurtleCalculateOrientationVector(Yertle);

    if ( CGWTurtleShouldLog(Yertle->flags) ) {
        fprintf(stderr,
            "CGWTurtle@%p[%s]:  orientation angle set to %g (%g deg)\n"
            "CGWTurtle@%p[%s]:  orientation vector set to <%g, %g>\n",
            Yertle, Yertle->name, Yertle->orientation.angle, CGWRadiansToDegrees(Yertle->orientation.angle),
            Yertle, Yertle->name, Yertle->orientation.vector.x, Yertle->orientation.vector.y);
    }
    return Yertle;
}

//

CGWPoint2D
CGWTurtleStateGetOrientationVector(
    CGWTurtleRef    Yertle
)
{
    return Yertle->orientation.vector;
}

//

CGWTurtleRef
CGWTurtleStateMove(
    CGWTurtleRef    Yertle,
    float           dposition
)
{
    // Generate the movement vector from the orientation vector:
    CGWPoint2D      v = { .x = Yertle->orientation.vector.x * dposition,
                          .y = Yertle->orientation.vector.y * dposition };
    v = CGWPoint2DSum(Yertle->position.turtle, v);
    return CGWTurtleStateSetPosition(Yertle, v);
}

//

CGWTurtleRef
CGWTurtleStateTurn(
    CGWTurtleRef    Yertle,
    float           dtheta
)
{
    return CGWTurtleStateSetAngle(Yertle, Yertle->orientation.angle + dtheta);
}

//

CGWTurtleEventObserver
CGWTurtleGetEventObserver(
    CGWTurtleRef        Yertle,
    CGWTurtleEventId    event_id
)
{
    if ( event_id >= 0 && event_id < kCGWTurtleEventIdMax ) return Yertle->event_observers[event_id].observer;
    return NULL;
}

//

void
CGWTurtleSetEventObserver(
    CGWTurtleRef            Yertle,
    CGWTurtleEventId        event_id,
    CGWTurtleEventObserver  observer,
    const void              *context
)
{
    if ( event_id >= 0 && event_id < kCGWTurtleEventIdMax ) {
        Yertle->event_observers[event_id].observer = observer;
        Yertle->event_observers[event_id].context = context;
    }
}

//

void
CGWTurtleActionMove(
    CGWTurtleRef    Yertle,
    float           dposition
)
{
    CGWPoint2D      destination = {
                        .x = Yertle->position.turtle.x + dposition * Yertle->orientation.vector.x,
                        .y = Yertle->position.turtle.y + dposition * Yertle->orientation.vector.y
                    };
    __CGWTurtleGeneratePath(Yertle, destination);
}

//

void
CGWTurtleActionMoveTo(
    CGWTurtleRef    Yertle,
    CGWPoint2D      position
)
{
    __CGWTurtleGeneratePath(Yertle, position);
}

//

void
CGWTurtleActionCurve(
    CGWTurtleRef    Yertle,
    CGWPoint2D      control_point,
    float           phi
)
{
    // The control_point in the origin, so find the vector from the
    // control_point to the current turtle position...
    CGWPoint2D      v0 = { .x = Yertle->position.turtle.x - control_point.x,
                           .y = Yertle->position.turtle.y - control_point.y
                         };
    // ...and convert that to a polar coordiate centered at the
    // control_point:
    CGWPolarPoint   pp = CGWPoint2DToPolarPoint(v0);
    
    // The turtle's starting position:
    CGWPoint2D      pt = Yertle->position.turtle, pt_prime;
    
    // At this point we have the radius and starting angle in pp.  The initial
    // turtle angle will be the polar theta ± π/2 (depending on the sign of
    // phi:
    float           theta_t = __CGWTurtleStandardizeAngle(pp.theta + copysignf(M_PI_2, phi));
    
    CGWTDBG("v0 = <%g, %g>; pp = <%g, %g (%g°)>; pt = <%g, %g>\n",
        v0.x, v0.y, pp.r, pp.theta, CGWRadiansToDegrees(pp.theta), pt.x, pt.y);
    CGWTDBG("theta_t = %g (%g°); phi = %g (%g°)\n",
        theta_t, CGWRadiansToDegrees(theta_t), phi, CGWRadiansToDegrees(phi));

    if ( phi < 0.0f ) {
        float       dphi_x = atan2f(pp.r, Yertle->coordinates.resolution.x),
                    dphi_y = atan2f(Yertle->coordinates.resolution.y, pp.r),
                    dphi = (dphi_x < dphi_y) ? dphi_x : dphi_y;
                    
        while ( phi < 0.0f ) {
            // Limit to the minimum-magnitude of dphi or whatever's left of phi:
            if ( fabs(phi) < fabs(dphi) ) dphi = -phi;
            
            // Rotate theta and phi by dphi radians:
            pp.theta -= dphi;
            theta_t -= dphi;
            phi += dphi;

            CGWTDBG("theta = %g (%g°); theta_t = %g (%g°); phi = %g (%g°)\n", pp.theta, CGWRadiansToDegrees(pp.theta), theta_t, CGWRadiansToDegrees(theta_t), phi, CGWRadiansToDegrees(phi));

            v0 = CGWPolarPointToPoint2D(pp);

            CGWTDBG("v = <%g, %g>; |v| = %g\n", v0.x, v0.y, CGWPoint2DNormL2(v0));

            pt_prime = CGWPoint2DSum(control_point, v0);

            CGWTDBG("pt' = <%g, %g>; pt = <%g, %g>\n", pt_prime.x, pt_prime.y, pt.x ,pt.y);

            __CGWTurtleEmitLine(Yertle, pt, pt_prime);
            pt = pt_prime;
        }
    } else {
        float       dphi_x = atan2f(pp.r, Yertle->coordinates.resolution.x),
                    dphi_y = atan2f(Yertle->coordinates.resolution.y, pp.r),
                    dphi = (dphi_x < dphi_y) ? dphi_x : dphi_y;
                    
        while ( phi > 0.0f ) {
            // Limit to the minimum-magnitude of dphi or whatever's left of phi:
            if ( fabs(phi) < fabs(dphi) ) dphi = phi;
            
            // Rotate theta and phi by dphi radians:
            pp.theta += dphi;
            theta_t += dphi;
            phi -= dphi;

            CGWTDBG("theta = %g (%g°); theta_t = %g (%g°); phi = %g (%g°)\n", pp.theta, CGWRadiansToDegrees(pp.theta), theta_t, CGWRadiansToDegrees(theta_t), phi, CGWRadiansToDegrees(phi));

            v0 = CGWPolarPointToPoint2D(pp);

            CGWTDBG("v = <%g, %g>; |v| = %g\n", v0.x, v0.y, CGWPoint2DNormL2(v0));

            pt_prime = CGWPoint2DSum(control_point, v0);

            CGWTDBG("pt' = <%g, %g>; pt = <%g, %g>\n", pt_prime.x, pt_prime.y, pt.x ,pt.y);

            __CGWTurtleEmitLine(Yertle, pt, pt_prime);
            pt = pt_prime;
        }
    }
    // Reset the turtle's direction:
    CGWTurtleStateSetPosition(Yertle, pt);
    CGWTurtleStateSetAngle(Yertle, theta_t);
}
   

//

void
CGWTurtleActionCurveAndContract(
    CGWTurtleRef    Yertle,
    CGWPoint2D      control_point,
    float           phi,
    float           dr_per_radian,
    bool            is_proportional_to_r
)
{
    // The control_point in the origin, so find the vector from the
    // control_point to the current turtle position...
    CGWPoint2D      v0 = { .x = Yertle->position.turtle.x - control_point.x,
                           .y = Yertle->position.turtle.y - control_point.y
                         };
    // ...and convert that to a polar coordiate centered at the
    // control_point:
    CGWPolarPoint   pp = CGWPoint2DToPolarPoint(v0);
    
    // The turtle's starting position:
    CGWPoint2D      pt = Yertle->position.turtle, pt_prime;
    
    // At this point we have the radius and starting angle in pp.  The initial
    // turtle angle will be the polar theta ± π/2 (depending on the sign of
    // phi:
    float           theta_t = __CGWTurtleStandardizeAngle(pp.theta + copysignf(M_PI_2, phi));
    

    CGWTDBG("v0 = <%g, %g>; pp = <%g, %g (%g°)>; pt = <%g, %g>\n",
        v0.x, v0.y, pp.r, pp.theta, CGWRadiansToDegrees(pp.theta), pt.x, pt.y);
    CGWTDBG("theta_t = %g (%g°); phi = %g (%g°)\n",
        theta_t, CGWRadiansToDegrees(theta_t), phi, CGWRadiansToDegrees(phi));

        
    if ( phi < 0.0f ) {
        while ( pp.r > 0.0f && phi < 0.0f ) {
            float       dphi_x = atan2f(pp.r, Yertle->coordinates.resolution.x),
                        dphi_y = atan2f(Yertle->coordinates.resolution.y, pp.r),
                        dphi = (dphi_x < dphi_y) ? dphi_x : dphi_y;
                        
            // Limit to the minimum-magnitude of dphi or whatever's left of phi:
            if ( fabs(phi) < fabs(dphi) ) dphi = -phi;
            
            // Rotate theta and phi by dphi radians:
            pp.theta -= dphi;
            theta_t -= dphi;
            phi += dphi;
            // Contract r:
            if ( is_proportional_to_r )
                pp.r -= pp.r * (dphi * dr_per_radian);
            else
                pp.r -= (dphi * dr_per_radian);

            CGWTDBG("theta = %g (%g°); theta_t = %g (%g°); phi = %g (%g°)\n", pp.theta, CGWRadiansToDegrees(pp.theta), theta_t, CGWRadiansToDegrees(theta_t), phi, CGWRadiansToDegrees(phi));

            v0 = CGWPolarPointToPoint2D(pp);

            CGWTDBG("v = <%g, %g>; |v| = %g\n", v0.x, v0.y, CGWPoint2DNormL2(v0));

            pt_prime = CGWPoint2DSum(control_point, v0);

            CGWTDBG("pt' = <%g, %g>; pt = <%g, %g>\n", pt_prime.x, pt_prime.y, pt.x ,pt.y);

            __CGWTurtleEmitLine(Yertle, pt, pt_prime);
            pt = pt_prime;
        }
    } else {        
        while ( pp.r > 0.0f && phi > 0.0f ) {
            float       dphi_x = atan2f(pp.r, Yertle->coordinates.resolution.x),
                        dphi_y = atan2f(Yertle->coordinates.resolution.y, pp.r),
                        dphi = (dphi_x < dphi_y) ? dphi_x : dphi_y;
                        
            // Limit to the minimum-magnitude of dphi or whatever's left of phi:
            if ( fabs(phi) < fabs(dphi) ) dphi = phi;
            
            // Rotate theta and phi by dphi radians:
            pp.theta += dphi;
            theta_t += dphi;
            phi -= dphi;
            // Contract r:
            if ( is_proportional_to_r )
                pp.r -= pp.r * (dphi * dr_per_radian);
            else
                pp.r -= (dphi * dr_per_radian);

            CGWTDBG("theta = %g (%g°); theta_t = %g (%g°); phi = %g (%g°)\n", pp.theta, CGWRadiansToDegrees(pp.theta), theta_t, CGWRadiansToDegrees(theta_t), phi, CGWRadiansToDegrees(phi));

            v0 = CGWPolarPointToPoint2D(pp);

            CGWTDBG("v = <%g, %g>; |v| = %g\n", v0.x, v0.y, CGWPoint2DNormL2(v0));

            pt_prime = CGWPoint2DSum(control_point, v0);

            CGWTDBG("pt' = <%g, %g>; pt = <%g, %g>\n", pt_prime.x, pt_prime.y, pt.x ,pt.y);

            __CGWTurtleEmitLine(Yertle, pt, pt_prime);
            pt = pt_prime;
        }
    }
    // Reset the turtle's direction:
    CGWTurtleStateSetPosition(Yertle, pt);
    CGWTurtleStateSetAngle(Yertle, theta_t);
} 
