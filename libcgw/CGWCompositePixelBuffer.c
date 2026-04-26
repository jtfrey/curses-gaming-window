/**
 * @file CGWCompositePixelBuffer.c
 * @brief Curses Gaming Window, Composite Pixel Buffers
 * 
 * Source code to accompany CGWCompositePixelBuffer.h.
 *
 */
 
#include "CGWCompositePixelBuffer.h"
#include "CGWCurses.h"

CGWCompositePixelBufferRef
CGWCompositePixelBufferCreate(
    CGWBitvector    options,
    CGWSizeI2D      display_size,
    ...
)
{
    CGWCompositePixelBuffer     *cpb = NULL;
    CGWPixelBufferRef           pb;
    unsigned int                i_layer, n_layers;
    size_t                      base_byte_size, addl_bytes_size;
    va_list                     argv;
    
    if ( n_layers < 1 ) return NULL;
    
    /* Verify that all layers are at least as large as the
     * display_size and count them: */
    va_start(argv, display_size);
    n_layers = 0;
    while ( (pb = va_arg(argv, CGWPixelBufferRef)) ) {
        if ( (display_size.w > pb->scaled_dimensions.w) || (display_size.h > pb->scaled_dimensions.h) ) {
            fprintf(stderr, "ERROR:  layer %u is smaller than the display rectangle\n", n_layers);
            return NULL;
        }
        n_layers++;
    }
    va_end(argv);
    
    /* Allocate the new composite buffer struct.  We need the baseline data
     * structure size plus all the composite layer structs in the variable
     * length array.  Plus, we need to allocate the line composite buffer
     * as trailing bytes: */
    base_byte_size = sizeof(CGWCompositePixelBuffer) + n_layers * sizeof(CGWCompositeLayer);
    addl_bytes_size = display_size.w * sizeof(chtype);
    cpb = (CGWCompositePixelBuffer*)calloc(1, base_byte_size + addl_bytes_size);
    if ( cpb ) {
        cpb->options = CGWBitvectorGetBits(options, kCGWCompositePixelBufferOptionsMask);
        cpb->display_size = display_size;
        cpb->n_layers = cpb->n_enabled_layers = n_layers;
        cpb->composite = (chtype*)((void*)cpb + base_byte_size);
        
        /* Init each layer: */
        va_start(argv, display_size);
        i_layer = 0;
        while ( (pb = va_arg(argv, CGWPixelBuffer*)) ) {
            cpb->layers[i_layer].is_enabled = true;
            cpb->layers[i_layer].display_rect = CGWRectI2DMake(0, 0, display_size.w, display_size.h);
            cpb->layers[i_layer].pb = pb;
            i_layer++;
        }
    }
    return cpb;
}

void
CGWCompositePixelBufferDestroy(
    CGWCompositePixelBufferRef  comp_pixbuf
)
{
    if ( CGWBitvectorAreSetBits(comp_pixbuf->options, kCGWCompositePixelBufferOptionsShouldDestroyBuffers, kCGWCompositePixelBufferOptionsShouldDestroyBuffers) ) {
        unsigned int            i_layer = 0;
        while ( i_layer < comp_pixbuf->n_layers)
            CGWPixelBufferDestroy(comp_pixbuf->layers[i_layer++].pb);
    }
    free((void*)comp_pixbuf);
}

void
CGWCompositePixelBufferCursesDraw(
    CGWCompositePixelBufferRef  comp_pixbuf,
    WINDOW                      *curses_window,
    CGWPointI2D                 curses_origin
)
{
    CGWPixelBuffer              *pb;
    unsigned int                layer_idx, w = comp_pixbuf->display_size.w,
                                h = comp_pixbuf->display_size.h;
    chtype                      *compbuf_ptr = comp_pixbuf->composite;
        
    if ( comp_pixbuf->n_enabled_layers == 0 ) {
        /* No layers enabled, fill the compostion buffer with transparent
         * characters: */
        while ( w-- ) *compbuf_ptr++ = CGWTransparentChar;
        
        /* Blit the composition buffer into each row in the curses window: */
        while ( h-- ) {
            mvwaddchnstr(curses_window, curses_origin.y++, curses_origin.x, comp_pixbuf->composite, comp_pixbuf->display_size.w);
        }
    } else {
        /* Compile a list of the active layers, activating each for reading
         * as we go: */
        CGWPixelBufferIterator  pb_iter[comp_pixbuf->n_enabled_layers];
        unsigned int            comp_idx;
        
        for ( layer_idx = comp_idx = 0; layer_idx < comp_pixbuf->n_layers; layer_idx++ ) {
            if ( CGWCompositePixelBufferLayerGetEnabled(comp_pixbuf, layer_idx) ) {
                CGWPixelBufferNormalizeRect(comp_pixbuf->layers[layer_idx].pb, &comp_pixbuf->layers[layer_idx].display_rect);
                CGWPixelBufferIteratorInit(comp_pixbuf->layers[layer_idx].pb, comp_pixbuf->layers[layer_idx].display_rect.origin, pb_iter[comp_idx]);
                comp_idx++;
            }
        }
        
        /* Loop over rows of characters: */
        while ( h-- ) {
            /* Loop over the characters in the row: */
            while ( w-- ) {
                chtype      comp_c = CGWTransparentChar;
                
                layer_idx = 0;
                while ( layer_idx < comp_pixbuf->n_enabled_layers ) {
                    chtype  c = CGWPixelBufferIteratorGetNextUnscaledCh(pb_iter[layer_idx]);
                    
                    /* Last character in this row, advance to the next line in each
                     * layer: */
                    if ( w == 0 ) CGWPixelBufferIteratorNextLine(pb_iter[layer_idx]);
                    
                    if ( c && c != CGWTransparentChar ) {
                        comp_c = c;
                        if ( w == 0 ) {
                            /* Advance any other layers before we exit early: */
                            while ( ++layer_idx < comp_pixbuf->n_enabled_layers )
                                CGWPixelBufferIteratorNextLine(pb_iter[layer_idx]);
                        } else {
                            while ( ++layer_idx < comp_pixbuf->n_enabled_layers )
                                CGWPixelBufferIteratorGetNextUnscaledCh(pb_iter[layer_idx]);
                        }
                        break;
                    }
                    layer_idx++;
                }
                *compbuf_ptr++ = comp_c;
            }
            w = comp_pixbuf->display_size.w;
            compbuf_ptr = comp_pixbuf->composite;
            mvwaddchnstr(curses_window, curses_origin.y++, curses_origin.x, compbuf_ptr, comp_pixbuf->display_size.w);
        }
    }
}
