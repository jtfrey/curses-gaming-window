#
# The 2D matrix transform testing program.
#
add_executable(matrixtest)
target_sources(matrixtest
    PRIVATE
        matrixtest.c)
target_link_libraries(matrixtest
    PRIVATE
        cgw_base)
