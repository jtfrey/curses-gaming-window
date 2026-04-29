#
# The base CMake build system configuration file for
# the Curses Gaming Window project.
#
cmake_minimum_required(VERSION 3.10)
project(CursesGraphicsWindow
    VERSION
        0.0.1
    LANGUAGES
        C ASM)

# Find the ncurses library
find_package(Curses REQUIRED)

option(ENABLE_TESTS "Build the test programs" ON)

add_subdirectory(libcgw)
add_subdirectory(yertle)
if ( ${ENABLE_TESTS} )
    add_subdirectory(tests)
endif()
