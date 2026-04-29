#
# The geometry testing program.
#
add_executable(geomtest)
target_sources(geomtest
    PRIVATE
        geomtest.c)
target_link_libraries(geomtest
    PRIVATE
        cgw_base)
