cmake_minimum_required(VERSION 3.10)
project(CursesGraphicsWindow LANGUAGES C ASM)

# Find the ncurses library
find_package(Curses REQUIRED)

add_subdirectory(libcgw)
