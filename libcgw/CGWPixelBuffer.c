/**
 * @file CGWPixelBuffer.c
 * @brief Curses Gaming Window, Pixel Buffers
 * 
 * Source code accompanying CGWPixelBuffer.h
 *
 */

#include "CGWPixelBuffer.h"
#include "CGWTile.h"
#include "CGWCurses.h"

#ifndef CGWPIXELBUFFERCLEAR_FIFTHS_MINIMUM_COPIES
#define CGWPIXELBUFFERCLEAR_FIFTHS_MINIMUM_COPIES        4
#endif

#ifndef CGWPIXELBUFFERCLEAR_QUARTERS_MINIMUM_COPIES
#define CGWPIXELBUFFERCLEAR_QUARTERS_MINIMUM_COPIES      5
#endif

#ifndef CGWPIXELBUFFERCLEAR_THIRDS_MINIMUM_COPIES
#define CGWPIXELBUFFERCLEAR_THIRDS_MINIMUM_COPIES        7
#endif

#ifndef CGWPIXELBUFFERCLEAR_HALVES_MINIMUM_COPIES
#define CGWPIXELBUFFERCLEAR_HALVES_MINIMUM_COPIES       10
#endif

void
__CGWPixelBufferClearFifths(
    CGWPixelBuffer  *pixbuf
)
{
    size_t          n_chars = pixbuf->scaled_dimensions.w * pixbuf->scaled_dimensions.h;
    chtype          *pixels = pixbuf->pixels;
    size_t          n_chars_fifth = n_chars / 5, n = n_chars_fifth;
            
    /* Fill the first fifth manually: */
    while ( n-- ) *pixels++ = CGWTransparentChar;
    /* Fill the second fifth: */
    memcpy(pixels, pixbuf->pixels, n_chars_fifth * sizeof(chtype));
    pixels += n_chars_fifth;
    /* Fill the second 2/5 with the first 2/5: */
    memcpy(pixels, pixbuf->pixels, n_chars_fifth * 2 * sizeof(chtype));
    pixels += n_chars_fifth + n_chars_fifth;
    /* Fill the final 1/5: */
    memcpy(pixels, pixbuf->pixels, n_chars_fifth * sizeof(chtype));
}

void
__CGWPixelBufferClearQuarters(
    CGWPixelBuffer  *pixbuf
)
{
    size_t          n_chars = pixbuf->scaled_dimensions.w * pixbuf->scaled_dimensions.h;
    chtype          *pixels = pixbuf->pixels;
    size_t          n_chars_quarter = n_chars / 4, n = n_chars_quarter;
            
    /* Fill the first quarter manually: */
    while ( n-- ) *pixels++ = CGWTransparentChar;
    /* Fill the second quarter: */
    memcpy(pixels, pixbuf->pixels, n_chars_quarter * sizeof(chtype));
    pixels += n_chars_quarter;
    /* Fill the second half: */
    memcpy(pixels, pixbuf->pixels, n_chars_quarter * 2 * sizeof(chtype));
}

void
__CGWPixelBufferClearThirds(
    CGWPixelBuffer  *pixbuf
)
{
    size_t          n_chars = pixbuf->scaled_dimensions.w * pixbuf->scaled_dimensions.h;
    chtype          *pixels = pixbuf->pixels;
    size_t          n_chars_third = n_chars / 3, n = n_chars_third;
            
    /* Fill the first third manually: */
    while ( n-- ) *pixels++ = CGWTransparentChar;
    /* Fill the second third: */
    memcpy(pixels, pixbuf->pixels, n_chars_third * sizeof(chtype));
    pixels += n_chars_third;
    /* Fill the last third: */
    memcpy(pixels, pixbuf->pixels, n_chars_third * sizeof(chtype));
}

void
__CGWPixelBufferClearHalves(
    CGWPixelBuffer  *pixbuf
)
{
    size_t          n_chars = pixbuf->scaled_dimensions.w * pixbuf->scaled_dimensions.h;
    chtype          *pixels = pixbuf->pixels;
    size_t          n_chars_half = n_chars / 2, n = n_chars_half;
    
    /* Fill the first half manually: */
    while ( n-- ) *pixels++ = CGWTransparentChar;
    /* Fill the second half: */
    memcpy(pixels, pixbuf->pixels, n_chars_half * sizeof(chtype));
}

void
__CGWPixelBufferClear(
    CGWPixelBuffer  *pixbuf
)
{
    size_t          n_chars = pixbuf->scaled_dimensions.w * pixbuf->scaled_dimensions.h;
    chtype          *pixels = pixbuf->pixels;
    
    while ( n_chars-- ) *pixels++ = CGWTransparentChar;
}

/**/

void
__CGWPixelBufferClearRectFifths(
    chtype          *pixels,
    unsigned int    w,
    unsigned int    h,
    unsigned int    stride
)
{
    chtype          *p = pixels;
    unsigned int    n_fifth = w / 5;
    
    while ( h-- ) {
        unsigned int    n = n_fifth;
        
        while ( n-- ) *p++ = CGWTransparentChar;
        /* Copy first fifth to second: */
        memcpy(p, pixels, n_fifth);
        p += n_fifth;
        /* Copy first 2/5 to second 2/5: */
        memcpy(p, pixels, n_fifth * 2);
        p += n_fifth * 2;
        /* Copy first 1/5 to final 1/5: */
        memcpy(p, pixels, n_fifth);
        p = (pixels += stride);
    }
}

/**/

void
__CGWPixelBufferClearRectQuarters(
    chtype          *pixels,
    unsigned int    w,
    unsigned int    h,
    unsigned int    stride
)
{
    chtype          *p = pixels;
    unsigned int    n_half = w / 2, n_quarter = w / 4;
    
    while ( h-- ) {
        unsigned int    n = n_quarter;
        
        while ( n-- ) *p++ = CGWTransparentChar;
        /* Copy first quarter to second: */
        memcpy(p, pixels, n_quarter);
        /* Copy first half to second: */
        memcpy(p + n_quarter, pixels, n_half);
        p = (pixels += stride);
    }
}

/**/

void
__CGWPixelBufferClearRectThirds(
    chtype          *pixels,
    unsigned int    w,
    unsigned int    h,
    unsigned int    stride
)
{
    chtype          *p = pixels;
    unsigned int    n_third = w / 3;
    
    while ( h-- ) {
        unsigned int    n = n_third;
        
        while ( n-- ) *p++ = CGWTransparentChar;
        /* Copy first third to second: */
        memcpy(p, pixels, n_third);
        /* Copy second third to last: */
        memcpy(p + n_third, pixels, n_third);
        p = (pixels += stride);
    }
}

/**/

void
__CGWPixelBufferClearRectHalves(
    chtype          *pixels,
    unsigned int    w,
    unsigned int    h,
    unsigned int    stride
)
{
    chtype          *p = pixels;
    unsigned int    n_half = w / 2;
    
    while ( h-- ) {
        unsigned int    n = n_half;
        
        while ( n-- ) *p++ = CGWTransparentChar;
        /* Copy first half to second: */
        memcpy(p, pixels, n_half);
        p = (pixels += stride);
    }
}

/**/

void
__CGWPixelBufferClearRect(
    chtype          *pixels,
    unsigned int    w,
    unsigned int    h,
    unsigned int    stride
)
{
    chtype          *p = pixels;
    
    while ( h-- ) {
        unsigned int    n = w;
        
        while ( n-- ) *p++ = CGWTransparentChar;
        p = (pixels += stride);
    }
}

/**/

CGWPixelBuffer*
CGWPixelBufferCreate(
    CGWSize                 dimensions,
    CGWPixelBufferHScale    hscale
)
{
    CGWPixelBuffer          *new_pixbuf = NULL;
    size_t                  n_chars = dimensions.w * dimensions.h * hscale;
    size_t                  chbuffer_len = sizeof(chtype) * n_chars;
    
    switch ( hscale ) {
        case kCGWPixelBufferHScale1x:
        case kCGWPixelBufferHScale2x:
            break;
        default:
            return NULL;
    }
    
    if ( chbuffer_len ) {
        new_pixbuf = (CGWPixelBuffer*)calloc(1, sizeof(CGWPixelBuffer) + chbuffer_len);
        if ( new_pixbuf ) {
            new_pixbuf->dimensions = new_pixbuf->scaled_dimensions = dimensions;
            if ( hscale > 1 ) new_pixbuf->scaled_dimensions.w *= hscale;
            new_pixbuf->hscale = hscale;
            new_pixbuf->pixels = (chtype*)(((void*)new_pixbuf) + sizeof(CGWPixelBuffer));
            new_pixbuf->pixels_end = new_pixbuf->pixels + n_chars;
            
            /* Choose the best clearing function: */
            if ( n_chars % 5 == 0 ) {
                new_pixbuf->callbacks.clear_buffer = __CGWPixelBufferClearFifths;
            }
            if ( n_chars % 4 == 0 ) {
                new_pixbuf->callbacks.clear_buffer = __CGWPixelBufferClearQuarters;
            }
            else if ( n_chars % 3 == 0 ) {
                new_pixbuf->callbacks.clear_buffer = __CGWPixelBufferClearThirds;
            }
            else if ( n_chars % 2 == 0 ) {
                new_pixbuf->callbacks.clear_buffer = __CGWPixelBufferClearHalves;
            }
            else {
                new_pixbuf->callbacks.clear_buffer = __CGWPixelBufferClear;
            }
            new_pixbuf->callbacks.clear_buffer();
        }
    }
    return new_pixbuf;
}

void
CGWPixelBufferDestroy(
    CGWPixelBufferRef   pixbuf
)
{
    free((void*)pixbuf);
}

void
CGWPixelBufferClearRect(
    CGWPixelBuffer  *pixbuf,
    CGWRect         clear_rect
)
{
    size_t          n_char = clear_rect.size.w * pixbuf->hscale * clear_rect.size.h;
    chtype          *pixels = pixbuf->pixels + (pixbuf->scaled_dimensions.w * clear_rect.origin.y) + \
                              clear_rect.origin.x;
                              
    if ( (n_char % 5 == 0) && (n_char / 5 >= CGWPIXELBUFFERCLEAR_FIFTHS_MINIMUM_COPIES) ) {
        __CGWPixelBufferClearRectFifths(pixels, clear_rect.size.w * pixbuf->hscale, clear_rect.size.h, pixbuf->scaled_dimensions.w);
    }
    else if( (n_char % 4 == 0) && (n_char / 4 >= CGWPIXELBUFFERCLEAR_QUARTERS_MINIMUM_COPIES) ) {
        __CGWPixelBufferClearRectQuarters(pixels, clear_rect.size.w * pixbuf->hscale, clear_rect.size.h, pixbuf->scaled_dimensions.w);
    }
    else if( (n_char % 3 == 0) && (n_char / 3 >= CGWPIXELBUFFERCLEAR_THIRDS_MINIMUM_COPIES) ) {
        __CGWPixelBufferClearRectThirds(pixels, clear_rect.size.w * pixbuf->hscale, clear_rect.size.h, pixbuf->scaled_dimensions.w);
    }
    else if( (n_char % 2 == 0) && (n_char / 2 >= CGWPIXELBUFFERCLEAR_HALVES_MINIMUM_COPIES) ) {
        __CGWPixelBufferClearRectHalves(pixels, clear_rect.size.w * pixbuf->hscale, clear_rect.size.h, pixbuf->scaled_dimensions.w);
    }
    else {
        __CGWPixelBufferClearRect(pixels, clear_rect.size.w * pixbuf->hscale, clear_rect.size.h, pixbuf->scaled_dimensions.w);
    }
}

void
CGWPixelBufferNormalizeRect(
    CGWPixelBufferRef   pixbuf,
    CGWRect             *source
)
{
    /* Account for periodic boundary conditions in x and y: */
    while ( source->origin.x < 0 ) source->origin.x += pixbuf->scaled_dimensions.w;
    source->origin.x %= pixbuf->scaled_dimensions.w;
    
    while ( source->origin.y < 0 ) source->origin.y += pixbuf->scaled_dimensions.h;
    source->origin.y %= pixbuf->scaled_dimensions.h;
}

void
CGWPixelBufferCursesDraw(
    CGWPixelBufferRef   pixbuf,
    CGWRect             *source,
    WINDOW              *curses_window,
    CGWPoint            curses_origin
)
{
    chtype              *pixels = pixbuf->pixels;
    int                 src_y, dst_y, n_rows;
    
    /* Account for periodic boundary conditions in x and y: */
    while ( source->origin.x < 0 ) source->origin.x += pixbuf->scaled_dimensions.w;
    source->origin.x %= pixbuf->scaled_dimensions.w;
    
    while ( source->origin.y < 0 ) source->origin.y += pixbuf->scaled_dimensions.h;
    source->origin.y %= pixbuf->scaled_dimensions.h;
    
    /* Locate the initial pixel: */
    pixels += pixbuf->scaled_dimensions.w * source->origin.y + source->origin.x;

    /* Prep our counters: */    
    n_rows = source->size.h;
    src_y = source->origin.y;
    dst_y = curses_origin.y;
        
    /* Will we need to split each line into two copy ops? */
    if ( source->origin.x + source->size.w > pixbuf->scaled_dimensions.w ) {
        /* Split copies: */
        chtype      *wrap_pixels = pixels - source->origin.x;
        int         lead_len = pixbuf->scaled_dimensions.w - source->origin.x;
        int         wrap_len = source->size.w - lead_len;
        
        /* Will we need to wrap in the y-direction, too? */
        if ( source->origin.y + source->size.h > pixbuf->scaled_dimensions.h ) {
            while ( n_rows-- ) {
                mvwaddchnstr(curses_window, dst_y, curses_origin.x, pixels, lead_len);
                mvwaddchnstr(curses_window, dst_y++, curses_origin.x + lead_len, wrap_pixels, wrap_len);
                if ( ++src_y >= pixbuf->scaled_dimensions.h ) {
                    /* Wrap in the y-direction: */
                    src_y %= pixbuf->scaled_dimensions.h;
                    wrap_pixels = pixbuf->pixels + pixbuf->scaled_dimensions.w * src_y;
                    pixels = wrap_pixels + source->origin.x;
                } else {
                    pixels += pixbuf->scaled_dimensions.w;
                    wrap_pixels += pixbuf->scaled_dimensions.w;
                }
            }
        } else {
            while ( n_rows-- ) {
                mvwaddchnstr(curses_window, dst_y, curses_origin.x, pixels, lead_len);
                mvwaddchnstr(curses_window, dst_y++, curses_origin.x + lead_len, wrap_pixels, wrap_len);
                pixels += pixbuf->scaled_dimensions.w;
                wrap_pixels += pixbuf->scaled_dimensions.w;
            }
        }
    }
    /* Will we need to wrap in the y-direction at any point? */
    else if ( source->origin.y + source->size.h > pixbuf->scaled_dimensions.h ) {
        while ( n_rows-- ) {
            mvwaddchnstr(curses_window, dst_y++, curses_origin.x, pixels, source->size.w);
            if ( ++src_y >= pixbuf->scaled_dimensions.h ) {
                /* Wrap in the y-direction: */
                src_y %= pixbuf->scaled_dimensions.h;
                pixels = pixbuf->pixels + pixbuf->scaled_dimensions.w * src_y + source->origin.x;
            } else {
                pixels += pixbuf->scaled_dimensions.w;
            }
        }
    }
    /* No wrapping necessary: */
    else {
        while ( n_rows-- ) {
            mvwaddchnstr(curses_window, dst_y++, curses_origin.x, pixels, source->size.w);
            pixels += pixbuf->scaled_dimensions.w;
        }
    }
}

