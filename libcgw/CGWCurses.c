/**
 * @file CGWCurses.c
 * @brief Curses Gaming Window, Curses utilities
 * 
 * Source code to accompany CGWCurses.h.
 *
 */

#include "CGWCurses.h"

const chtype CGWTransparentChar = (' ' | A_NORMAL);

CGWCursesInitStatus
CGWCursesInit(
    CGWSizeI2D  minimum_size,
    CGWSizeI2D  *out_console_size
)
{
    int         w, h;
    
    initscr();
    getmaxyx(stdscr, h, w);
    if ( out_console_size ) {
        out_console_size->w = w;
        out_console_size->h = h;
    }
    if ( (minimum_size.w && (w < minimum_size.w)) || (minimum_size.h && (h < minimum_size.h)) ) {
        endwin();
        return kCGWCursesInitStatusSizeMismatch;
    }
    if ( (start_color() == ERR) || ! has_colors() || ! can_change_color() ) {
        endwin();
        return kCGWCursesInitStatusNoColorSupport;
    }
    clear();
	noecho();
	
	cbreak();	/* Line buffering disabled. pass on everything */
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	clearok(stdscr, FALSE);
	leaveok(stdscr, TRUE);
	scrollok(stdscr, FALSE);
	immedok(stdscr, FALSE);
    bkgd(COLOR_PAIR(1) | ' ');
    curs_set(0);
    return kCGWCursesInitStatusOkay;
}

WINDOW*
CGWCursesWindowCreate(
    CGWRectI2D  display_rect
)
{
    WINDOW      *w = newwin(display_rect.size.h,
                            display_rect.size.w,
                            display_rect.origin.y,
                            display_rect.origin.x);
    if ( w ) {
        keypad(w, TRUE);
        nodelay(w, TRUE);
        clearok(w, FALSE);
        leaveok(w, TRUE);
        scrollok(w, FALSE);
        immedok(w, FALSE);
    }
    return w;
}

void
CGWCursesWindowDestroy(
    WINDOW      *w
)
{
    nodelay(w, FALSE);
    delwin(w);
}

void
CGWCursesShutdown()
{
    /* Consume all input that got buffered... */
    nodelay(stdscr, FALSE);
    halfdelay(1);
    while ( getch() != ERR );
    
    endwin();
}