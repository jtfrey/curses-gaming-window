#
# Curses console interface component library
#

# Source files for the library:
set(LIBCGW_CURSES_SOURCES
        CGWCurses.c)
# Header files for the library:
set(LIBCGW_CURSES_HEADERS
        CGWCurses.h)

add_library(cgw_curses)
target_sources(cgw_curses
    PRIVATE
        ${LIBCGW_CURSES_SOURCES}
    
    PUBLIC
        FILE_SET static_headers
        TYPE HEADERS
        FILES
            ${LIBCGW_CURSES_HEADERS}
)
target_link_libraries(cgw_curses
    PUBLIC
        cgw_base
        Curses)
