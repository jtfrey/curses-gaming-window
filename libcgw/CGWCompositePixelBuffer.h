/**
 * @file CGWCompositePixelBuffer.h
 * @brief Curses Gaming Window, Composite Pixel Buffers
 * 
 * Types and public functions that manage stacks of pixel
 * buffers that composite on top of each other.
 *
 */
 
#ifndef __CGWCOMPOSITEPIXELBUFFER_H__
#define __CGWCOMPOSITEPIXELBUFFER_H__

#include "CGWPixelBuffer.h"
#include "CGWBitvector.h"

/**
 * Single layer of a composite pixel buffer
 * Each layer in a composite pixel buffer includes its own
 * pixel buffer and a display rectangle whose size is that of
 * the parent object.  The display rectangle in each layer
 * can move independent of the other layers (e.g. for parallax
 * effects).  Layers can be en/disabled to affect their inclusion
 * in the composite.
 */
typedef struct {
    bool            is_enabled;     /*!< include the layer in rendering if true */
    CGWRect         display_rect;   /*!< area of the pixel buffer that will be
                                     *   composited with the other layers */
    CGWPixelBuffer  *pb;            /*!< the pixel buffer associated with the layer */
} CGWCompositeLayer;

/**
 * CGWCompositePixelBuffer options
 * Optional behaviors associted with \ref CGWCompositePixelBuffer
 * instances.
 */
enum {
    kCGWCompositePixelBufferOptionsShouldDestroyBuffers = (1 << 0),   /*!< destroy the underlying pixel buffers when
                                                                           the \ref CGWCompositePixelBuffer is
                                                                           destroyed */
    kCGWCompositePixelBufferOptionsMask = kCGWCompositePixelBufferOptionsShouldDestroyBuffers /*!< all options */
};

/**
 * Multi-layer composite pixel buffer
 * Using separate curses calls to fill the background and THEN adding sprites
 * on top would have transparent sprite pixels displayed as the background color
 * and not the underlying pixel or background.  To achieve proper layering, the
 * sprite pixels must be composited onto the background pixels:  a transparent
 * pixel in the front layer will reveal that of the next layer, then the next,
 * etc.  In this paradigm, the composite involves no blending or mathematical
 * transformation, just the use of the first non-empty character from the top
 * layer down.
 *
 * The simplest instance would be:
 *
 *     - Layer 0:  sprite tiles, 512x240 (1 screen)
 *     - Layer 1:  background tiles, 5120x240 (10 screens of data)
 *
 * The sprites move within a display plane that slides on top of the background layer
 * but requires no off-screen buffering.  Adding an additional background layer can
 * be useful:
 *
 *     - Layer 0:  sprite tiles, 512x240 (1 screen)
 *     - Layer 1:  background tiles, 1536x240 (3 screens of data)
 *     - Layer 2:  far background tiles, 1024x240 (2 screens of data)
 * 
 * Scrolling layer 2 at 25% the rate of Layer 1, for example, would produce a
 * parallax effect.
 */
typedef struct {
    CGWBitvector        options;            /*!< behavioral options */
    CGWSize             display_size;       /*!< the size of the visible region
                                                 across all layers */
    unsigned int        n_layers;           /*!< the number of layers */
    unsigned int        n_enabled_layers;   /*!< the number of layers enabled */
    chtype              *composite;         /*!< storage for a single row of composited
                                                 pixels */
    CGWCompositeLayer   layers[];           /*!< the composite layers */
} CGWCompositePixelBuffer;

/**
 * Pointer to a CGWCompositePixelBuffer
 * Type of a pointer to a CGWCompositePixelBuffer.  Because they are always
 * dynamically-allocated, the term reference is used.
 */
typedef CGWCompositePixelBuffer * CGWCompositePixelBufferRef;

/**
 * Create a new composite pixel buffer
 * Creates and initializes a new multi-layer composite pixel
 * buffer.  The \p display_size should be the dimensions of the
 * window into which the buffer will be drawn.  The layers being
 * grouped into the composite buffer must all be at least of
 * that dimension.
 * 
 * Following the \p display_size argument should be one or more
 * pointers to CGWPixelBuffer objects, terminated by a NULL
 * pointer.  The layers are ordered from the first in the
 * argument list to the last:  pixels in the first overlay those
 * in the next, etc.
 *
 * @param display_size      the size of the display area
 * @return                  the new composite pixel buffer or
 *                          NULL on error
 */
CGWCompositePixelBufferRef CGWCompositePixelBufferCreate(CGWBitvector options, CGWSize display_size, ...);

/**
 * Destroy a CGWCompositePixelBuffer
 * Dispose of resources used by \p comp_pixbuf.
 * @param comp_pixbuf       the CGWCompositePixelBufferRef
 */
void CGWCompositePixelBufferDestroy(CGWCompositePixelBufferRef comp_pixbuf);

/**
 * Get the enable state of a layer
 * Returns the enable state of layer \p layer_index in \p comp_pixbuf.
 * @param comp_pixbuf       the CGWCompositePixelBufferRef
 * @param layer_index       the layer index
 * @return                  boolean true (enabled) or false (disabled)
 */
static inline
bool
CGWCompositePixelBufferLayerGetEnabled(
    CGWCompositePixelBufferRef  comp_pixbuf,
    unsigned int                layer_index
)
{
    return comp_pixbuf->layers[layer_index].is_enabled;
}

/**
 * Get the enable state of a layer
 * Returns the enable state of layer \p layer_index in \p comp_pixbuf.
 * @param comp_pixbuf       the CGWCompositePixelBufferRef
 * @param layer_index       the layer index
 * @return                  boolean true (enabled) or false (disabled)
 */
static inline
void
CGWCompositePixelBufferLayerSetEnabled(
    CGWCompositePixelBufferRef  comp_pixbuf,
    unsigned int                layer_index,
    bool                        is_enabled
)
{
    if ( comp_pixbuf->layers[layer_index].is_enabled != is_enabled ) {
        comp_pixbuf->layers[layer_index].is_enabled = is_enabled;
        comp_pixbuf->n_enabled_layers += is_enabled ? 1 : -1;
    }
}

/**
 * Composite the layers and draw to a curses window
 * The current display rectangle in each active layer is walked
 * one row at a time, composite from the top layer down into
 * a buffer that is then written en masse to a line in the \p
 * curses_window.
 *
 * Drawing in the window matches the display size of \p comp_pixbuf
 * and starts at window coordinate \p curses_origin.
 *
 * @param comp_pixbuf       the composite pixel buffer
 * @param curses_window     The curses window into which pixels
 *                          are drawn
 * @param curses_origin     window coordinate at which drawing
 *                          begins
 */
void CGWCompositePixelBufferCursesDraw(CGWCompositePixelBufferRef comp_pixbuf, WINDOW *curses_window, CGWPoint curses_origin);

#endif /* __CGWCOMPOSITEPIXELBUFFER_H__ */
