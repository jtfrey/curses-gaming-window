/**
 * @file CGWPixelBuffer.h
 * @brief Curses Gaming Window, Pixel Buffers
 * 
 * Types and public functions that manage pixel buffers
 * that accumulate tile drawing and draw to the curses
 * window using *addch* functions and a subrange of
 * the buffer.
 *
 */
 
#ifndef __CGWPIXELBUFFER_H__
#define __CGWPIXELBUFFER_H__

#include "CGWGeom.h"

/**
 * Pointer to a pixel buffer clearing function
 * Depending on the dimension of a pixel buffer, the optimal
 * method for clearing the buffer may vary.  When a pixel
 * buffer it created a function pointer is chosen which is
 * optimal.
 */
typedef void (*CGWPixelBufferClearFn)();

/**
 * Horizontal scaling of pixels
 * Since the destination graphics device is a terminal window
 * and characters are typically not at a 1.0 aspect ratio
 * (they are narrower than they are tall) the API supports
 * multiple characters per pixel.
 *
 * To keep drawing optimal, the pixel buffer will be
 * copied en masse into the curses display buffer; that
 * implies that the pixel buffer must actually contain the
 * multiple character per pixel.  If we were to fill the
 * curses buffer pixel-by-pixel, the pixel could be expanded
 * during the copy; but the copy would be far slower and
 * frame rates would suffer.
 */
typedef enum {
    kCGWPixelBufferHScale1x = 1,
    kCGWPixelBufferHScale2x = 2
} CGWPixelBufferHScale;

/**
 * A pixel buffer
 * A pixel buffer is at least as large as the visible
 * area of the curses display.  Backgrounds are drawn
 * into a range of positions in the pixel buffer,
 * and the \ref mvwaddchnstr() function is used to
 * efficiently copy data from the buffer into the
 * display.
 *
 * If the pixel buffer is wider than the curses display,
 * for example, then pixel data can be pre-drawn in the
 * offscreen area of the buffer (perhaps using an
 * additional thread) and "scrolling" is as simple as
 * calling \ref mvwaddchnstr() at a different horizontal
 * origin in the buffer.
 *
 * When drawing reaches the rightmost position in the
 * buffer, it wraps back to the leftmost position.
 * When a frame is copied into the display, each line
 * may be split into a trailing and leading section if
 * the copy area extends beyond the right edge.  This
 * is similar to the way the NES uses memory mapping to
 * stitch the same single-screen pixel buffer into two
 * adjacent regions of memory.
 *
 * The \p pixels buffer is row-major:  the characters in
 * each line must be adjacent for \ref mvwaddchnstr() to
 * work.  Drawing into the buffer is accomplished using
 * macros and pointers cached in the data structure.  The
 * operation begins by calling \ref CGWPixelBufferIterInit()
 * to position the pointers; \ref CGWPixelBufferIterAddCh() is
 * called some number of times to add characters to the
 * current line, then \ref CGWPixelBufferIterNextLine() is
 * called to advance to the next line.
 */
typedef struct {
    CGWSize                 dimensions;         /*!< the size of the grid */
    CGWSize                 scaled_dimensions;  /*!< the size of the grid w/ pixel-scaling accounted */
    CGWPixelBufferHScale    hscale;             /*!< the horizontal pixel-scaling */
    chtype                  *pixels;            /*!< pointer to the array of curses characters */
    chtype                  *pixels_end;        /*!< pointer to the character beyond the array */
    struct {
        //
        // Callback functions:
        //
        CGWPixelBufferClearFn   clear_buffer;          /*!< pointer to the buffer clear function */
    } callbacks;
} CGWPixelBuffer;

/**
 * Pointer to a CGWPixelBuffer
 * Type of a pointer to a CGWPixelBuffer.  Because they are always
 * dynamically-allocated, the term reference is used.
 */
typedef CGWPixelBuffer * CGWPixelBufferRef;

/**
 * Allocate and initialize a pixel buffer
 * Allocate a new pixel buffer of the given \p dimensions and
 * initialize to NUL characters.
 * @param dimensions    the size of the rectangular grid of
 *                      pixels
 * @param hscale        the horizontal scaling mode for pixels
 * @return              the newly-initialized pixel buffer or
 *                      NULL on failure
 */
CGWPixelBufferRef CGWPixelBufferCreate(CGWSize dimensions, CGWPixelBufferHScale hscale);

/**
 * Destroy a pixel buffer
 * Deallocate resources used by \p pixbuf.
 * @param pixbuf            the CGWPixelBuffer to destroy
 */
void CGWPixelBufferDestroy(CGWPixelBufferRef pixbuf);

/**
 * Get pixel at coordinate
 * Get the pixel value at an arbitrary coordinate in the buffer.
 * @param PB                the pixel buffer
 * @param C                 pixel coordinate
 * @return                  pixel value
 */
#define CGWPixelBufferGetPixel(PB, C) \
    ((PB)->pixels[(C).y * (PB)->dimensions.w * (PB)->hscale + (C).x])

/**
 * Set pixel at coordinate
 * Set the pixel value at an arbitrary coordinate in the buffer.
 * @param PB                the pixel buffer
 * @param C                 pixel coordinate
 * @param PV                pixel value
 */
#define CGWPixelBufferSetPixel(PB, C, PV) \
    ((PB)->pixels[(C).y * (PB)->dimensions.w * (PB)->hscale + (C).x] = (PV))

/**
 * Draw a region of the pixel buffer to the screen
 * Given a source rectangle \p source in the pixel buffer \p pixbuf, 
 * draw pixel data to the given curses window (\p curses_window) with
 * the destination top-left corner at \p curses_origin.
 * @param pixbuf            the pixel buffer
 * @param source            the rectangular region of the pixel buffer to draw
 * @param curses_window     the window into which pixels are drawn
 * @param curses_origin     the top-left coordinate where drawing begins in the window
 */
void CGWPixelBufferCursesDraw(CGWPixelBufferRef pixbuf, CGWRect *source, WINDOW *curses_window, CGWPoint curses_origin);

/**
 * Clear the pixel buffer
 * Calls the clear function associated with pixel buffer \p PB.
 */
#define CGWPixelBufferClear(PB) PB->callbacks.clear_buffer()

/**
 * Clear a rectangular region of the pixel buffer
 * Given an arbitrary rectangular region \p clear_rect of pixel
 * buffer \p pixbuf, clear all pixels to the empty state.
 *
 * The caller is reponsible to ensuring \p clear_rect does not
 * extend outside the dimensions of \p pixbuf.
 * @param pixbuf        the pixel buffer
 * @param clear_rect    the rectangular area to clear
 */
void CGWPixelBufferClearRect(CGWPixelBufferRef pixbuf, CGWRect clear_rect);

/**
 * Normalize a rectangle to the pixel buffer bounds
 * A \p source rectangle with its origin at (-2, -2) should have
 * the \p pixbuf periodic boundary conditions applied to it.  The
 * origin would shift to two pixels above and left of the lower-right
 * corner.  The four pixels in the lower-right corner would be mapped
 * to the upper-right corner.
 * @param pixbuf        the pixel buffer
 * @param source        pointer fo the \ref CGWRect to be adjusted
 */
void CGWPixelBufferNormalizeRect(CGWPixelBufferRef pixbuf, CGWRect *source);

/**
 * Context for iterating over a pixel buffer
 * This struct is used to maintain state while iterating over the
 * pixels in \p pixbuf.  The \p pixel field will contain the last-iterated
 * pixel value.
 */
typedef struct {
    CGWPixelBufferRef       pixbuf;         /*!< the parent pixel buffer */
    chtype                  pixel;          /*!< the last-iterated pixel */
    chtype                  *line_start;    /*!< pointer to the first character on the
                                             *   current line */
    chtype                  *line_end;      /*!< pointer to the position beyond the last
                                             *   character on the current line */
    chtype                  *base_pointer;  /*!< pointer to the starting position for drawing
                                             *   on the current line */
    chtype                  *draw_pointer;  /*!< pointer to the next character drawing location
                                             *   on the current line*/
} CGWPixelBufferIterator;

/**
 * Prepare to iterate pixel data
 * Calculate the pointer to the pixel corresponding to position
 * \p TOP_LEFT in the buffer \p PB and cache it in \p PBI.
 *
 * This is implemented as a macro to avoid function call overhead.
 * @param PB        the pixel buffer
 * @param TOP_LEFT  the pixel coordinate at which drawing will
 *                  begin
 * @param PBI       the \ref CGWPixelBufferIterator to init
 */
#define CGWPixelBufferIteratorInit(PB, TOP_LEFT, PBI) \
            ((PBI).pixbuf = (PB), \
            (PBI).line_start = (PB)->pixels + ((PB)->dimensions.w * (PB)->hscale * ((TOP_LEFT).y % (PB)->dimensions.h)), \
            (PBI).line_end = (PBI).line_start + (PB)->dimensions.w * (PB)->hscale, \
            (PBI).draw_pointer = (PBI).base_pointer = (PBI).line_start + ((TOP_LEFT).x % ((PB)->dimensions.w * (PB)->hscale)))

/**
 * Read a pixel from the buffer
 * Read the current cached pixel location in \p PBI and
 * advance to the next horizontal position.
 *
 * Use of this iteration method should NOT be mixed with
 * \ref CGWPixelBufferIteratorGetNextUnscaledCh().
 *
 * This is implemented as a macro to avoid function call
 * overhead.
 * @param PBI       the \ref CGWPixelBufferIterator
 * @return          the character at the pixel location
 */
#define CGWPixelBufferIteratorGetNextCh(PBI) \
            ( ((PBI).draw_pointer = ((PBI).draw_pointer >= (PBI).line_end) ? (PBI).line_start : (PBI).draw_pointer), \
             (PBI).pixel = *(PBI).draw_pointer, (PBI).draw_pointer += (PBI).pixbuf->hscale, (PBI).pixel )

/**
 * Read a pixel from the buffer, ignoring pixel scaling
 * Read the current cached pixel location in \p PBI and
 * advance to the next horizontal position.
 *
 * Use of this iteration method should NOT be mixed with
 * \ref CGWPixelBufferIteratorGetNextCh().
 *
 * This is implemented as a macro to avoid function call
 * overhead.
 * @param PBI       the \ref CGWPixelBufferIterator
 * @return          the character at the pixel location
 */
#define CGWPixelBufferIteratorGetNextUnscaledCh(PBI) \
            ( ((PBI).draw_pointer = ((PBI).draw_pointer >= (PBI).line_end) ? (PBI).line_start : (PBI).draw_pointer), \
             (PBI).pixel = *(PBI).draw_pointer++ )

/**
 * Add a pixel to the buffer
 * Set the current cached pixel location in \p PBI to the
 * character \p PIXEL and advance to the next horizontal
 * position.
 *
 * This is implemented as a macro to avoid function call
 * overhead.
 * @param PBI       the \ref CGWPixelBufferIterator
 * @param PIXEL     the pixel to add
 */
#define CGWPixelBufferIteratorSetNextCh(PBI, PIXEL) \
        { \
            (PBI).draw_pointer = ((PBI).draw_pointer >= (PBI).line_end) ? (PBI).line_start : (PBI).draw_pointer; \
            switch ( (PBI).pixbuf->hscale ) { \
                case 2: \
                    *(PBI).draw_pointer++ = (PIXEL); \
                case 1: \
                    *(PBI).draw_pointer++ = (PIXEL); \
            } \
        }

/**
 * Advance to the next horizontal line
 * Move the cached pointer in \p PBI to the first pixel of the next
 * line in the buffer.
 *
 * This is implemented as a macro to avoid function call
 * overhead.
 * @param PBI       the \ref CGWPixelBufferIterator
 */
#define CGWPixelBufferIteratorNextLine(PBI) \
        { \
            if ((PBI).line_end >= (PBI).pixbuf->pixels_end) { \
                (PBI).base_pointer = (PBI).pixbuf->pixels + ((PBI).base_pointer - (PBI).line_start), \
                (PBI).line_start = (PBI).pixbuf->pixels, (PBI).line_end = (PBI).pixbuf->pixels + (PBI).pixbuf->dimensions.w * (PBI).pixbuf->hscale; \
            } else { \
                (PBI).base_pointer += (PBI).pixbuf->dimensions.w * (PBI).pixbuf->hscale, \
                (PBI).line_start += (PBI).pixbuf->dimensions.w * (PBI).pixbuf->hscale, \
                (PBI).line_end += (PBI).pixbuf->dimensions.w * (PBI).pixbuf->hscale; \
            } \
            (PBI).draw_pointer = (PBI).base_pointer; \
        }

#endif /* __CGWPIXELBUFFER_H__ */
