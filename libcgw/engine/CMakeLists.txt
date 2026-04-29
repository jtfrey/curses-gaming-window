#
# Game engine component library
#

# Source files for the library:
set(LIBCGW_ENGINE_SOURCES
        CGWColor.c
        CGWCompositePixelBuffer.c
        CGWControls.c
        CGWMetaTileTable.c
        CGWPalette.c
        CGWPaletteTable.c
        CGWPaletteTableArray.c
        CGWPixelBuffer.c
        CGWTile.c
        CGWTileMap.c
        CGWTileTable.c)
# Header files for the library:
set(LIBCGW_ENGINE_HEADERS
        CGWAttr.h
        CGWColor.h
        CGWCompositePixelBuffer.h
        CGWControls.h
        CGWMetaTileTable.h
        CGWPalette.h
        CGWPaletteTable.h
        CGWPaletteTableArray.h
        CGWPixel.h
        CGWPixelBuffer.h
        CGWSprite.h
        CGWTile.h
        CGWTileCell.h
        CGWTileMap.h
        CGWTileTable.h
        CGWTileView.h
        CGWView.h)

add_library(cgw_engine)
target_sources(cgw_engine
    PRIVATE
        ${LIBCGW_ENGINE_SOURCES}
    
    PUBLIC
        FILE_SET static_headers
        TYPE HEADERS
        FILES
            ${LIBCGW_ENGINE_HEADERS}
)
target_link_libraries(cgw_engine
    PRIVATE
        cgw_base
        cgw_curses)
