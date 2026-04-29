#
# Turtle graphics component library
#

option(CGW_TURTLE_DEBUG "Enable algorithm-debugging code" OFF)

# Source files for the library:
set(LIBCGW_TURTLE_SOURCES
        CGWTurtle2D.c)
# Header files for the library:
set(LIBCGW_TURTLE_HEADERS
        CGWTurtle2D.h)

add_library(cgw_turtle)
if ( ${CGW_TURTLE_DEBUG} )
    target_compile_definitions(cgw_turtle
        PRIVATE
            CGW_TURTLE_DEBUG)
endif()
target_sources(cgw_turtle
    PRIVATE
        ${LIBCGW_TURTLE_SOURCES}
    
    PUBLIC
        FILE_SET static_headers
        TYPE HEADERS
        FILES
            ${LIBCGW_TURTLE_HEADERS}
)
target_link_libraries(cgw_turtle
    PUBLIC
        cgw_base)
