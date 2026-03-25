/**
 * @file CGWCurses.h
 * @brief Curses Gaming Window, Curses utilities
 * 
 * A header file that implements curses/ncurses helper functions.
 *
 */

#ifndef __CGWCURSES_H__
#define __CGWCURSES_H__

#include "CGWConfig.h"
#include "CGWGeom.h"

/** 
 * Curses "empty" character (transparent to background).
 * A constant holding the value of a transparent pixel in curses
 * display.
 */
extern const chtype CGWTransparentChar;

/**
 * Curses/ncurses init errors
 * The \ref CGWCursesInit() function returns an error code that
 * indicates the nature of the error encountered.
 */
typedef enum {
    kCGWCursesInitStatusOkay = 0,       /*!< curses/ncurses initialized successfully */
    kCGWCursesInitStatusSizeMismatch,   /*!< a minimum console size was provided and the actual
                                             console is too small */
    kCGWCursesInitStatusNoColorSupport  /*!< the console lacks support for color */
} CGWCursesInitStatus;

/**
 * Initialize the curses/ncurses console library
 * Starts the curses library.  If \p minimum_size is a non-NULL
 * pointer, the console dimensions are checked.  If the console is
 * at least the size in \p *minimum_size, additional library setup
 * functions are called and the console is cleared.
 * @param minimum_size      pass a \ref CGWSize indicating the
 *                          minimum dimensions permissible; a width
 *                          or height of zero (0) indicates no minimum
 * @param out_console_size  pass a pointer to a \ref CGWSize to
 *                          have the console dimensions returned
 * @return                  a value from the \ref CGWCursesInitStatus
 *                          enumeration
 */
CGWCursesInitStatus CGWCursesInit(CGWSize minimum_size, CGWSize *out_console_size);

/**
 * Create a curses window
 * Drawing to the stdscr can be very slow; this function creates
 * a window into which pixel buffers can be drawn, etc.
 *
 * The window is configured to allow the keypad, do no immediate
 * updates, etc. -- all the usual display optimizations.
 * @param display_rect      area of the stdscr in which the new
 *                          window should appear
 * @return                  the new windows or NULL in case of
 *                          error
 */
WINDOW* CGWCursesWindowCreate(CGWRect display_rect);

/**
 * Destroy a curses window
 * Delete the window \p w from the screen.
 * @param w         a window created with \ref CGWCursesWindowCreate()
 */
void CGWCursesWindowDestroy(WINDOW *w);

/**
 * Shutdown the curses/ncurses console library.
 * Ends windowing and returns the console to its original state.
 */
void CGWCursesShutdown();

/**
 * Begin window update
 * Prepare the window \p curses_window for a redraw.  The entire
 * content area is marked for refresh.
 * @param curses_window     the window being redrawn
 */
static inline
void
CGWCursesBeginUpdate(WINDOW *curses_window)
{
    redrawwin(curses_window);
    //wclear(curses_window);
}

/**
 * End window update
 * Indicate that all window updates have been completed and the
 * terminal should now be redrawn.
 */
static inline
void
CGWCursesEndUpdates(WINDOW *curses_window)
{
    wnoutrefresh(curses_window);
    doupdate();
}

/**
 * Auxilliary curses key constants
 * Additional special key constants not directly handled by
 * the curses library.  Mainly used by the \ref CGWControls
 * infrastructure.
 */
enum {
    AUX_KEY_ESC         = 27,               /*!< the ESC key */
    AUX_KEY_LSHIFT      = KEY_MAX + 100,    /*!< the left-hand shift key */
    AUX_KEY_RSHIFT,                         /*!< the right-hand shift key */
    AUX_KEY_LCONTROL,                       /*!< the left-hand control key */
    AUX_KEY_RCONTROL,                       /*!< the right-hand control key */
#ifdef CGW_OS_MACOSX
    AUX_KEY_LCOMMAND,                       /*!< the left-hand Apple key */
    AUX_KEY_RCOMMAND,                       /*!< the right-hand Apple key */
    AUX_KEY_LOPTION,                        /*!< the left-hand Option key */
    AUX_KEY_ROPTION,                        /*!< the right-hand Option key */
#endif
    AUX_KEY_MAX                             /*!< the first unused auxilliary key constant */
};

#endif /* __CGWCURSES_H__ */