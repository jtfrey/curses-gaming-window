/**
 * @file CGWCSGeom.h
 * @brief Curses Gaming Window, Constructive solid geometry support
 * 
 * C code to implement the header.
 *
 */

#include "CGWCSGeom.h"

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
                                    if ( CGWPointEqual(cur_shape->shape.p, shapes_1[j].shape.p) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWRectContainsPoint(shapes_1[j].shape.r, cur_shape->shape.p) ) {
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
                                    if ( CGWRectContainsPoint(cur_shape->shape.r, shapes_1[j].shape.p ) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeRect:
                                    if ( CGWRectContainsRect(cur_shape->shape.r, shapes_1[j].shape.r) ) {
                                        // Disable the other shape, move on to the next:
                                        keep_shapes[i_base + j] = false;
                                        n_kept--;
                                    }
                                    else if ( CGWRectContainsRect(shapes_1[j].shape.r, cur_shape->shape.r) ) {
                                        // Disable the current shape, move on to the next:
                                        keep_shapes[i_base + i] = false;
                                        n_kept--;
                                        j = n_shapes_1;
                                    }
                                    break;
                                case kCGWCSGeomShapeTypeCircle:
                                    if ( CGWRectContainsCircle(cur_shape->shape.r, shapes_1[j].shape.c) ) {
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
                                    else if ( CGWRectContainsCircle(shapes_1[j].shape.r, cur_shape->shape.c) ) {
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
    const CGWPoint      *p
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWPointEqual(csg_obj->shapes[i].shape.p, *p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWRectContainsPoint(csg_obj->shapes[i].shape.r, *p);
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
    const CGWRect       *r
)
{
    unsigned int        i = 0;
    bool                out = false;
    
    while ( ! out && (i < csg_obj->n_shapes) ) {
        switch ( csg_obj->shapes[i].type ) {
            case kCGWCSGeomShapeTypePoint:
                out = CGWRectContainsPoint(*r, csg_obj->shapes[i].shape.p);
                break;
            case kCGWCSGeomShapeTypeRect:
                out = CGWRectOverlapsRect(csg_obj->shapes[i].shape.r, *r);
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
                            if ( CGWPointEqual(cur_shape->shape.p, shapes[j].shape.p) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWRectContainsPoint(shapes[j].shape.r, cur_shape->shape.p) ) {
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
                            if ( CGWRectContainsPoint(cur_shape->shape.r, shapes[j].shape.p ) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeRect:
                            if ( CGWRectContainsRect(cur_shape->shape.r, shapes[j].shape.r) ) {
                                // Disable the other shape, move on to the next:
                                shapes[j].type = kCGWCSGeomShapeTypeOptDisabled;
                                has_been_modified = true;
                            }
                            break;
                        case kCGWCSGeomShapeTypeCircle:
                            if ( CGWRectContainsCircle(cur_shape->shape.r, shapes[j].shape.c) ) {
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
                out = __CGWCSGeomOverlapsPoint(csg_obj, va_arg(argv, CGWPoint*));
                break;
            case kCGWCSGeomShapeTypeRect:
                out = __CGWCSGeomOverlapsRect(csg_obj, va_arg(argv, CGWRect*));
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
    CGWPoint        p
)
{
    return __CGWCSGeomOverlapsPoint(csg_obj, &p);
}

//

bool
CGWCSGeomOverlapsRect(
    CGWCSGeomRef    csg_obj,
    CGWRect         r
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
