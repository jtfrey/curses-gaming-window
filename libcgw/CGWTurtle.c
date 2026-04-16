/**
 * @file CGWTurtle.c
 * @brief Curses Gaming Window, Turtle graphics
 * 
 * Source code that implements the header.
 *
 */

#include "CGWTurtle.h"

#define CGW_TURTLE_NAME_LENGTH_MAX  64
 
/*
 * Private bits in the options/flags bit vector:
 */
enum {
    kCGWTurtleFlagsStylusIsFresh                = 16,
    kCGWTurtleFlagsStylusIsOn                   = 17,
    kCGWTurtleFlagsAngleIsZeroOrPi              = 18,
    kCGWTurtleFlagsAngleIsPlusMinusPiOverTwo    = 19,
    
    kCGWTurtleFlagsStylusIsFreshMask                = CGWBitvectorMask(kCGWTurtleFlagsStylusIsFresh),
    kCGWTurtleFlagsStylusIsOnMask                   = CGWBitvectorMask(kCGWTurtleFlagsStylusIsOn),
    kCGWTurtleFlagsAngleIsZeroOrPiMask              = CGWBitvectorMask(kCGWTurtleFlagsAngleIsZeroOrPi),
    kCGWTurtleFlagsAngleIsPlusMinusPiOverTwoMask    = CGWBitvectorMask(kCGWTurtleFlagsAngleIsPlusMinusPiOverTwo),
    
    kCGWTurtleFlagsMask                         = kCGWTurtleFlagsStylusIsFreshMask
                                                    | kCGWTurtleFlagsStylusIsOnMask
                                                    | kCGWTurtleFlagsAngleIsZeroOrPiMask
                                                    |kCGWTurtleFlagsAngleIsPlusMinusPiOverTwoMask
};

//

#define CGWTurtleShouldLog(BV) (CGWBitvectorGetBitIndex((BV), kGGWTurtleOptionsEnableLogging))

#define CGWTurtleBoundsType(BV, AXIS) \
            (CGWBitvectorGetBits((BV), kCGWTurtleOptions ## AXIS ## BoundsMask))

#define CGWTurtleBoundsIsPeriodic(BV, AXIS) \
            ((CGWBitvectorGetBits((BV), kCGWTurtleOptions ## AXIS ## BoundsMask) == kCGWTurtleOptions ## AXIS ## BoundsPeriodic) ? true : false)

//

typedef struct CGWTurtle {
    char                        name[CGW_TURTLE_NAME_LENGTH_MAX];
    CGWRect                     bounds;
    CGWBitvector                flags;
    struct {
        CGWPoint                cartesian;
        CGWPointP               polar;
    } position;
    float                       angle;
    float                       theta_min_x;
    float                       theta_min_y;
    CGWTurtleCallbacks          callbacks;
    struct {
        CGWTurtleEventObserver  observer;
        const void              *context;
    } event_observers[kCGWTurtleEventIdMax];
} CGWTurtle;

//

CGWPoint
__CGWTurtleDefaultCoordToWorldCallback(
    CGWTurtleRef    Yertle,
    CGWPoint        turtle_coord,
    const void      *context
)
{
    CGWPoint        w;
    w.x = (Yertle->bounds.size.w / 2) + turtle_coord.x;
    w.y = (Yertle->bounds.size.h / 2) - turtle_coord.y;
    return w;
}
const CGWTurtleCoordToWorldCallback CGWTurtleDefaultCoordToWorldCallback = \
            __CGWTurtleDefaultCoordToWorldCallback;
            
CGWPoint
__CGWTurtleDefaultCoordFromWorldCallback(
    CGWTurtleRef    Yertle,
    CGWPoint        world_coord,
    const void      *context
)
{
    CGWPoint        t;
    
    t.x = world_coord.x - (Yertle->bounds.size.w / 2);
    t.y = (Yertle->bounds.size.h / 2) - world_coord.y;
    return t;
}         
const CGWTurtleCoordFromWorldCallback CGWTurtleDefaultCoordFromWorldCallback = \
    __CGWTurtleDefaultCoordFromWorldCallback;

//

const CGWTurtleCallbacks __CGWTurtleDefaultCallbacks = {
            .draw_pixel = NULL,
            .turtle_to_world = CGWTurtleDefaultCoordToWorldCallback,
            .world_to_turtle = CGWTurtleDefaultCoordFromWorldCallback
        };
const CGWTurtleCallbacks *CGWTurtleDefaultCallbacks = \
    &__CGWTurtleDefaultCallbacks;

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

CGWPoint
__CGWTurtleClampPointToBoundsEdges(
    CGWRect             *bounds,
    CGWPoint            *p1,
    CGWPoint            *p2,
    CGWTurtleEdgeDetect *edge_detected             
)
{
    CGWPoint            p2_prime;
    double              m;
    
    // Pre-condition:  p1 is interior to bounds, p1 != p2
    
    *edge_detected = kCGWTurtleEdgeDetectNone;
    
    // Is p2 interior to the bounds?
    if ( CGWRectContainsPoint(*bounds, *p2) ) return *p2;
    
    // If the line connecting p1 to p2 is vertical,
    // we need only test the top/bottom edges:
    if ( p1->x == p2->x ) {
        p2_prime.x = p2->x;
        if ( p2->y >= -bounds->origin.y ) {
            p2_prime.y = -bounds->origin.y;
            *edge_detected = kCGWTurtleEdgeDetectTop;
        }
        else if ( p2->y <= bounds->origin.y ) {
            p2_prime.y = bounds->origin.y;
            *edge_detected = kCGWTurtleEdgeDetectBottom;
        }
        return p2_prime;
    }
    
    // If the line connecting p1 to p2 is horizontal,
    // we need only test the left/right edges:
    if ( p1->y == p2->y ) {
        p2_prime.y = p2->y;
        if ( p2->x >= -bounds->origin.x ) {
            p2_prime.x = -bounds->origin.x;
            *edge_detected = kCGWTurtleEdgeDetectRight;
        }
        else if ( p2->x <= bounds->origin.x ) {
            p2_prime.x = bounds->origin.x;
            *edge_detected = kCGWTurtleEdgeDetectLeft;
        }
        return p2_prime;
    }
    
    // Calculate the slope:
    m = (double)(p2->y - p1->y) / (double)(p2->x - p1->x);
    
    // Does p2 lie beyond the right edge?
    if ( p2->x >= -bounds->origin.x ) {
        //
        // Solve y = m(-bounds->origin.x - p1->x) + p1->y
        //
        double  y = -m * (double)(bounds->origin.x + p1->x) + (double)p1->y;
        
        // If y is between the top/bottom bounds, we're done:
        if ( fabs(y) <= -bounds->origin.y ) {
            p2_prime.x = -bounds->origin.x, p2_prime.y = round(y);
            *edge_detected = kCGWTurtleEdgeDetectRight;
            return p2_prime;
        }
        
        // y is outside the top/bottom limit, so the line intersects
        // the top/bottom edge and not the right edge
    }
    // Does p2 lie beyond the left edge?
    else if ( p2->x <= bounds->origin.x ) {
        //
        // Solve y = m(bounds->origin.x - p1->x) + p1->y
        //
        double  y = m * (double)(bounds->origin.x - p1->x) + (double)p1->y;
        
        // If y is between the top/bottom bounds, we're done:
        if ( fabs(y) <= -bounds->origin.y ) {
            p2_prime.x = bounds->origin.x, p2_prime.y = round(y);
            *edge_detected = kCGWTurtleEdgeDetectLeft;
            return p2_prime;
        }
        
        // y is outside the top/bottom limit, so the line intersects
        // the top/bottom edge and not the right edge
    }
    
    // We need the inverted slope for the rest of the calculations:
    m = 1.0 / m;
    
    // Does p2 lie beyond the top edge?
    if ( p2->y >= -bounds->origin.y ) {
        //
        // Solve x = (1/m)(-bounds->origin.y - p1->y) + p1->x
        //
        double  x = -m * (double)(bounds->origin.y + p1->y) + (double)p1->x;
        
        // If x is between the left/right bounds, we're done:
        if ( fabs(x) <= -bounds->origin.x ) {
            p2_prime.y = -bounds->origin.y, p2_prime.x = round(x);
            *edge_detected = kCGWTurtleEdgeDetectTop;
            return p2_prime;
        }
        
        // x is outside the left/right limit, so the line intersects...
        // what?
    }
    // Does p2 lie beyond the bottom edge?
    if ( p2->y <= bounds->origin.y ) {
        //
        // Solve x = (1/m)(bounds->origin.y - p1->y) + p1->x
        //
        double  x = m * (double)(bounds->origin.y - p1->y) + (double)p1->x;
        
        // If x is between the left/right bounds, we're done:
        if ( fabs(x) <= -bounds->origin.x ) {
            p2_prime.y = bounds->origin.y, p2_prime.x = round(x);
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

void
__CGWTurtleCalculatePolarPosition(
    CGWTurtleRef        Yertle
)
{
    Yertle->position.polar = CGWPointToPolar(Yertle->position.cartesian,
                                kCGWCoordConvRoundModeRound);
}

//

float
__CGWTurtleStandardizeAngle(
    float       theta
)
{
    if ( theta > M_PI ) {
        theta -= M_PI * floor(theta / M_PI);
        if ( fabs(theta - M_PI) < 1e-6 ) theta = -M_PI;
    }
    else if ( theta < -M_PI ) {
        theta += M_PI * ceil(theta / M_PI);
    }
    return theta;
}

//

float
__CGWTurtleAngleForPoints(
    CGWPoint        p1,
    CGWPoint        p2
)
{
    double          dx = (double)(p2.x - p1.x),
                    dy = (double)(p2.y - p1.y);
    double          theta = atan2(dy, dx);
    
    if ( fabs(theta - M_PI) < 1e-6 ) return -M_PI;
    return theta;
}

//

CGWTurtleRef
CGWTurtleCreate(
    const char                  *name,
    CGWSize                     bounds,
    CGWBitvector                options,
    const CGWTurtleCallbacks    *cbs
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
        CGWTurtleSetCallbacks(Yertle, cbs);
        CGWTurtleStateResetWithChanges(Yertle, bounds, options);
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

CGWTurtleRef
CGWTurtleStateReset(
    CGWTurtleRef    Yertle
)
{
    return CGWTurtleStateResetWithChanges(Yertle, Yertle->bounds.size, Yertle->flags);
}

//

CGWTurtleRef
CGWTurtleStateResetWithChanges(
    CGWTurtleRef    Yertle,
    CGWSize         bounds,
    CGWBitvector    options
)
{
    Yertle->bounds = CGWRectMake(
                        -(bounds.w / 2), -(bounds.h / 2),
                        bounds.w, bounds.h);
    Yertle->flags = (options & kCGWTurtleOptionsMask)
                        | kCGWTurtleFlagsStylusIsFreshMask
                        | kCGWTurtleFlagsAngleIsZeroOrPiMask;
    CGWPointPGetThetaMin(bounds, &Yertle->theta_min_x, &Yertle->theta_min_y);
    
    if ( Yertle->event_observers[kCGWTurtleEventIdSetBounds].observer ) {
        CGWTurtleEvent  event = {
                .from_turtle = Yertle,
                .event_id = kCGWTurtleEventIdSetBounds,
                .event_data.set_bounds.options = Yertle->flags & kCGWTurtleOptionsBoundsMask,
                .event_data.set_bounds.size = bounds };
        Yertle->event_observers[kCGWTurtleEventIdSetBounds].observer(&event,
                    Yertle->event_observers[kCGWTurtleEventIdSetBounds].context);
    }
    
    
    if ( CGWTurtleShouldLog(Yertle->flags) ) {
        fprintf(stderr,
            "CGWTurtle@%p[%s]:  bounds set to { origin=(%d, %d), size=(%u, %u) }\n"
            "CGWTurtle@%p[%s]:  theta_min set to (%g, %g)\n",
            Yertle, Yertle->name,
            Yertle->bounds.origin.x, Yertle->bounds.origin.y, Yertle->bounds.size.w, Yertle->bounds.size.h,
            Yertle, Yertle->name,
            Yertle->theta_min_x, Yertle->theta_min_y);
    }
    
    CGWTurtleStateSetPosition(Yertle, CGWPointOrigin);
    CGWTurtleStateSetAngle(Yertle, 0.0f);
    
    return Yertle;
}

//
void
CGWTurtleSummarize(
    CGWTurtleRef    Yertle
)
{
    fprintf(stderr,
        "CGWTurtle@%p \"%s\" {\n"
        "   bounds = { origin=(%d, %d), size=(%u, %u) }\n"
        "   flags = " CGW_BITVECTOR_FMT "\n"
        "   position.cartesian = (%d, %d)\n"
        "   position.polar = (%u, %g)\n"
        "   angle = %g\n"
        "   theta_min = (%g, %g)\n"
        "}\n",
        Yertle, Yertle->name,
        Yertle->bounds.origin.x, Yertle->bounds.origin.y, Yertle->bounds.size.w, Yertle->bounds.size.h,
        Yertle->flags,
        Yertle->position.cartesian.x, Yertle->position.cartesian.y,
        Yertle->position.polar.r, Yertle->position.polar.theta,
        Yertle->angle,
        Yertle->theta_min_x, Yertle->theta_min_y);
}

//

const void*
CGWTurtleGetCallbackContext(
    CGWTurtleRef    Yertle
)
{
    return Yertle->callbacks.context;
}
CGWTurtleRef
CGWTurtleSetCallbackContext(
    CGWTurtleRef    Yertle,
    const void      *cb_context
)
{
    Yertle->callbacks.context = cb_context;
    return Yertle;
}

//

void
CGWTurtleGetCallbacks(
    CGWTurtleRef        Yertle,
    CGWTurtleCallbacks  *cbs
)
{
    *cbs = Yertle->callbacks;
}
CGWTurtleRef
CGWTurtleSetCallbacks(
    CGWTurtleRef                Yertle,
    const CGWTurtleCallbacks    *cbs
)
{
    if ( ! cbs ) {
        memset(&Yertle->callbacks, 0, sizeof(Yertle->callbacks));
    } else {
        Yertle->callbacks = *cbs;
    }
    return Yertle;
}

//

CGWTurtleDrawPixelCallback
CGWTurtleGetDrawPixelCallback(
    CGWTurtleRef    Yertle
)
{
    return Yertle->callbacks.draw_pixel;
}
CGWTurtleRef
CGWTurtleSetDrawPixelCallback(
    CGWTurtleRef                Yertle,
    CGWTurtleDrawPixelCallback  cb
)
{
    Yertle->callbacks.draw_pixel = cb;
    return Yertle;
}

//

CGWTurtleCoordToWorldCallback
CGWTurtleGetCoordToWorldCallback(
    CGWTurtleRef    Yertle
)
{
    return Yertle->callbacks.turtle_to_world;
}
CGWTurtleRef
CGWTurtleSetCoordToWorldCallback(
    CGWTurtleRef                    Yertle,
    CGWTurtleCoordToWorldCallback   cb
)
{
    Yertle->callbacks.turtle_to_world = cb;
    return Yertle;
}

//

CGWTurtleCoordFromWorldCallback
CGWTurtleGetCoordFromWorldCallback(
    CGWTurtleRef    Yertle
)
{
    return Yertle->callbacks.world_to_turtle;
}
CGWTurtleRef
CGWTurtleSetCoordFromWorldCallback(
    CGWTurtleRef                    Yertle,
    CGWTurtleCoordFromWorldCallback cb
)
{
    Yertle->callbacks.world_to_turtle = cb;
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
        CGWTurtleEvent  event = {
                .from_turtle = Yertle,
                .event_id = kCGWTurtleEventIdStylusStateChange,
                .event_data.stylus_state_change = is_on };
        // Return of false = do not allow state change:
        if ( ! Yertle->event_observers[kCGWTurtleEventIdStylusStateChange].observer(&event,
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

CGWPoint
CGWTurtleStateGetPosition(
    CGWTurtleRef    Yertle
)
{
    return Yertle->position.cartesian;
}
CGWTurtleRef
CGWTurtleStateSetPosition(
    CGWTurtleRef    Yertle,
    CGWPoint        position
)
{
    CGWPointP       polar;
    
    // Check for the x-coordinate's being outside the bounds:
    if ( position.x < Yertle->bounds.origin.x || position.x > -Yertle->bounds.origin.x ) {
        // For periodic boundary in x, map the coordinate back into the box
        if ( CGWTurtleBoundsIsPeriodic(Yertle->flags, X) ) {
            if ( position.x > 0 ) {
                while ( position.x > -Yertle->bounds.origin.x ) position.x -= Yertle->bounds.size.w;
            } else {
                while ( position.x < Yertle->bounds.origin.x ) position.x += Yertle->bounds.size.w;
            }
        } else {
            // For hard-wall and elastic boundaries, just project
            // map the coordinate to the proper edge:
            position.x = (position.x < 0) ? Yertle->bounds.origin.x : -Yertle->bounds.origin.x;
        }
    }
    // Check for the y-coordinate's being outside the bounds:
    if ( position.y < Yertle->bounds.origin.y || position.y > -Yertle->bounds.origin.y ) {
        // For periodic boundary in y, map the coordinate back into the box
        if ( CGWTurtleBoundsIsPeriodic(Yertle->flags, Y) ) {
            if ( position.y > 0 ) {
                while ( position.y > -Yertle->bounds.origin.y ) position.y -= Yertle->bounds.size.h;
            } else {
                while ( position.y < Yertle->bounds.origin.y ) position.y += Yertle->bounds.size.h;
            }
        } else {
            // For hard-wall and elastic boundaries, just project
            // map the coordinate to the proper edge:
            position.y = (position.y < 0) ? Yertle->bounds.origin.y : -Yertle->bounds.origin.y;
        }
    }
    polar = CGWPointToPolar(position, kCGWCoordConvRoundModeRound);
    
    if ( Yertle->event_observers[kCGWTurtleEventIdSetPosition].observer ) {
        CGWTurtleEvent  event = {
                .from_turtle = Yertle,
                .event_id = kCGWTurtleEventIdSetPosition,
                .event_data.set_position.cartesian = position,
                .event_data.set_position.polar = polar };
        int             result = Yertle->event_observers[kCGWTurtleEventIdSetPosition].observer(&event,
                                        Yertle->event_observers[kCGWTurtleEventIdSetPosition].context);
        switch ( result ) {
            case kCGWTurtleEventSetReturnCodeDoNotAlter:
                return Yertle;
            case kCGWTurtleEventSetAlteredValue:
                position = event.event_data.set_position.cartesian;
                polar = CGWPointToPolar(position, kCGWCoordConvRoundModeRound);
                break;
            default:
                break;
        }
    }
    
    Yertle->position.cartesian = position;
    Yertle->position.polar = polar;
    CGWBitvectorSetBitIndex(Yertle->flags, kCGWTurtleFlagsAngleIsZeroOrPi,
            ((Yertle->position.polar.theta == 0.0f ) || (Yertle->position.polar.theta == -M_PI )));
    CGWBitvectorSetBitIndex(Yertle->flags, kCGWTurtleFlagsAngleIsPlusMinusPiOverTwo,
            (fabs(Yertle->position.polar.theta) == M_PI_2));
            
    if ( CGWTurtleShouldLog(Yertle->flags) ) {
        fprintf(stderr,
            "CGWTurtle@%p[%s]:  position.cartesian set to (%d, %d)\n"
            "CGWTurtle@%p[%s]:  position.polar set to (%u, %g = %g deg)\n",
            Yertle, Yertle->name,
            position.x, position.y,
            Yertle, Yertle->name,
            polar.r, polar.theta,
                CGWRadiansToDegrees(polar.theta));
    }
    return Yertle;
}

//

CGWPointP
CGWTurtleStateGetPolarPosition(
    CGWTurtleRef    Yertle
)
{
    return Yertle->position.polar;
}
CGWTurtleRef
CGWTurtleStateSetPolarPosition(
    CGWTurtleRef    Yertle,
    CGWPointP       position
)
{
    return CGWTurtleStateSetPosition(Yertle,
                CGWPointPToCartesian(position, kCGWCoordConvRoundModeRound));
}

//

CGWPoint
CGWTurtleStateGetWorldPosition(
    CGWTurtleRef    Yertle
)
{
    if ( Yertle->callbacks.turtle_to_world )
        return Yertle->callbacks.turtle_to_world(
                    Yertle,
                    Yertle->position.cartesian,
                    Yertle->callbacks.context);
    return Yertle->position.cartesian;
}
CGWTurtleRef
CGWTurtleStateSetWorldPosition(
    CGWTurtleRef    Yertle,
    CGWPoint        wposition
)
{
    if ( Yertle->callbacks.world_to_turtle )
        wposition = Yertle->callbacks.world_to_turtle(
                        Yertle,
                        wposition,
                        Yertle->callbacks.context);
    return CGWTurtleStateSetPosition(Yertle, wposition);
}

//

float
CGWTurtleStateGetAngle(
    CGWTurtleRef    Yertle
)
{
    return Yertle->angle;
}
CGWTurtleRef
CGWTurtleStateSetAngle(
    CGWTurtleRef    Yertle,
    float           theta
)
{
    theta = CGWPointPThetaClampWithThresholds(theta, Yertle->theta_min_x, Yertle->theta_min_y);

    if ( Yertle->event_observers[kCGWTurtleEventIdSetAngle].observer ) {
        CGWTurtleEvent  event = {
                .from_turtle = Yertle,
                .event_id = kCGWTurtleEventIdSetAngle,
                .event_data.set_angle.theta = theta };
        int             result = Yertle->event_observers[kCGWTurtleEventIdSetAngle].observer(&event,
                                        Yertle->event_observers[kCGWTurtleEventIdSetAngle].context);
        switch ( result ) {
            case kCGWTurtleEventSetReturnCodeDoNotAlter:
                return Yertle;
            case kCGWTurtleEventSetAlteredValue:
                theta = event.event_data.set_angle.theta;
                break;
            default:
                break;
        }
    }
    
    Yertle->angle = theta;

    if ( CGWTurtleShouldLog(Yertle->flags) ) {
        fprintf(stderr,
            "CGWTurtle@%p[%s]:  angle set to %g (%g deg)\n",
            Yertle, Yertle->name, Yertle->angle, CGWRadiansToDegrees(Yertle->angle));
    }
    return Yertle;
}

//

CGWTurtleRef
CGWTurtleStateMove(
    CGWTurtleRef    Yertle,
    float           dposition
)
{
    // We can easily get the vector for movement by
    // making a polar point and converting it to
    // Cartesian:
    CGWPointP       pv;
    CGWPoint        cv, new_pos;
    
    if ( dposition < 0 ) {
        pv = CGWPointPMake(-dposition, Yertle->angle + M_PI);
    } else {
        pv = CGWPointPMake(dposition, Yertle->angle);
    }
    cv = CGWPointPToCartesian(pv, kCGWCoordConvRoundModeRound);
    new_pos = CGWPointSum(Yertle->position.cartesian, cv);
    return CGWTurtleStateSetPosition(Yertle, new_pos);
}

//

CGWTurtleRef
CGWTurtleStateTurn(
    CGWTurtleRef    Yertle,
    float           dtheta
)
{
    return CGWTurtleStateSetAngle(Yertle, Yertle->angle + dtheta);
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
__CGWTurtleEmitLine(
    CGWTurtleRef        Yertle,
    CGWPoint            p0,
    CGWPoint            p1
)
{
    printf("EMIT LINE (%d, %d) -> (%d, %d)\n", p0.x, p0.y, p1.x, p1.y);
    // If there is no drawing callback, there's no need to actually
    // walk the points:
    if ( Yertle->callbacks.draw_pixel ) {
        int                 dx = p1.x - p0.x,
                            dy = p1.y - p0.y;
        
        // If dy = 0, it's a horizontal line and we just move through
        // each coordinate p0.x through p1.x:
        if ( dy == 0 ) {
            int             npts = (dx < 0) ? -dx : dx;
            
            dx = (dx < 0) ? -1 : 1;
            while ( npts-- ) {
                p0.x += dx;
                Yertle->callbacks.draw_pixel(Yertle, p0, Yertle->callbacks.context);
            }
        }
        // If dx = 0, it's a vertical line and we just move through
        // each coordinate p0.y through p1.y:
        else if ( dx == 0 ) {
            dy = (dy < 0) ? -1 : 1;
            while ( (p0.y += dy) != p1.y )
                Yertle->callbacks.draw_pixel(Yertle, p0, Yertle->callbacks.context);
        }
        else {
            // The turtle needs to move dx pixels horizontally and dy pixels
            // vertically.  The one with the smaller absolute value will be
            // placed in the numerator of the slope and the x- or y-direction
            // traversed in relation to that slope.
            int         abs_dx = ((dx < 0) ? -dx : dx),
                        abs_dy = ((dy < 0) ? -dy : dy);
                        
            if ( abs_dx > abs_dy ) {
                // Y is smaller:
                double          y = (double)p0.y, m = (double)dy / (double)dx;
                
                dx = (dx < 0) ? -1 : 1;
                while ( abs_dx-- ) {
                    p0.x += dx;
                    p0.y = round(y += m);
                    Yertle->callbacks.draw_pixel(Yertle, p0, Yertle->callbacks.context);
                }
            } else {
                // X is smaller:
                double          x = (double)p0.x, m = (double)dx / (double)dy;
                
                dy = (dy < 0) ? -1 : 1;
                while ( abs_dy-- ) {
                    p0.y += dy;
                    p0.x = round(x += m);
                    Yertle->callbacks.draw_pixel(Yertle, p0, Yertle->callbacks.context);
                }
            }
        }
    }
    printf("DONE LINE\n");
    Yertle->position.cartesian = p1;
    Yertle->position.polar = CGWPointToPolar(p1, kCGWCoordConvRoundModeRound);
}

//

#define CGWBeginEvent(T)    if ( Yertle->event_observers[kCGWTurtleEventPathFinderStep].observer ) { \
                                event.from_turtle = Yertle, \
                                event.event_id = kCGWTurtleEventPathFinderStep, \
                                event.event_data.path_finder.step_type = (T);
#define CGWEndEvent()           Yertle->event_observers[kCGWTurtleEventPathFinderStep].observer(&event, \
                                        Yertle->event_observers[kCGWTurtleEventPathFinderStep].context); \
                            }


void
__CGWTurtleGeneratePath(
    CGWTurtleRef        Yertle,
    CGWPoint            destination
)
{
    CGWPoint            current_loc = Yertle->position.cartesian;
    CGWTurtleEdgeDetect edge_found;
    bool                is_path_complete = CGWPointEqual(current_loc, destination);
    float               angle;
    CGWTurtleEvent      event;
    
    CGWBeginEvent(kCGWTurtlePathFinderStepBegin);
        event.event_data.path_finder.step_data.begin.start = current_loc;
        event.event_data.path_finder.step_data.begin.destination = destination;
    CGWEndEvent();
    if ( Yertle->callbacks.draw_pixel ) 
        Yertle->callbacks.draw_pixel(Yertle, current_loc, Yertle->callbacks.context);
    
    while ( ! is_path_complete ) {
        // Is the destination inside the bounds?
        angle = __CGWTurtleAngleForPoints(current_loc, destination);
        if ( CGWRectContainsPoint(Yertle->bounds, destination) ) {
            //
            // Emit line segment current_loc -> destination
            //
            CGWBeginEvent(kCGWTurtlePathFinderStepLine);
                event.event_data.path_finder.step_data.line.start = current_loc;
                event.event_data.path_finder.step_data.line.end = destination;
            CGWEndEvent();
            
            __CGWTurtleEmitLine(Yertle, current_loc, destination);
            current_loc = destination;
            is_path_complete = true;
        } else {
            CGWPoint    bounds_hit = __CGWTurtleClampPointToBoundsEdges(
                                                &Yertle->bounds,
                                                &current_loc,
                                                &destination,
                                                &edge_found);
            //
            // Emit line segment current_loc -> bounds_hit
            // Yertle->position and current_loc should be at
            // bounds_hit after this procedure
            //
            CGWBeginEvent(kCGWTurtlePathFinderStepLine);
                event.event_data.path_finder.step_data.line.start = current_loc;
                event.event_data.path_finder.step_data.line.end = bounds_hit;
            CGWEndEvent();
            
            __CGWTurtleEmitLine(Yertle, current_loc, bounds_hit);
            current_loc = bounds_hit;
            
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
                            event.event_data.path_finder.step_data.hit_wall.hit_point = current_loc;
                        CGWEndEvent();
                    } else {
                        CGWBeginEvent(kCGWTurtlePathFinderStepHitRightWall);
                            event.event_data.path_finder.step_data.hit_wall.hit_point = current_loc;
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
                                    destination.y = -Yertle->bounds.origin.y;
                                } else {
                                    angle = -M_PI_2;
                                    destination.y = Yertle->bounds.origin.y;
                                }
                                destination.x = current_loc.x;
                                
                                CGWBeginEvent((edge_found == kCGWTurtleEdgeDetectLeft) ? kCGWTurtlePathFinderSlideOnLeftWall : kCGWTurtlePathFinderSlideOnRightWall)
                                    event.event_data.path_finder.step_data.slide_on_wall.hit_point = bounds_hit;
                                    event.event_data.path_finder.step_data.slide_on_wall.angle = angle;
                                    event.event_data.path_finder.step_data.slide_on_wall.new_destination = destination;
                                CGWEndEvent();
                            }
                            break;
                        }
                        case kCGWTurtleOptionsXBoundsPeriodic: {
                            // Angle of movement is preserved, turtle
                            // teleports to the opposite edge; the
                            // destination needs to be translated the
                            // same distance:
                            destination.x -= 2.0 * current_loc.x;
                            current_loc.x = -current_loc.x;
                            
                            CGWBeginEvent(kCGWTurtlePathFinderStepTeleport);
                                event.event_data.path_finder.step_data.teleport.hit_point = bounds_hit;
                                event.event_data.path_finder.step_data.teleport.teleport_to = current_loc;
                                event.event_data.path_finder.step_data.teleport.new_destination = destination;
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
                                event.event_data.path_finder.step_data.reflect.hit_point = bounds_hit;
                                event.event_data.path_finder.step_data.reflect.incident_angle = orig_angle;
                                event.event_data.path_finder.step_data.reflect.reflected_angle = angle;
                                event.event_data.path_finder.step_data.reflect.new_destination = destination;
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
                            event.event_data.path_finder.step_data.hit_wall.hit_point = current_loc;
                        CGWEndEvent();
                    } else {
                        CGWBeginEvent(kCGWTurtlePathFinderStepHitBottomWall);
                            event.event_data.path_finder.step_data.hit_wall.hit_point = current_loc;
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
                                    destination.x = Yertle->bounds.origin.x;
                                } else {
                                    angle = 0;
                                    destination.x = -Yertle->bounds.origin.x;
                                }
                                destination.y = current_loc.y;
                                
                                CGWBeginEvent((edge_found == kCGWTurtleEdgeDetectTop) ? kCGWTurtlePathFinderSlideOnTopWall : kCGWTurtlePathFinderSlideOnBottomWall)
                                    event.event_data.path_finder.step_data.slide_on_wall.hit_point = bounds_hit;
                                    event.event_data.path_finder.step_data.slide_on_wall.angle = angle;
                                    event.event_data.path_finder.step_data.slide_on_wall.new_destination = destination;
                                CGWEndEvent();
                            }
                            break;
                        }
                        case kCGWTurtleOptionsYBoundsPeriodic: {
                            // Angle of movement is preserved, turtle
                            // teleports to the opposite edge; the
                            // destination needs to be translated the
                            // same distance:
                            destination.y -= 2.0 * current_loc.y;
                            current_loc.y = -current_loc.y;
                            
                            CGWBeginEvent(kCGWTurtlePathFinderStepTeleport);
                                event.event_data.path_finder.step_data.teleport.hit_point = bounds_hit;
                                event.event_data.path_finder.step_data.teleport.teleport_to = current_loc;
                                event.event_data.path_finder.step_data.teleport.new_destination = destination;
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
                                event.event_data.path_finder.step_data.reflect.hit_point = bounds_hit;
                                event.event_data.path_finder.step_data.reflect.incident_angle = orig_angle;
                                event.event_data.path_finder.step_data.reflect.reflected_angle = angle;
                                event.event_data.path_finder.step_data.reflect.new_destination = destination;
                            CGWEndEvent();
                            break;
                        }
                    }
                }
            }
        }
    }
    CGWTurtleStateSetPosition(Yertle, destination);
    CGWTurtleStateSetAngle(Yertle, angle);
    CGWBeginEvent(kCGWTurtlePathFinderStepEnd);
        event.event_data.path_finder.step_data.end.end = current_loc;
    CGWEndEvent();
}

#undef CGWBeginEvent
#undef CGWEndEvent

//

void
CGWTurtleActionMove(
    CGWTurtleRef    Yertle,
    float           dposition
)
{
    CGWPoint        destination = {
                        .x = Yertle->position.cartesian.x + round(dposition * cos(Yertle->angle)),
                        .y = Yertle->position.cartesian.y + round(dposition * sin(Yertle->angle)) };
    __CGWTurtleGeneratePath(Yertle, destination);
}

//

void
CGWTurtleActionMoveTo(
    CGWTurtleRef    Yertle,
    CGWPoint        position
)
{
    __CGWTurtleGeneratePath(Yertle, position);
}









