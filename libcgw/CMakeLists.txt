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

#
# Configure the bit vector minimum size:
#
set(CGW_BITVECTOR_MIN_BITS "32" CACHE STRING "Minimum bit-depth of bit vectors (default 32)")

#
# Configure the integer bit size for packed pixels:
#
set(CGW_PACKED_PIXELS_BITS "64" CACHE STRING "Bit-depth of packed pixels (default 64)")

#
# Configure the pixel bit-depth:
#
set(CGW_BITS_PER_PIXEL "2" CACHE STRING "Bit-depth of pixels (default 2)")

#
# Configure the palette index bit-depth:
#
set(CGW_BITS_PER_PALETTE "3" CACHE STRING "Bit-depth of palette indices (default 3)")

#
# Tile pixel iterators, coordinate tracking?
#
option(CGW_TILE_ITERATOR_NO_COORDS "Disable coordinate tracking in tile pixel iterators?" OFF)

#
# Configure the base tile size:
#
set(CGW_BASE_TILE_WIDTH "8" CACHE STRING "Pixel width of a baseline graphics tile (default 8)")
set(CGW_BASE_TILE_HEIGHT "8" CACHE STRING "Pixel height of a baseline graphics tile (default 8)")

#
# Configure the meta tile size:
#
set(CGW_META_TILE_WIDTH "2" CACHE STRING "Base-tile width of a meta graphics tile (default 2)")
set(CGW_META_TILE_HEIGHT "2" CACHE STRING "Base-tile height of a meta graphics tile (default 2)")

#
# Configure the maximum tile table size:
#
set(CGW_TILE_TABLE_MAX_INDEX_BITS "8" CACHE STRING "Maximum bit-depth of tile table indices.")

#
# Configure the graphics attribute bit-depth:
#
set(CGW_ATTR_BITS "8" CACHE STRING "Maximum bit-depth of graphics attributes.")

#
# Tile map iterators, coordinate tracking?
#
option(CGW_TILEMAP_ITERATOR_NO_COORDS "Disable coordinate tracking in tile map iterators?" OFF)

#
# Region byte size cutoff for special-handling of patterned memory fills:
#
set(CGWMEMFILL_INITIAL_THRESHOLD_BYTES "256" CACHE STRING "Buffer size that triggers optimized patterned fills.")

#
# Configure the fixed-precision math:
#
set(CGW_FIXEDPREC_FRAC_BITS "6" CACHE STRING "Number of fixed-precision fractional bits.")
set(CGW_FIXEDPREC_WHOLE_BITS "9" CACHE STRING "Number of fixed-precision whole bits.")
option(CGW_FIXEDPREC_FASTMATH "Disable all error-checking in fixed point arithmetic." OFF)

#
# Sanity checks:
#
if( CGW_BITS_PER_PIXEL GREATER CGW_PACKED_PIXELS_BITS )
    message(FATAL_ERROR "The value of CGW_BITS_PER_PIXEL exceeds the chosen value of CGW_PACKED_PIXELS_BITS (${CGW_BITS_PER_PIXEL} > ${CGW_PACKED_PIXELS_BITS}).")
endif()
math(EXPR CGW_FRACTIONAL_PIXELS_CHECK "${CGW_PACKED_PIXELS_BITS} % ${CGW_BITS_PER_PIXEL}")
if( NOT CGW_FRACTIONAL_PIXELS_CHECK EQUAL 0 )
    message(FATAL_ERROR "The value of CGW_PACKED_PIXELS_BITS must be a multiple of CGW_BITS_PER_PIXEL.")
endif()
if( CGW_TILE_ATTR_BITS LESS CGW_BITS_PER_PALETTE )
    message(FATAL_ERROR "The value of CGW_BITS_PER_PALETTE exceeds the chosen value of CGW_TILE_ATTR_BITS (${CGW_BITS_PER_PALETTE} > ${CGW_TILE_ATTR_BITS}).")
endif()
if( CGW_TILE_ATTR_BITS GREATER 32 )
    message(FATAL_ERROR "The value of CGW_TILE_ATTR_BITS exceeds 32.")
endif()
if ( CGW_TILE_TABLE_MAX_INDEX_BITS GREATER 32 )
    message(FATAL_ERROR "The value of CGW_TILE_TABLE_MAX_INDEX_BITS exceeds 32.")
endif()
if ( CGW_BITVECTOR_BITS GREATER 64 )
    message(FATAL_ERROR "The value of CGW_BITVECTOR_BITS exceeds 64.")
elseif ( CGW_BITVECTOR_BITS LESS 8 )
    message(FATAL_ERROR "The value of CGW_BITVECTOR_BITS must be at least 8.")
endif()
math(EXPR CGW_FIXEDPREC_BITS "${CGW_FIXEDPREC_FRAC_BITS} + ${CGW_FIXEDPREC_WHOLE_BITS} + 1")
if ( CGW_FIXEDPREC_BITS GREATER 64 )
    message(FATAL_ERROR "The total fractional, whole, and sign bits for fixed-precision exceeds 64.")
endif()

#
# Detect bit size of standard types:
#
include(CheckTypeSize)
check_type_size("short int" SHORT_INT_SIZE_BYTES)
math(EXPR SHORT_INT_SIZE_BITS "${SHORT_INT_SIZE_BYTES} * 8")
check_type_size("int" INT_SIZE_BYTES)
math(EXPR INT_SIZE_BITS "${INT_SIZE_BYTES} * 8")
check_type_size("long int" LONG_INT_SIZE_BYTES)
math(EXPR LONG_INT_SIZE_BITS "${LONG_INT_SIZE_BYTES} * 8")

#
# Generate our common header file and have all targets in this
# directory use the binary directory on the search path:
#
configure_file(CGWConfig.h.in CGWConfig.h)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Create the library target:
set(LIBCGW_SOURCES
        CGWUtil.c CGWMath.c CGWMath.s CGWGeom.c CGWGeom.s CGWCSGeom.c CGWFixedPrec.c CGWStats.c
        CGWDecoder.c CGWCurses.c
        CGWColor.c CGWPalette.c CGWPaletteTable.c CGWPaletteTableArray.c
        CGWTile.c CGWTileTable.c CGWMetaTileTable.c CGWTileMap.c
        CGWPixelBuffer.c CGWCompositePixelBuffer.c
        CGWControls.c
        test.c)
set(LIBCGW_LIBS
        ${CURSES_LIBRARIES})
if(CGW_OS_MACOSX)
    list(APPEND LIBCGW_LIBS "-framework Carbon")
endif()

add_executable(cgw ${LIBCGW_SOURCES})
target_link_libraries(cgw PRIVATE ${LIBCGW_LIBS})

# Include the necessary directories
target_include_directories(cgw PRIVATE ${CURSES_INCLUDE_DIR})



# Create the library target:
add_executable(fptest
        CGWUtil.c CGWMath.c CGWMath.s CGWFixedPrec.c
        CGWGeom2D.c CGWGeom2D.s CGWCSGeom2D.c
        fptest.c)



# Create the library target:
add_executable(turtletest
        CGWUtil.c CGWMath.c CGWMath.s CGWFixedPrec.c CGWGeom2D.c CGWGeom2D.s
        CGWTurtle2D.c
        turtletest.c)
