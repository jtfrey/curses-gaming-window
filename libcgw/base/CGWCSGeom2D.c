/**
 * @file CGWCSGeom2D.h
 * @brief Curses Gaming Window, Constructive solid geometry support
 * 
 * C code to implement the header.
 *
 */

#include "CGWCSGeom2D.h"

//

const CGWCSGeomI __CGWCSGeomIEmpty = { .n_shapes = 0 };
CGWCSGeomIRef CGWCSGeomIEmpty = (CGWCSGeomIRef)&__CGWCSGeomIEmpty;

//

static unsigned int
__CGWCSGeomShapeIDetectRedundancies(
    unsigned int        n_shapes,
    bool                *keep_shapes,
    unsigned int        n_shapes_1,
    CGWCSGeomShapeI     *shapes_1,
    ...
)
{
    unsigned int        i_base = 0, i = 0, n_kept = n_shapes;
    va_list             argv;
    
    while ( i < n_shapes ) keep_shapes[i++] = true;
    
    va_start(argv, shapes_1);
    i_base = 0;
    while ( n_shapes_1 && shapes_1 ) {
        CGWCSGeomShapeI *cur_shape = shapes_1;
        i = 0;
        while ( i < n_shapes_1 ) {
            unsigned int    j = i + 1;
            
            // Check for cur_shape being contained by other shapes:
            switch ( cur_shape->type ) {
                case kCGWCSGeomShapeTypePoint: {
                    while ( j < n_shapes_1 ) {
                        if ( keep_shapes[i_base + j] ) {
                            switch ( shapes_1[j].type ) {
                                case kCGWCSGeomShapeTypePoint:
                                    if ( CGWPointI2DEqual(cur_shape->shape.p, shapes_1[j].shape.p) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWRectI2DContainsPoint(shapes_1[j].shape.r, cur_shape->shape.p) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeCircle:
                                    if ( CGWCircleIContainsPoint(shapes_1[j].shape.c, cur_shape->shape.p) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        j++;
                    }
                    break;
                }   
                case kCGWCSGeomShapeTypeRect: {
                    while ( j < n_shapes_1 ) {
                        if ( keep_shapes[i_base + j] ) {
                            switch ( shapes_1[j].type ) {
                                case kCGWCSGeomShapeTypePoint:
                                    if ( CGWRectI2DContainsPoint(cur_shape->shape.r, shapes_1[j].shape.p ) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWRectI2DContainsRect(cur_shape->shape.r, shapes_1[j].shape.r) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWRectI2DContainsRect(shapes_1[j].shape.r, cur_shape->shape.r) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeCircle:
                                    if ( CGWRectI2DContainsCircle(cur_shape->shape.r, shapes_1[j].shape.c) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWCircleIContainsRect(shapes_1[j].shape.c, cur_shape->shape.r) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        j++;
                    }
                    break;
                }
                case kCGWCSGeomShapeTypeCircle: {
                    while ( j < n_shapes_1 ) {
                        if ( keep_shapes[i_base + j] ) {
                            switch ( shapes_1[j].type ) {
                                case kCGWCSGeomShapeTypePoint:
                                    if ( CGWCircleIContainsPoint(cur_shape->shape.c, shapes_1[j].shape.p ) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWCircleIContainsRect(cur_shape->shape.c, shapes_1[j].shape.r) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWRectI2DContainsCircle(shapes_1[j].shape.r, cur_shape->shape.c) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeCircle:
                                    if ( CGWCircleIContainsCircle(cur_shape->shape.c, shapes_1[j].shape.c) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWCircleIContainsCircle(shapes_1[j].shape.c, cur_shape->shape.c) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        j++;
                    }
                    break;
                }
                default:
                    break;
            }
        
            // Find the next non-disabled shape:
            while ( ++i < n_shapes_1 ) {
                ++cur_shape;
                if ( keep_shapes[i_base + i] ) break;
            }
        }
        i_base = i;
        n_shapes_1 = va_arg(argv, unsigned int);
        shapes_1 = va_arg(argv, CGWCSGeomShapeI*);
    }
    return n_kept;
}


//

static CGWCSGeomI*
__CGWCSGeomIAlloc(
    unsigned int    n_shapes
)
{
    size_t          n_bytes = sizeof(CGWCSGeomI) + n_shapes * sizeof(CGWCSGeomShapeI);
    CGWCSGeomI*     new_cgs_obj = (CGWCSGeomI*)calloc(1, n_bytes);
    
    if ( new_cgs_obj ) {
        new_cgs_obj->n_shapes = n_shapes;
    }
    return new_cgs_obj;
}

//

static bool
__CGWCSGeomIOverlapsPoint(
    CGWCSGeomIRef       csg_obj,
    const CGWPointI2D   *p
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWPointI2DEqual(csg_obj->shapes[i].shape.p, *p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWRectI2DContainsPoint(csg_obj->shapes[i].shape.r, *p);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = CGWCircleIContainsPoint(csg_obj->shapes[i].shape.c, *p);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

static bool
__CGWCSGeomIOverlapsRect(
    CGWCSGeomIRef       csg_obj,
    const CGWRectI2D    *r
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWRectI2DContainsPoint(*r, csg_obj->shapes[i].shape.p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWRectI2DOverlapsRect(csg_obj->shapes[i].shape.r, *r);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = CGWCircleIOverlapsRect(csg_obj->shapes[i].shape.c, *r);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

static bool
__CGWCSGeomIOverlapsCircle(
    CGWCSGeomIRef       csg_obj,
    const CGWCircleI    *c
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWCircleIContainsPoint(*c, csg_obj->shapes[i].shape.p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWCircleIOverlapsRect(*c, csg_obj->shapes[i].shape.r);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = CGWCircleIOverlapsCircle(csg_obj->shapes[i].shape.c, *c);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

bool
CGWCSGeomShapeIOptimizeArray(
    unsigned int        n_shapes,
    CGWCSGeomShapeI     *shapes
)
{
    unsigned int        i = 0;
    CGWCSGeomShapeI     *cur_shape = shapes;
    bool                has_been_modified = false;
    
    while ( i < n_shapes ) {
        unsigned int    j = i + 1;
        
        // Check for cur_shape being contained by other shapes:
        switch ( cur_shape->type ) {
            case kCGWCSGeomShapeTypePoint: {
                while ( j < n_shapes ) {
                    switch ( shapes[j].type ) {
                        case kCGWCSGeomShapeTypePoint:
                            if ( CGWPointI2DEqual(cur_shape->shape.p, shapes[j].shape.p) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWRectI2DContainsPoint(shapes[j].shape.r, cur_shape->shape.p) ) {
                                // Disable the current shape, move on to the next:
                                cur_shape->type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                                j = n_shapes;
                            }
                            break;
                        case kCGWCSGeomShapeTypeCircle:
                            if ( CGWCircleIContainsPoint(shapes[j].shape.c, cur_shape->shape.p) ) {
                                // Disable the current shape, move on to the next:
                                cur_shape->type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                                j = n_shapes;
                            }
                            break;
                        default:
                            break;
                    }
                    j++;
                }
                break;
            }   
            case kCGWCSGeomShapeTypeRect: {
                while ( j < n_shapes ) {
                    switch ( shapes[j].type ) {
                        case kCGWCSGeomShapeTypePoint:
                            if ( CGWRectI2DContainsPoint(cur_shape->shape.r, shapes[j].shape.p ) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWRectI2DContainsRect(cur_shape->shape.r, shapes[j].shape.r) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeCircle:
                            if ( CGWRectI2DContainsCircle(cur_shape->shape.r, shapes[j].shape.c) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        default:
                            break;
                    }
                    j++;
                }
                break;
            }
            case kCGWCSGeomShapeTypeCircle: {
                while ( j < n_shapes ) {
                    switch ( shapes[j].type ) {
                        case kCGWCSGeomShapeTypePoint:
                            if ( CGWCircleIContainsPoint(cur_shape->shape.c, shapes[j].shape.p ) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWCircleIContainsRect(cur_shape->shape.c, shapes[j].shape.r) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeCircle:
                            if ( CGWCircleIContainsCircle(cur_shape->shape.c, shapes[j].shape.c) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        default:
                            break;
                    }
                    j++;
                }
                break;
            }
            default:
                break;
        }
    
        // Find the next non-disabled shape:
        while ( ++i < n_shapes ) {
            ++cur_shape;
            if ( cur_shape->type != kCGWCSGeomShapeTypeOptDisabled ) break;
        }
    }
    return has_been_modified;
}

//

CGWCSGeomIRef
CGWCSGeomICreateWithShapesList(
    CGWCSGeomShapeI *shapes_and_eol
)
{
    CGWCSGeomShapeI *s = shapes_and_eol;
    unsigned int    n_shapes = 0;
    
    if ( ! s ) return CGWCSGeomIEmpty;
    while ( s->type != kCGWCSGeomShapeTypeEndOfList ) s++, n_shapes++;
    return CGWCSGeomICreateWithShapesArray(n_shapes, shapes_and_eol);
}

//

CGWCSGeomIRef
CGWCSGeomICreateWithShapesArray(
    unsigned int    n_shapes,
    CGWCSGeomShapeI *shapes
)
{
    CGWCSGeomI      *new_cgs_obj = (CGWCSGeomI*)&CGWCSGeomIEmpty;
    
    if ( n_shapes && shapes ) {
        bool            keep_shapes[n_shapes];
        unsigned int    n_kept;
        
        n_kept = __CGWCSGeomShapeIDetectRedundancies(n_shapes, keep_shapes,
                        n_shapes, shapes,
                        0, NULL);
        if ( n_kept ) {
            new_cgs_obj = __CGWCSGeomIAlloc(n_kept);
            if ( new_cgs_obj ) {
                unsigned int    i;
                CGWCSGeomShapeI *s = &new_cgs_obj->shapes[0];
                
                // Add any shapes that were kept:
                for ( i = 0; i < n_shapes; i++ ) {
                    if ( keep_shapes[i] ) *s++ = shapes[i];
                }
            }
        }
    }
    return new_cgs_obj;
}

//

CGWCSGeomIRef
CGWCSGeomICombine(
    CGWCSGeomIRef       cgs_obj1,
    CGWCSGeomIRef       cgs_obj2
)
{
    CGWCSGeomI*         new_cgs_obj = (CGWCSGeomI*)&CGWCSGeomIEmpty;
    unsigned int        n_shapes = cgs_obj1->n_shapes + cgs_obj2->n_shapes;
    
    if ( n_shapes ) {
        bool            keep_shapes[n_shapes];
        unsigned int    n_kept;
        
        n_kept = __CGWCSGeomShapeIDetectRedundancies(n_shapes, keep_shapes,
                            (unsigned int)cgs_obj1->n_shapes, (CGWCSGeomShapeI*)cgs_obj1->shapes,
                            (unsigned int)cgs_obj2->n_shapes, (CGWCSGeomShapeI*)cgs_obj2->shapes,
                            0, NULL);
        if ( n_kept ) {
            new_cgs_obj = __CGWCSGeomIAlloc(n_kept);
            if ( new_cgs_obj ) {
                unsigned int    i, i_base;
                CGWCSGeomShapeI *s = &new_cgs_obj->shapes[0];
                
                // Add any shapes from cgs_obj1:
                for ( i = 0; i < cgs_obj1->n_shapes; i++ ) {
                    if ( keep_shapes[i] ) *s++ = cgs_obj1->shapes[i];
                }
                i_base = i;
                // Add any shapes from cgs_obj2:
                for ( i = 0; i < cgs_obj2->n_shapes; i++ ) {
                    if ( keep_shapes[i_base + i] ) *s++ = cgs_obj2->shapes[i];
                }
            }
        }
    }
    return (CGWCSGeomIRef)new_cgs_obj;
}

//

void
CGWCSGeomIDestroy(
    CGWCSGeomIRef       cgs_obj
)
{
    if ( cgs_obj != CGWCSGeomIEmpty ) free((void*)cgs_obj);
}

//

bool
CGWCSGeomIOverlapsCSGeom(
    CGWCSGeomIRef       csg_obj1,
    CGWCSGeomIRef       csg_obj2
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj1->n_shapes) ) {
        switch ( csg_obj1->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = __CGWCSGeomIOverlapsPoint(csg_obj2, &csg_obj1->shapes[i].shape.p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = __CGWCSGeomIOverlapsRect(csg_obj2, &csg_obj1->shapes[i].shape.r);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = __CGWCSGeomIOverlapsCircle(csg_obj2, &csg_obj1->shapes[i].shape.c);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

bool
CGWCSGeomIOverlapsShapes(
    CGWCSGeomIRef       csg_obj,
    CGWCSGeomShapeType  type,
    ...
)
{
    bool                out;
    va_list             argv;
    
    va_start(argv, type);
    while ( type != kCGWCSGeomShapeTypeEndOfList ) {
        switch ( type ) {
            case kCGWCSGeomShapeTypePoint:
                out = __CGWCSGeomIOverlapsPoint(csg_obj, va_arg(argv, CGWPointI2D*));
                break;
            case kCGWCSGeomShapeTypeRect:
                out = __CGWCSGeomIOverlapsRect(csg_obj, va_arg(argv, CGWRectI2D*));
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = __CGWCSGeomIOverlapsCircle(csg_obj, va_arg(argv, CGWCircleI*));
                break;
            default:
                out = false;
                break;
        }
        type = va_arg(argv, CGWCSGeomShapeType);
    }
    va_end(argv);
    return out;
}

//

bool
CGWCSGeomIContainsPoint(
    CGWCSGeomIRef   csg_obj,
    CGWPointI2D     p
)
{
    return __CGWCSGeomIOverlapsPoint(csg_obj, &p);
}

//

bool
CGWCSGeomIOverlapsRect(
    CGWCSGeomIRef   csg_obj,
    CGWRectI2D      r
)
{
    return __CGWCSGeomIOverlapsRect(csg_obj, &r);
}

//

bool
CGWCSGeomIOverlapsCircle(
    CGWCSGeomIRef   csg_obj,
    CGWCircleI      c
)
{
    return __CGWCSGeomIOverlapsCircle(csg_obj, &c);
}

//
////
//


const CGWCSGeom __CGWCSGeomEmpty = { .n_shapes = 0 };
CGWCSGeomRef CGWCSGeomEmpty = (CGWCSGeomRef)&__CGWCSGeomEmpty;

//

static unsigned int
__CGWCSGeomShapeDetectRedundancies(
    unsigned int        n_shapes,
    bool                *keep_shapes,
    unsigned int        n_shapes_1,
    CGWCSGeomShape      *shapes_1,
    ...
)
{
    unsigned int        i_base = 0, i = 0, n_kept = n_shapes;
    va_list             argv;
    
    while ( i < n_shapes ) keep_shapes[i++] = true;
    
    va_start(argv, shapes_1);
    i_base = 0;
    while ( n_shapes_1 && shapes_1 ) {
        CGWCSGeomShape  *cur_shape = shapes_1;
        i = 0;
        while ( i < n_shapes_1 ) {
            unsigned int    j = i + 1;
            
            // Check for cur_shape being contained by other shapes:
            switch ( cur_shape->type ) {
                case kCGWCSGeomShapeTypePoint: {
                    while ( j < n_shapes_1 ) {
                        if ( keep_shapes[i_base + j] ) {
                            switch ( shapes_1[j].type ) {
                                case kCGWCSGeomShapeTypePoint:
                                    if ( CGWPoint2DEqual(cur_shape->shape.p, shapes_1[j].shape.p) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWRect2DContainsPoint(shapes_1[j].shape.r, cur_shape->shape.p) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeCircle:
                                    if ( CGWCircleContainsPoint(shapes_1[j].shape.c, cur_shape->shape.p) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        j++;
                    }
                    break;
                }   
                case kCGWCSGeomShapeTypeRect: {
                    while ( j < n_shapes_1 ) {
                        if ( keep_shapes[i_base + j] ) {
                            switch ( shapes_1[j].type ) {
                                case kCGWCSGeomShapeTypePoint:
                                    if ( CGWRect2DContainsPoint(cur_shape->shape.r, shapes_1[j].shape.p ) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWRect2DContainsRect(cur_shape->shape.r, shapes_1[j].shape.r) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWRect2DContainsRect(shapes_1[j].shape.r, cur_shape->shape.r) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeCircle:
                                    if ( CGWRect2DContainsCircle(cur_shape->shape.r, shapes_1[j].shape.c) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWCircleContainsRect(shapes_1[j].shape.c, cur_shape->shape.r) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        j++;
                    }
                    break;
                }
                case kCGWCSGeomShapeTypeCircle: {
                    while ( j < n_shapes_1 ) {
                        if ( keep_shapes[i_base + j] ) {
                            switch ( shapes_1[j].type ) {
                                case kCGWCSGeomShapeTypePoint:
                                    if ( CGWCircleContainsPoint(cur_shape->shape.c, shapes_1[j].shape.p ) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWCircleContainsRect(cur_shape->shape.c, shapes_1[j].shape.r) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWRect2DContainsCircle(shapes_1[j].shape.r, cur_shape->shape.c) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeCircle:
                                    if ( CGWCircleContainsCircle(cur_shape->shape.c, shapes_1[j].shape.c) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWCircleContainsCircle(shapes_1[j].shape.c, cur_shape->shape.c) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        j++;
                    }
                    break;
                }
                default:
                    break;
            }
        
            // Find the next non-disabled shape:
            while ( ++i < n_shapes_1 ) {
                ++cur_shape;
                if ( keep_shapes[i_base + i] ) break;
            }
        }
        i_base = i;
        n_shapes_1 = va_arg(argv, unsigned int);
        shapes_1 = va_arg(argv, CGWCSGeomShape*);
    }
    return n_kept;
}


//

static CGWCSGeom*
__CGWCSGeomAlloc(
    unsigned int    n_shapes
)
{
    size_t          n_bytes = sizeof(CGWCSGeom) + n_shapes * sizeof(CGWCSGeomShape);
    CGWCSGeom*      new_cgs_obj = (CGWCSGeom*)calloc(1, n_bytes);
    
    if ( new_cgs_obj ) {
        new_cgs_obj->n_shapes = n_shapes;
    }
    return new_cgs_obj;
}

//

static bool
__CGWCSGeomOverlapsPoint(
    CGWCSGeomRef        csg_obj,
    const CGWPoint2D    *p
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWPoint2DEqual(csg_obj->shapes[i].shape.p, *p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWRect2DContainsPoint(csg_obj->shapes[i].shape.r, *p);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = CGWCircleContainsPoint(csg_obj->shapes[i].shape.c, *p);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

static bool
__CGWCSGeomOverlapsRect(
    CGWCSGeomRef        csg_obj,
    const CGWRect2D     *r
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWRect2DContainsPoint(*r, csg_obj->shapes[i].shape.p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWRect2DOverlapsRect(csg_obj->shapes[i].shape.r, *r);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = CGWCircleOverlapsRect(csg_obj->shapes[i].shape.c, *r);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

static bool
__CGWCSGeomOverlapsCircle(
    CGWCSGeomRef        csg_obj,
    const CGWCircle     *c
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWCircleContainsPoint(*c, csg_obj->shapes[i].shape.p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWCircleOverlapsRect(*c, csg_obj->shapes[i].shape.r);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = CGWCircleOverlapsCircle(csg_obj->shapes[i].shape.c, *c);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

bool
CGWCSGeomShapeOptimizeArray(
    unsigned int        n_shapes,
    CGWCSGeomShape      *shapes
)
{
    unsigned int        i = 0;
    CGWCSGeomShape      *cur_shape = shapes;
    bool                has_been_modified = false;
    
    while ( i < n_shapes ) {
        unsigned int    j = i + 1;
        
        // Check for cur_shape being contained by other shapes:
        switch ( cur_shape->type ) {
            case kCGWCSGeomShapeTypePoint: {
                while ( j < n_shapes ) {
                    switch ( shapes[j].type ) {
                        case kCGWCSGeomShapeTypePoint:
                            if ( CGWPoint2DEqual(cur_shape->shape.p, shapes[j].shape.p) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWRect2DContainsPoint(shapes[j].shape.r, cur_shape->shape.p) ) {
                                // Disable the current shape, move on to the next:
                                cur_shape->type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                                j = n_shapes;
                            }
                            break;
                        case kCGWCSGeomShapeTypeCircle:
                            if ( CGWCircleContainsPoint(shapes[j].shape.c, cur_shape->shape.p) ) {
                                // Disable the current shape, move on to the next:
                                cur_shape->type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                                j = n_shapes;
                            }
                            break;
                        default:
                            break;
                    }
                    j++;
                }
                break;
            }   
            case kCGWCSGeomShapeTypeRect: {
                while ( j < n_shapes ) {
                    switch ( shapes[j].type ) {
                        case kCGWCSGeomShapeTypePoint:
                            if ( CGWRect2DContainsPoint(cur_shape->shape.r, shapes[j].shape.p ) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWRect2DContainsRect(cur_shape->shape.r, shapes[j].shape.r) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeCircle:
                            if ( CGWRect2DContainsCircle(cur_shape->shape.r, shapes[j].shape.c) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        default:
                            break;
                    }
                    j++;
                }
                break;
            }
            case kCGWCSGeomShapeTypeCircle: {
                while ( j < n_shapes ) {
                    switch ( shapes[j].type ) {
                        case kCGWCSGeomShapeTypePoint:
                            if ( CGWCircleContainsPoint(cur_shape->shape.c, shapes[j].shape.p ) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWCircleContainsRect(cur_shape->shape.c, shapes[j].shape.r) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeCircle:
                            if ( CGWCircleContainsCircle(cur_shape->shape.c, shapes[j].shape.c) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        default:
                            break;
                    }
                    j++;
                }
                break;
            }
            default:
                break;
        }
    
        // Find the next non-disabled shape:
        while ( ++i < n_shapes ) {
            ++cur_shape;
            if ( cur_shape->type != kCGWCSGeomShapeTypeOptDisabled ) break;
        }
    }
    return has_been_modified;
}

//

CGWCSGeomRef
CGWCSGeomCreateWithShapesList(
    CGWCSGeomShape  *shapes_and_eol
)
{
    CGWCSGeomShape  *s = shapes_and_eol;
    unsigned int    n_shapes = 0;
    
    if ( ! s ) return CGWCSGeomEmpty;
    while ( s->type != kCGWCSGeomShapeTypeEndOfList ) s++, n_shapes++;
    return CGWCSGeomCreateWithShapesArray(n_shapes, shapes_and_eol);
}

//

CGWCSGeomRef
CGWCSGeomCreateWithShapesArray(
    unsigned int    n_shapes,
    CGWCSGeomShape  *shapes
)
{
    CGWCSGeom       *new_cgs_obj = (CGWCSGeom*)&CGWCSGeomEmpty;
    
    if ( n_shapes && shapes ) {
        bool            keep_shapes[n_shapes];
        unsigned int    n_kept;
        
        n_kept = __CGWCSGeomShapeDetectRedundancies(n_shapes, keep_shapes,
                        n_shapes, shapes,
                        0, NULL);
        if ( n_kept ) {
            new_cgs_obj = __CGWCSGeomAlloc(n_kept);
            if ( new_cgs_obj ) {
                unsigned int    i;
                CGWCSGeomShape  *s = &new_cgs_obj->shapes[0];
                
                // Add any shapes that were kept:
                for ( i = 0; i < n_shapes; i++ ) {
                    if ( keep_shapes[i] ) *s++ = shapes[i];
                }
            }
        }
    }
    return new_cgs_obj;
}

//

CGWCSGeomRef
CGWCSGeomCombine(
    CGWCSGeomRef        cgs_obj1,
    CGWCSGeomRef        cgs_obj2
)
{
    CGWCSGeom*          new_cgs_obj = (CGWCSGeom*)&CGWCSGeomEmpty;
    unsigned int        n_shapes = cgs_obj1->n_shapes + cgs_obj2->n_shapes;
    
    if ( n_shapes ) {
        bool            keep_shapes[n_shapes];
        unsigned int    n_kept;
        
        n_kept = __CGWCSGeomShapeDetectRedundancies(n_shapes, keep_shapes,
                            (unsigned int)cgs_obj1->n_shapes, (CGWCSGeomShape*)cgs_obj1->shapes,
                            (unsigned int)cgs_obj2->n_shapes, (CGWCSGeomShape*)cgs_obj2->shapes,
                            0, NULL);
        if ( n_kept ) {
            new_cgs_obj = __CGWCSGeomAlloc(n_kept);
            if ( new_cgs_obj ) {
                unsigned int    i, i_base;
                CGWCSGeomShape  *s = &new_cgs_obj->shapes[0];
                
                // Add any shapes from cgs_obj1:
                for ( i = 0; i < cgs_obj1->n_shapes; i++ ) {
                    if ( keep_shapes[i] ) *s++ = cgs_obj1->shapes[i];
                }
                i_base = i;
                // Add any shapes from cgs_obj2:
                for ( i = 0; i < cgs_obj2->n_shapes; i++ ) {
                    if ( keep_shapes[i_base + i] ) *s++ = cgs_obj2->shapes[i];
                }
            }
        }
    }
    return (CGWCSGeomRef)new_cgs_obj;
}

//

void
CGWCSGeomDestroy(
    CGWCSGeomRef        cgs_obj
)
{
    if ( cgs_obj != CGWCSGeomEmpty ) free((void*)cgs_obj);
}

//

bool
CGWCSGeomOverlapsCSGeom(
    CGWCSGeomRef        csg_obj1,
    CGWCSGeomRef        csg_obj2
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj1->n_shapes) ) {
        switch ( csg_obj1->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = __CGWCSGeomOverlapsPoint(csg_obj2, &csg_obj1->shapes[i].shape.p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = __CGWCSGeomOverlapsRect(csg_obj2, &csg_obj1->shapes[i].shape.r);
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = __CGWCSGeomOverlapsCircle(csg_obj2, &csg_obj1->shapes[i].shape.c);
                break;
            default:
                break;
        }
        i++;
    }
    return out;
}

//

bool
CGWCSGeomOverlapsShapes(
    CGWCSGeomRef        csg_obj,
    CGWCSGeomShapeType  type,
    ...
)
{
    bool                out;
    va_list             argv;
    
    va_start(argv, type);
    while ( type != kCGWCSGeomShapeTypeEndOfList ) {
        switch ( type ) {
            case kCGWCSGeomShapeTypePoint:
                out = __CGWCSGeomOverlapsPoint(csg_obj, va_arg(argv, CGWPoint2D*));
                break;
            case kCGWCSGeomShapeTypeRect:
                out = __CGWCSGeomOverlapsRect(csg_obj, va_arg(argv, CGWRect2D*));
                break;
            case kCGWCSGeomShapeTypeCircle:
                out = __CGWCSGeomOverlapsCircle(csg_obj, va_arg(argv, CGWCircle*));
                break;
            default:
                out = false;
                break;
        }
        type = va_arg(argv, CGWCSGeomShapeType);
    }
    va_end(argv);
    return out;
}

//

bool
CGWCSGeomContainsPoint(
    CGWCSGeomRef    csg_obj,
    CGWPoint2D      p
)
{
    return __CGWCSGeomOverlapsPoint(csg_obj, &p);
}

//

bool
CGWCSGeomOverlapsRect(
    CGWCSGeomRef    csg_obj,
    CGWRect2D       r
)
{
    return __CGWCSGeomOverlapsRect(csg_obj, &r);
}

//

bool
CGWCSGeomOverlapsCircle(
    CGWCSGeomRef    csg_obj,
    CGWCircle       c
)
{
    return __CGWCSGeomOverlapsCircle(csg_obj, &c);
}
