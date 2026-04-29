#
# The turtle program, yertle.
#
add_executable(yertle)
target_sources(yertle
    PRIVATE
        yertle.c)
target_link_libraries(yertle
    PRIVATE
        cgw_turtle
        cgw_curses)
