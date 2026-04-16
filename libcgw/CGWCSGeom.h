/**
 * @file CGWCSGeom.h
 * @brief Curses Gaming Window, Constructive solid geometry support
 * 
 * Types and functions that implement arbitrary shapes as
 * constructive solid geometry lists.
 *
 */

#ifndef __CGWCSGEOM_H__
#define __CGWCSGEOM_H__

#include "CGWGeom.h"

/**
 * Enumeration of fundamental shape types
 * A CSG object is composed of one or more fundamental geometry
 * figures.  This type enumerates the kinds of figures.
 */
typedef enum {
    kCGWCSGeomShapeTypeOptDisabled = -2,    /*!< shape type used to indicate that the optimizer
                                                 disabled this shape */
    kCGWCSGeomShapeTypeEndOfList = -1,      /*!< shape type used to indicate the end of a list of
                                                 shape definitions */
    kCGWCSGeomShapeTypePoint = 0,           /*!< a 2D point */
    kCGWCSGeomShapeTypeRect,                /*!< a 2D rectangle */
    kCGWCSGeomShapeTypeCircle,              /*!< a 2D circle */
    kCGWCSGeomShapeTypeMax                  /*!< the number of types present */
} CGWCSGeomShapeType;

/**
 * A fundamental shape
 * Structure that is used to represent a fundamental shape in a
 * CSG list of component shapes.
 */
typedef struct {
    CGWCSGeomShapeType      type;       /*!< the shape type */
    union {
        CGWPoint            p;          /*!< fields associated with a 2D point shape */
        CGWRect             r;          /*!< fields associated with a 2D rectangle shape */
        CGWCircle           c;          /*!< fields associated with a 2D circle shape */
    } shape;                            /*!< union of all shape type data structures */
} CGWCSGeomShape;


bool CGWCSGeomShapeOptimizeArray(unsigned int n_shapes, CGWCSGeomShape *shapes);




/**
 * A CSG shape
 * A constructive solid geometry shape is an array of one or more
 * fundamental shapes.  For example, a capsule could be composed from
 * two circles bridged by a rectangle:
 *
 *     { o=(-1, 0), r=1 } + { tl=(-1, -1), br=(1, 1) } + { o=(1, 0), r=1 }
 *                _ __ _
 *               / |  | \
 *              |  |  |  |
 *               \_|__|_/
 *
 * The \ref CGWCSGeomStructStart() macros can be used to initialize a
 * static instance of the above at compile time:
 *
 *     static CGWCSGeom capsule = 
 *                 CGWCSGeomStructStart(3)
 *                     CGWCSGeomStructAddCircle(-1, 0, 1)
 *                     GGWCSGeomStructAddRect(-1, -1, 2, 2)
 *                     CGWCSGeomStructAddCircle( 1, 0, 1)
 *                 CGWCSGeomStructEnd();
 *               
 */
typedef struct {
    unsigned int        n_shapes;       /*!< the number of fundamental shapes in the array */                              
    CGWCSGeomShape      shapes[];       /*!< the array of fundamental shapes */
} CGWCSGeom;
/**
 * Reference to a CSG shape
 * Since CSG shapes are usually dynamically-allocated, a
 * reference (pointer) is used when working with it.
 */
typedef const CGWCSGeom * CGWCSGeomRef;

/**
 * An empty constructive solid geometry object
 * A constant CGWCSGeom object that includes zero fundamental
 * shapes.
 */
extern CGWCSGeomRef CGWCSGeomEmpty;

/**
 * Create a CGWCSGeom from a list of fundamental shapes
 * The fundamental shapes list at \p shapes_and_eol is used to create a
 * new constructive solid geometry shape.  The list MUST be terminated by
 * an element with type kCGWCSGeomShapeTypeEndOfList.
 * @param shapes_and_eol        pointer to a specially-terminated list
 *                              of fundamental shapes
 * @return                      the new CGWCSGeom object if successful,
 *                              NULL otherwise
 */
CGWCSGeomRef CGWCSGeomCreateWithShapesList(CGWCSGeomShape *shapes_and_eol);

/**
 * Create a CGWCSGeom from an array of fundamental shapes
 * The fundamental shapes array of dimension \p n_shapes  at \p shapes is
 * used to create a new constructive solid geometry shape.
 * @param n_shapes              the number of shapes in the array
 * @param shapes_and_eol        pointer to the array of shapes
 * @return                      the new CGWCSGeom object if successful,
 *                              NULL otherwise
 */
CGWCSGeomRef CGWCSGeomCreateWithShapesArray(unsigned int n_shapes, CGWCSGeomShape *shapes);

/**
 * Deallocate a CGWCSGeom
 * Dispose of all memory occupied by \p cgs_obj and invalidate the
 * object.
 * @param cgs_obj       the CGS object to destroy
 */
void CGWCSGeomDestroy(CGWCSGeomRef cgs_obj);

/**
 * Combine the fundamental shapes from two CSG shapes
 * Creates a new CGWCSGeom object that contains the fundamental shapes
 * of both \p cgs_obj1 and \p cgs_obj2.
 *
 * Note that this does not optimize the resulting fundamental shape
 * list to remove redundancies, e.g. if a 2D rect is completely contained
 * by another shape.
 * @param csg_obj1          first CGWCSGeom object
 * @param csg_obj2          second CGWCSGeom object
 * @return                  combined CGWCSGeom object
 */
CGWCSGeomRef CGWCSGeomCombine(CGWCSGeomRef cgs_obj1, CGWCSGeomRef cgs_obj2);

/**
 * Start a CGWCSGeom struct initialization block
 * Macro that emits the data structure initialization statements
 * for a CGWCSGeom with \p N_SHAPES in its array.
 * @param N_SHAPES      the number of fundamental shapes that will
 *                      be declared
 */
#define CGWCSGeomStructStart(N_SHAPES) \
            { .n_shapes = (N_SHAPES), .shapes = {
/**
 * Add a CGWPoint to the fundamental shape array
 * Used in conjunction with a \ref CGWCSGeomStructStart() block to
 * add a 2D point to the fundamental shape array.
 * @param X             the x-coordinate
 * @param Y             the y-coordinate
 */
#define CGWCSGeomStructAddPoint(X, Y)   \
                { .type = kCGWCSGeomShapeTypePoint, \
                  .shape.p.x = (X), .shape.p.y = (Y) },
/**
 * Add a CGWRect to the fundamental shape array
 * Used in conjunction with a \ref CGWCSGeomStructStart() block to
 * add a 2D rectangle to the fundamental shape array.
 * @param X             the origin x-coordinate
 * @param Y             the origin y-coordinate
 * @param W             the width in the x-dimension
 * @param H             the height in the y-dimension
 */        
#define CGWCSGeomStructAddRect(X, Y, W, H)   \
                { .type = kCGWCSGeomShapeTypeRect, \
                  .shape.r.origin.x = (X), .shape.r.origin.y = (Y) , \
                  .shape.r.size.w = (W), .shape.r.size.h = (H) },
/**
 * Add a CGWCircle to the fundamental shape array
 * Used in conjunction with a \ref CGWCSGeomStructStart() block to
 * add a 2D circle to the fundamental shape array.
 * @param X             the origin x-coordinate
 * @param Y             the origin y-coordinate
 * @param R             the radius
 */       
#define CGWCSGeomStructAddCircle(X, Y, R)   \
                { .type = kCGWCSGeomShapeTypeCircle, \
                  .shape.c.origin.x = (X), .shape.c.origin.y = (Y) , \
                  .shape.c.radius = (R) },
/**
 * Add an explicit end-of-list shape entry
 * Macro that emits an end-of-list CGWCSGeomShape initializer.
 */
#define CGWCSGeomStructAddEndOfList() { .type = kCGWCSGeomShapeTypeEndOfList }

/**
 * End a CGWCSGeom struct initialization block
 * Macro that emits the final bits of a data structure initialization
 * for a CGWCSGeom.
 */
#define CGWCSGeomStructEnd() \
                { .type = kCGWCSGeomShapeTypeEndOfList } \
            } }

/**
 * Do two CGWCSGeom shapes overlap?
 * Searches the two CGWCSGeom objects' fundamental shape lists
 * for any overlap.  If any shape from \p csg_obj1 overlaps a shape
 * in \p csg_obj2, true is returned.
 * @param csg_obj1          first CGWCSGeom object
 * @param csg_obj2          second CGWCSGeom object
 * @return                  boolean true if any overlap exists
 */
bool CGWCSGeomOverlapsCSGeom(CGWCSGeomRef csg_obj1, CGWCSGeomRef csg_obj2);

/** 
 * Does a CGWCSGeom shape overlap with list of fundamental shapes?
 * Searches the CGWCSGeom object's fundamental shape list
 * for any that overlaps shapes from the variable-length list of
 * arguments passed to this function.
 *
 * Shapes are passed to this function via two arguments:  the
 * fundamental type followed by a pointer to the shape data.
 * The argument list should be terminated by passing the
 * \ref kCGWCSGeomShapeTypeEndOfList type.
 * @param csg_obj           a CGWCSGeom object
 * @param type              type of the first fundamental shape
 * @return                  boolean true if any overlap exists
 */
bool CGWCSGeomOverlapsShapes(CGWCSGeomRef csg_obj, CGWCSGeomShapeType type, ...);

/**
 * Does a CGWCSGeom shape contain an arbitrary 2D point?
 * Searches the CGWCSGeom object's fundamental shape list
 * for any that contain \p p.
 * @param csg_obj           a CGWCSGeom object
 * @param p                 a 2D point
 * @return                  boolean true if any overlap exists
 */
bool CGWCSGeomContainsPoint(CGWCSGeomRef csg_obj, CGWPoint p);

/**
 * Does a CGWCSGeom shape contain an arbitrary 2D rect?
 * Searches the CGWCSGeom object's fundamental shape list
 * for any that contain \p r.
 * @param csg_obj           a CGWCSGeom object
 * @param r                 a 2D rectangle
 * @return                  boolean true if any overlap exists
 */
bool CGWCSGeomOverlapsRect(CGWCSGeomRef csg_obj, CGWRect r);

/**
 * Does a CGWCSGeom shape contain an arbitrary 2D circle?
 * Searches the CGWCSGeom object's fundamental shape list
 * for any that contain \p c.
 * @param csg_obj           a CGWCSGeom object
 * @param c                 a 2D circle
 * @return                  boolean true if any overlap exists
 */
bool CGWCSGeomOverlapsCircle(CGWCSGeomRef csg_obj, CGWCircle c);

#endif /* __CGWCSGEOM_H__ */