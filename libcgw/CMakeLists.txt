#
# Find the ncurses library
#
find_package(Curses REQUIRED)

#
# Which OS?
#
if(APPLE)
    set(CGW_OS_MACOSX true)
    set(CGW_ENABLE_ASSEMBLY 1 CACHE BOOL "Enable optimized assembly functions.")
elseif(UNIX)
    set(CGW_OS_LINUX true)
endif()

add_subdirectory(base)
add_subdirectory(curses)
add_subdirectory(engine)
add_subdirectory(turtle)
