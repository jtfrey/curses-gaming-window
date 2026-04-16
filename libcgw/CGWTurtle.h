/**
 * @file CGWTurtle.h
 * @brief Curses Gaming Window, Turtle graphics
 * 
 * A header file that defines an API for turtle graphics.
 *
 */

#ifndef __CGWTURTLEH__
#define __CGWTURTLEH__

#include "CGWGeom.h"
#include "CGWMath.h"
#include "CGWBitvector.h"

/**
 * User-facing operational options for \ref CGWTurtle objects
 * Each instance of a \ref CGWTurtle can behave in different ways.
 *
 * For example, to enable periodic boundary conditions
 */
enum {
    kCGWTurtleOptionsXBoundsHardWall        = 0,                /*!< The left-right boundaries behave like a hard
                                                                     wall -- when the turtle reaches the wall, it
                                                                     does not alter direction and just slides along
                                                                     the boundary */
    kCGWTurtleOptionsXBoundsPeriodic        = 1,               /*!< The left-right boundaries behave like they
                                                                     are connected -- when the turtle reaches the
                                                                     wall, it reappears on the opposite side of
                                                                     the bounds */
    kCGWTurtleOptionsXBoundsElastic         = 2,               /*!< The left-right boundaries embody elastic rebound
                                                                     behavior -- when the turtle reaches the wall,
                                                                     its angle of impact is reflected such that the
                                                                     turtle "bounces" off the wall */
    kCGWTurtleOptionsXBoundsMask            = 0b11,             /*!< Bitmask that demarcates the left-right
                                                                     boundary behavior bits */
    
    kCGWTurtleOptionsYBoundsHardWall        = 0,                /*!< The top-bottom boundaries behave like a hard
                                                                     wall -- when the turtle reaches the wall, it
                                                                     does not alter direction and just slides along
                                                                     the boundary */
    kCGWTurtleOptionsYBoundsPeriodic        = 1 << 2,          /*!< The top-bottom boundaries behave like they
                                                                     are connected -- when the turtle reaches the
                                                                     wall, it reappears on the opposite side of
                                                                     the bounds */
    kCGWTurtleOptionsYBoundsElastic         = 2 << 2,          /*!< The top-bottom boundaries embody elastic rebound
                                                                     behavior -- when the turtle reaches the wall,
                                                                     its angle of impact is reflected such that the
                                                                     turtle "bounces" off the wall */
    kCGWTurtleOptionsYBoundsMask            = 0b1100,          /*!< Bitmask that demarcates the top-bottom
                                                                     boundary behavior bits */
    
    kCGWTurtleOptionsBoundsPeriodic         = kCGWTurtleOptionsXBoundsPeriodic 
                                                | kCGWTurtleOptionsYBoundsPeriodic, /*!< Periodic boundary for all walls */
    kCGWTurtleOptionsBoundsElastic          = kCGWTurtleOptionsXBoundsElastic
                                                | kCGWTurtleOptionsYBoundsElastic,  /*!< Elastic boundary for all walls */
    kCGWTurtleOptionsBoundsMask             = kCGWTurtleOptionsXBoundsMask
                                                | kCGWTurtleOptionsYBoundsMask,     /*!< Mask for all boundary bits */
    
    kGGWTurtleOptionsEnableLogging          = 4,                /*!< This bit index is used to enable logging of
                                                                     all actions and state changes to the turtle */
    kGGWTurtleOptionsEnableLoggingMask      = CGWBitvectorMask(kGGWTurtleOptionsEnableLogging), /*!< Bit mask for \ref kGGWTurtleOptionsEnableLogging */
    
    
    kCGWTurtleOptionsMask                   = kCGWTurtleOptionsBoundsMask      /*!< Bitmask that isolates all user-facing options */
                                                | kGGWTurtleOptionsEnableLoggingMask
};

/**
 * A reference to a CGWTurtle object
 * The type of a reference to a dynamically-allocated CGWTurtle
 * object.
 */
typedef struct CGWTurtle * CGWTurtleRef;

/**
 * Pointer to a function called to draw a pixel
 * When a turtle moves with its stylus down, pixels are generated.  This API does not
 * do any drawing itself:  instead, it calls a function of this type to display the
 * pixels it generates.
 * @param Yertle        the \ref CGWTurtle generating the pixel
 * @param pixel         the location (in world coordinates) of the pixel
 * @param context       user-provided opaque pointer that the callback can use
 *                      for its own data w.r.t. drawing
 */
typedef void (*CGWTurtleDrawPixelCallback)(CGWTurtleRef Yertle, CGWPoint pixel, const void *context);

/**
 * Pointer to a function that converts turtle coordinates to world coordinate
 * When the turtle generates pixels, it communicates coordinates to the drawing
 * callback in world coordinates -- the coordinate system of the drawing context.
 * A function of this type converts turtle coordinates to a different
 * coordinate system.
 * @param Yertle        the \ref CGWTurtle object
 * @param turtle_coord  the location in turtle coordinates
 * @param context       user-provided opaque pointer that the callback can use
 *                      for its own data w.r.t. drawing
 * @return              the location in world coordinates
 */
typedef CGWPoint (*CGWTurtleCoordToWorldCallback)(CGWTurtleRef Yertle, CGWPoint turtle_coord, const void *context);

/**
 * Pointer to a function that converts world coordinates to turtle coordinate
 * Coordinates in the drawing context are converted to the turtle's
 * coordinate system using a function of this type.
 * @param Yertle        the \ref CGWTurtle object
 * @param world_coord   the location in world coordinates
 * @param context       user-provided opaque pointer that the callback can use
 *                      for its own data w.r.t. drawing
 * @return              the location in turtle coordinates
 */
typedef CGWPoint (*CGWTurtleCoordFromWorldCallback)(CGWTurtleRef Yertle, CGWPoint world_coord, const void *context);

/**
 * A set of callback functions to a CGWTurtle object
 * Setting any member field of this structure to NULL indicates the absence
 * of a callback function.
 */
typedef struct {
    const void                          *context;           /*!< an opaque pointer that will be passed to the callback functions */
    CGWTurtleDrawPixelCallback          draw_pixel;         /*!< callback that draws a generated pixel */
    CGWTurtleCoordToWorldCallback       turtle_to_world;    /*!< callback that converts turtle coordinates to world coordinates */
    CGWTurtleCoordFromWorldCallback     world_to_turtle;    /*!< callback that converts world coordinates to turtle coordinates */
} CGWTurtleCallbacks;

/**
 * Default turtle-to-world coordinate converter
 * The turtle has a bounds that is a symmetric Cartesian region with the
 * origin at the center.  Since the assumption is that the turtle pixels
 * will be displayed to a screen whose origin is at the top-left, this
 * callback converts turtle coordinates to that scheme.
 */
const CGWTurtleCoordToWorldCallback CGWTurtleDefaultCoordToWorldCallback;

/**
 * Default world-to-turtle coordinate converter
 * The turtle has a bounds that is a symmetric Cartesian region with the
 * origin at the center.  Since the assumption is that the turtle pixels
 * will be displayed to a screen whose origin is at the top-left, this
 * callback converts world coordinates in that scheme to the turtle's
 * standard Cartesian scheme.
 */
const CGWTurtleCoordFromWorldCallback CGWTurtleDefaultCoordFromWorldCallback;

/**
 * Default non-NULL callbacks
 * This set of callbacks includes the default coordinate transformation
 * callbacks.  It can easily be augmented by copying it:
 *
 *      CGWTurtleCallbacks  myCallbacks = *CGWTurtleDefaultCallbacks;
 *
 *      myCallbacks.context = "this is my supporting data";
 */
const CGWTurtleCallbacks *CGWTurtleDefaultCallbacks;

/**
 * Observable events generated by a turtle
 * As a turtle's state is changed, events are generated which can be
 * observed by user code.  Some events allow the observer to make
 * alterations to the proposed state change.
 *
 * kCGWTurtleEventIdWillDestroy
 *     The observer can return zero (0) to indicate that the object
 *     should NOT be destroyed, non-zero to destroy it.
 *
 * kCGWTurtleEventIdStylusStateChange
 *      The is_on field in event_date indicates the proposed new
 *      state of the style; the observer can return zero (0) to
 *      reject the state change, non-zero to accept it.
 *
 * kCGWTurtleEventIdSetPosition
 *      The cartesian and polar fields contain the proposed new
 *      coordinate for the turtle's location.  The observer should
 *      return kCGWTurtleEventSetReturnCodeOk to accept it;
 *      kCGWTurtleEventSetReturnCodeDoNotAlter to reject it; or
 *      kCGWTurtleEventSetAlteredValue to copy-back the value of
 *      the cartesian field ostensibly altered by the observer.
 *      Alterations to the polar coordinate are not copied-back.
 *
 * kCGWTurtleEventIdSetAngle
 *      The theta field contains the proposed new directional
 *      angle for the turtle.  The observer should return
 *      kCGWTurtleEventSetReturnCodeOk to accept it;
 *      kCGWTurtleEventSetReturnCodeDoNotAlter to reject it; or
 *      kCGWTurtleEventSetAlteredValue to copy-back the value of
 *      the theta field ostensibly altered by the observer.
 *
 * kCGWTurtleEventPathFinderStep
 *      When the turtle moves from one point to another to emit
 *      points for drawing, the "path finder" breaks the traversal
 *      into line segments that move through the bounds, hit its
 *      edges, and respond to boundary conditions.  Path-finder
 *      events are generated in that process; see the
 *      \ref CGWTurtlePathFinderStep enumeration for details on
 *      the steps that can be observed.
 */
typedef enum {
    kCGWTurtleEventIdSetBounds      = 0,
    kCGWTurtleEventIdWillDestroy,
    kCGWTurtleEventIdDidDestroy,
    kCGWTurtleEventIdStylusStateChange,
    kCGWTurtleEventIdSetPosition,
    kCGWTurtleEventIdSetAngle,
    kCGWTurtleEventPathFinderStep,
    kCGWTurtleEventIdMax
} CGWTurtleEventId;

/**
 * Return codes for observers that can alter proposed changes
 */
enum {
    kCGWTurtleEventSetReturnCodeOk = 0,     /*!< accept the change */
    kCGWTurtleEventSetReturnCodeDoNotAlter, /*!< reject the change */
    kCGWTurtleEventSetAlteredValue          /*!< observer is providing altered values for the change */
};

/**
 * Steps that will be emitted by the path-finder
 * When the turtle moves from one point to another to emit
 * points for drawing, the "path finder" breaks the traversal
 * into line segments that move through the bounds, hit its
 * edges, and respond to boundary conditions.  Path-finder
 * events are generated in that process.
 */
typedef enum {
    kCGWTurtlePathFinderStepBegin = 0,      /*!< the path-finder has been triggered */
    kCGWTurtlePathFinderStepLine,           /*!< a line from one point to another in the bounds has been generated */
    kCGWTurtlePathFinderStepHitLeftWall,    /*!< the left wall has been reached */
    kCGWTurtlePathFinderStepHitRightWall,   /*!< the right wall has been reached */
    kCGWTurtlePathFinderStepHitTopWall,     /*!< the top wall has been reached */
    kCGWTurtlePathFinderStepHitBottomWall,  /*!< the bottom wall has been reached */
    kCGWTurtlePathFinderStepTeleport,       /*!< the wall in question is periodic, so the turtle will teleport to the opposite wall */
    kCGWTurtlePathFinderStepReflect,        /*!< the wall in question is elastic, so the turtle will "bounce" off it */
    kCGWTurtlePathFinderSlideOnLeftWall,    /*!< the left wall is a hard wall; the turtle hit it at a non-orthogonal angle and will slide along it */
    kCGWTurtlePathFinderSlideOnRightWall,   /*!< the right wall is a hard wall; the turtle hit it at a non-orthogonal angle and will slide along it */
    kCGWTurtlePathFinderSlideOnTopWall,     /*!< the top wall is a hard wall; the turtle hit it at a non-orthogonal angle and will slide along it */
    kCGWTurtlePathFinderSlideOnBottomWall,  /*!< the bottom wall is a hard wall; the turtle hit it at a non-orthogonal angle and will slide along it */
    kCGWTurtlePathFinderStepEnd,            /*!< the path-finder has reached the destination */
    kCGWTurtlePathFinderStepMax             /*!< sentinel value, the number of step types enumerated */
} CGWTurtlePathFinderStep;

/**
 * Supporting data for a path-finder event
 * When a path-finder observer is sent an event, supporting data is provided
 * in a data structure of this type.
 */
typedef struct {
    CGWTurtlePathFinderStep     step_type;          /*!< the type of path-finder step generated */
    union {
        struct {
            CGWPoint            start;              /*!< the starting point of the path */
            CGWPoint            destination;        /*!< the destination to which the path is headed */
        } begin;                                    /*!< data associated with a kCGWTurtlePathFinderStepBegin event */
        struct {
            CGWPoint            start;              /*!< the starting point of the line segment */
            CGWPoint            end;                /*!< the ending point of the line segment */
        } line;                                     /*!< data associated with a kCGWTurtlePathFinderStepLine event */
        struct {
            CGWPoint            hit_point;          /*!< the point on the wall that has been reached */
        } hit_wall;                                 /*!< data associated with a kCGWTurtlePathFinderStepHit*Wall event */
        struct {
            CGWPoint            hit_point;          /*!< the point on the wall that has been reached */
            CGWPoint            teleport_to;        /*!< the point to which the turtle will teleport */
            CGWPoint            new_destination;    /*!< the revised destination point, adjusted for the teleportation */
        } teleport;                                 /*!< data associated with a kCGWTurtlePathFinderStepTeleport event */
        struct {
            CGWPoint            hit_point;          /*!< the point on the wall that has been reached */
            float               incident_angle;     /*!< the angle at which the turtle hit the wall */
            float               reflected_angle;    /*!< the angle at which the turtle will leave the wall */
            CGWPoint            new_destination;    /*!< the revised destination point, adjusted for the reflection */
        } reflect;                                  /*!< data associated with a kCGWTurtlePathFinderStepReflect event */
        struct {
            CGWPoint            hit_point;          /*!< the point on the wall that has been reached */
            float               angle;              /*!< the angle at which the turtle hit the wall */
            CGWPoint            new_destination;    /*!< the revised destination point, adjusted for the impact */
        } slide_on_wall;                            /*!< data associated with a kCGWTurtlePathFinderSlideOn*Wall event */
        struct {
            CGWPoint            end;                /*!< the final point the turtle has reached */
        } end;                                      /*!< data associated with a kCGWTurtlePathFinderStepEnd event */
    } step_data;
} CGWTurtlePathFinder;

/**
 * A turtle event
 * Data structure that provides data w.r.t. a turtle event that an 
 * observer wishes to...observe.
 *
 * The event_id field indicates the kind of event and determines
 * which data structure in the event_data union applies to this
 * event.
 */
typedef struct {
    CGWTurtleRef            from_turtle;        /*!< the CGWTurtle that generated the event */
    CGWTurtleEventId        event_id;           /*!< the event that was generated */
    union {
        struct {
            CGWBitvector    options;            /*!< the new boundary behaviors for the turtle */
            CGWSize         size;               /*!< the new region size for the turtle */
        } set_bounds;                           /*!< fields associated with kCGWTurtleEventIdSetBounds events */
        struct {
            bool            is_on;              /*!< the proposed new state of the stylus */
        } stylus_state_change;                  /*!< fields associated with kCGWTurtleEventIdStylusStateChange events */
        struct {
            CGWPoint        cartesian;          /*!< the proposed new Cartesian coordinate of the turtle */
            CGWPointP       polar;              /*!< the proposed new polar coordinate of the turtle */
        } set_position;                         /*!< fields associated with kCGWTurtleEventIdSetPosition events */
        struct {
            float           theta;              /*!< the proposed new orientation angle of the turtle (in radians) */
        } set_angle;                            /*!< fields associated with kCGWTurtleEventIdSetAngle events */
        CGWTurtlePathFinder path_finder;        /*!< data associated with the path-mapping algorithm */
    } event_data;
} CGWTurtleEvent;

/**
 * Pointer to event data
 * Type of a pointer to a CGWTurtleEvent data structure.
 */
typedef CGWTurtleEvent * CGWTurtleEventPtr;

/**
 * A turtle event observer
 * Type of a function that receives events from a CGWTurtle.  The int return
 * value will adopt different interpretations based on the event in question;
 * see \ref CGWTurtleEventId for information on each kind of event.
 * @param the_event     the event generated
 * @param context       the opaque pointer registered with the event
 *                      observer
 * @return              values dependent upon the event id
 */
typedef int (*CGWTurtleEventObserver)(CGWTurtleEventPtr the_event, const void *context);

/**
 * Allocate and initialize a new CGWTurtle object
 * Create a new turtle with the provided boundary dimensions, \p bounds, and
 * optional behaviors dictated by \p options.
 *
 * The callbacks at \p cbs are copied into the object.  If \p cbs is NULL,
 * all callbacks in the object will be unset (including the coordinate
 * transformations).
 *
 * The optional \p name is limited to 63 characters.  If \p name is NULL,
 * a default name is generated for the object (e.g. "turtle_12").  Take a
 * wild guess what the number represents.
 * @param name              the optional name of the turtle
 * @param bounds            the size of the region containing the turtle
 * @param options           optional behaviors (see the enumeration)
 * @param cbs               callbacks for the object; NULL implies no
 *                          callback functions will be used
 * @return                  the new CGWTurtle object, NULL on error
 */
CGWTurtleRef CGWTurtleCreate(const char *name, CGWSize bounds, CGWBitvector options, const CGWTurtleCallbacks *cbs);

/**
 * Destroy a CGWTurtle object
 * Deallocate and invalidate \p Yertle.
 * @param Yertle        the CGWTurtle object to destroy
 */
void CGWTurtleDestroy(CGWTurtleRef Yertle);

/**
 * Write a summary of the CGWTurtle object to stderr
 * Prints a technical description of \p Yertle to stderr.
 * @param Yertle        the CGWTurtle object to summarize
 */
void CGWTurtleSummarize(CGWTurtleRef Yertle);

 /**
 * Get a copy of a turtle's callbacks
 * Copies the callbacks associated with \p Yertle to the struct
 * that \p cbs points to.
 * @param Yertle        the CGWTurtle object
 * @param cbs           pointer to a CGWTurtleCallbacks
 */
void CGWTurtleGetCallbacks(CGWTurtleRef Yertle, CGWTurtleCallbacks *cbs);

/**
 * Set a turtle's callbacks
 * Overwrite the callbacks of \p Yertle with those in the \ref CGWTurtleCallbacks
 * that \p cbs points to.
 * @param Yertle        the CGWTurtle object
 * @param cbs           pointer to a CGWTurtleCallbacks
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleSetCallbacks(CGWTurtleRef Yertle, const CGWTurtleCallbacks *cbs);

/**
 * Get a turtle's callback context
 * Returns the opaque pointer that will be passed to all callbacks.
 * @param Yertle        the CGWTurtle object
 * @return              the opaque pointer associated with \p Yertle
 */
const void* CGWTurtleGetCallbackContext(CGWTurtleRef Yertle);

/**
 * Set a turtle's callback context
 * Sets the opaque pointer that will be passed to all callbacks.
 * @param Yertle        the CGWTurtle object
 * @param cb_context    the opaque pointer associated with \p Yertle
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleSetCallbackContext(CGWTurtleRef Yertle, const void *cb_context);

/**
 * Get a turtle's pixel-drawing callback
 * Returns the CGWTurtleDrawPixelCallback associated with \p Yertle
 * @param Yertle        the CGWTurtle object
 * @return              the CGWTurtleDrawPixelCallback
 */
CGWTurtleDrawPixelCallback CGWTurtleGetDrawPixelCallback(CGWTurtleRef Yertle);

/**
 * Set a turtle's pixel-drawing callback
 * Sets the CGWTurtleDrawPixelCallback associated with \p Yertle
 * @param Yertle        the CGWTurtle object
 * @param cb            a CGWTurtleDrawPixelCallback (or NULL)
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleSetDrawPixelCallback(CGWTurtleRef Yertle, CGWTurtleDrawPixelCallback cb);

/**
 * Get a turtle's turtle-to-world coordinate transform callback
 * Returns the CGWTurtleCoordToWorldCallback associated with \p Yertle
 * @param Yertle        the CGWTurtle object
 * @return              the CGWTurtleCoordToWorldCallback
 */
CGWTurtleCoordToWorldCallback CGWTurtleGetCoordToWorldCallback(CGWTurtleRef Yertle);

/**
 * Set a turtle's turtle-to-world coordinate transform callback
 * Sets the CGWTurtleCoordToWorldCallback associated with \p Yertle
 * @param Yertle        the CGWTurtle object
 * @param cb            a CGWTurtleCoordToWorldCallback (or NULL)
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleSetCoordToWorldCallback(CGWTurtleRef Yertle, CGWTurtleCoordToWorldCallback cb);

/**
 * Get a turtle's world-to-turtle coordinate transform callback
 * Returns the CGWTurtleCoordFromWorldCallback associated with \p Yertle
 * @param Yertle        the CGWTurtle object
 * @return              the CGWTurtleCoordFromWorldCallback
 */
CGWTurtleCoordFromWorldCallback CGWTurtleGetCoordFromWorldCallback(CGWTurtleRef Yertle);

/**
 * Set a turtle's world-to-turtle coordinate transform callback
 * Sets the CGWTurtleCoordFromWorldCallback associated with \p Yertle
 * @param Yertle        the CGWTurtle object
 * @param cb            a CGWTurtleCoordFromWorldCallback (or NULL)
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleSetCoordFromWorldCallback(CGWTurtleRef Yertle, CGWTurtleCoordFromWorldCallback cb);

/**
 * Reset all internal state for a turtle
 * The internal state of \p Yertle is reset.  The turtle returns to its default
 * starting position and angle.
 * @param Yertle        the CGWTurtle object
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateReset(CGWTurtleRef Yertle);

/**
 * Reset all internal state for a turtle with changes to its behavior
 * The internal state of \p Yertle is reset and the optional behaviors and bounds
 * are modified.  The turtle returns to its default starting position and angle.
 * @param Yertle        the CGWTurtle object
 * @param bounds        the new boundary size for \p Yertle
 * @param options       the new optional behaviors for \p Yertle
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateResetWithChanges(CGWTurtleRef Yertle, CGWSize bounds, CGWBitvector options);

/**
 * Get the stylus state
 * @param Yertle        the CGWTurtle object
 * @return              boolean true if the stylus is on, false otherwise
 */
bool CGWTurtleStateGetStylusIsOn(CGWTurtleRef Yertle);

/**
 * Set the stylus state
 * @param Yertle        the CGWTurtle object
 * @param is_on         boolean true to turn the stylus on, false for off
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateSetStylusIsOn(CGWTurtleRef Yertle, bool is_on);

/**
 * Toggle the stylus state
 * The state of the stylus is flipped (on -> off, off -> on)
 * @param Yertle        the CGWTurtle object
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateToggleStylusIsOn(CGWTurtleRef Yertle);

/**
 * Get the Cartesian position of the turtle
 * @param Yertle        the CGWTurtle object
 * @return              the Cartesian location of \p Yertle
 */
CGWPoint CGWTurtleStateGetPosition(CGWTurtleRef Yertle);

/**
 * Set the Cartesian position of the turtle
 * Any boundary behaviors for \p Yertle are applied to \p position.
 * @param Yertle        the CGWTurtle object
 * @param position      the Cartesian location of \p Yertle
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateSetPosition(CGWTurtleRef Yertle, CGWPoint position);

/**
 * Get the polar position of the turtle
 * @param Yertle        the CGWTurtle object
 * @return              the polar location of \p Yertle
 */
CGWPointP CGWTurtleStateGetPolarPosition(CGWTurtleRef Yertle);

/**
 * Set the polar position of the turtle
 * Any boundary behaviors for \p Yertle are applied to \p position.
 * @param Yertle        the CGWTurtle object
 * @param position      the polar location of \p Yertle
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateSetPolarPosition(CGWTurtleRef Yertle, CGWPointP position);

/**
 * Get the world-coordinates Cartesian position of the turtle
 * @param Yertle        the CGWTurtle object
 * @return              the Cartesian location of \p Yertle in world
 *                      coordinates
 */
CGWPoint CGWTurtleStateGetWorldPosition(CGWTurtleRef Yertle);

/**
 * Set the world-coordinates Cartesian position of the turtle
 * Any boundary behaviors for \p Yertle are applied to \p wposition.
 * @param Yertle        the CGWTurtle object
 * @param wposition     the Cartesian location of \p Yertle in world
 *                      coordinates
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateSetWorldPosition(CGWTurtleRef Yertle, CGWPoint wposition);

/**
 * Get the angle of the turtle
 * @param Yertle        the CGWTurtle object
 * @return              the angle of \p Yertle
 */
float CGWTurtleStateGetAngle(CGWTurtleRef Yertle);

/**
 * Set the angle of the turtle
 * Any range-corrections are applied to \p theta.
 * @param Yertle        the CGWTurtle object
 * @param theta         the angle (in radians) of \p Yertle
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateSetAngle(CGWTurtleRef Yertle, float theta);

/**
 * Move the turtle relative to its current position
 * Given the current position and angle of \p Yertle, move the turtle
 * the distance \p dposition.
 *
 * Any boundary behaviors are applied to the movement.
 * @param Yertle        the CGWTurtle object
 * @param dposition     the distance to move; if negative, the
 *                      movement is in the opposite direction of
 *                      the angle but the angle remains unchanged
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateMove(CGWTurtleRef Yertle, float dposition);

/**
 * Augment the turtle's existing angle
 * Adds \p dtheta to the current angle of \p Yertle.  Any range-corrections
 * are applied.
 * @param Yertle        the CGWTurtle object
 * @param dtheta        the addition (in radians) to make to the
 *                      angle of \p Yertle
 * @return              returns the reference to the CGWTurtle (for call-chaining)
 */
CGWTurtleRef CGWTurtleStateTurn(CGWTurtleRef Yertle, float dtheta);

/**
 * Get a turtle's event observer for an event
 * Returns the \ref CGWTurtleEventObserver that is registered with \p Yertle to
 * observe events of type \p event_id.
 * @param Yertle        the CGWTurtle object
 * @param event_id      the type of event
 * @return              the registered \ref CGWTurtleEventObserver or NULL if no
 *                      observer is registered for \p event_id
 */
CGWTurtleEventObserver CGWTurtleGetEventObserver(CGWTurtleRef Yertle, CGWTurtleEventId event_id);

/**
 * Set a turtle's event observer for an event
 * Sets the \ref CGWTurtleEventObserver to which \p Yertle will deliver events
 * of type \p event_id.  The \p context will be passed to the observer as an
 * argument.
 *
 * Pass NULL for \p observer to unregister a previously-registered observer.
 * @param Yertle        the CGWTurtle object
 * @param event_id      the type of event
 * @param observer      the observer function pointer or NULL to unregister the event
 *                      type
 * @param context       an opaque pointer passed to the event observer; can be used
 *                      to provide supporting data to the observer
 */
void CGWTurtleSetEventObserver(CGWTurtleRef Yertle, CGWTurtleEventId event_id, CGWTurtleEventObserver observer, const void *context);




void CGWTurtleActionMove(CGWTurtleRef Yertle, float dposition);
void CGWTurtleActionMoveTo(CGWTurtleRef Yertle, CGWPoint position);


#endif /* __CGWTURTLEH__ */
