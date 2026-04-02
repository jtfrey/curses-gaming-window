

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "CGWConfig.h"
#include "CGWCurses.h"
#include "CGWColor.h"
#include "CGWPixel.h"
#include "CGWPalette.h"
#include "CGWPaletteTable.h"
#include "CGWPaletteTableArray.h"
#include "CGWTile.h"
#include "CGWTileTable.h"
#include "CGWMetaTileTable.h"
#include "CGWTileMap.h"
#include "CGWPixelBuffer.h"
#include "CGWCompositePixelBuffer.h"
#include "CGWTiming.h"
#include "CGWStats.h"
#include "CGWControls.h"

enum {
    kControlUp = 0,
    kControlDown,
    kControlRight,
    kControlLeft,
    kControlJump,
    kControlRun,
    kControlQuit,
    kControlPalette,
    kControlMax
};

static CGWControlBinding game_control_bindings[] = {
            CGWControlBindingInit(KEY_UP,       kControlUp),
            CGWControlBindingInit('i',          kControlUp),
            CGWControlBindingInit(KEY_DOWN,     kControlDown),
            CGWControlBindingInit('k',          kControlDown),
            CGWControlBindingInit(KEY_RIGHT,    kControlRight),
            CGWControlBindingInit('l',          kControlRight),
            CGWControlBindingInit(KEY_LEFT,     kControlLeft),
            CGWControlBindingInit('j',          kControlLeft),
            CGWControlBindingInit('s',          kControlJump),
            CGWControlBindingInit(' ',          kControlJump),
            CGWControlBindingInit('a',          kControlRun),
            CGWControlBindingInit('q',          kControlQuit),
            CGWControlBindingInit(AUX_KEY_ESC,  kControlQuit),
            CGWControlBindingInit('p',          kControlPalette),
            CGWControlBindingEndOfList()
        };

int
main()
{
    CGWControlsRef          game_controls = CGWControlsCreateWithBindings(game_control_bindings);
    CGWControlsStatesRef    game_controls_states = CGWControlsStatesCreate(game_controls);
    CGWTilePixelIterator    tpi;
    CGWTileTableRef         a_tile_table;
    CGWPaletteTableArrayRef the_palette_tables;
    CGWTile                 a_tile;
    FILE                    *fptr;
    int                     i, j, k;
    
    if ( ! game_controls ) {
        printf("Failed to create game controller.\n");
        exit(1);
    }
    
    printf("CGW_BITS_PER_PIXEL = %d\n", CGW_BITS_PER_PIXEL);
    printf("CGW_COLORS_PER_PALETTE = %d\n", CGW_COLORS_PER_PALETTE);
    printf("CGW_PIXEL_MASK = 0x%X\n", CGW_PIXEL_MASK);
    printf("CGW_PIXELS_PER_ATOM = %d\n", CGW_PIXELS_PER_ATOM);
    printf("CGW_BITS_PER_PALETTE = %d\n", CGW_BITS_PER_PALETTE);
    printf("CGW_PALETTES_PER_TABLE = %d\n", CGW_PALETTES_PER_TABLE);
    printf("CGW_PALETTE_MASK = 0x%X\n", CGW_PALETTE_MASK);
    printf("CGW_BASE_TILE_WIDTH = %d\n", CGW_BASE_TILE_WIDTH);
    printf("CGW_BASE_TILE_HEIGHT = %d\n", CGW_BASE_TILE_HEIGHT);
    printf("CGW_BASE_TILE_PACKED_PIXEL_COUNT = %d\n", CGW_BASE_TILE_PACKED_PIXEL_COUNT);
    printf("CGWAttrPaletteMask = " CGWAttrFmt "\n", CGWAttrPaletteMask);
    printf("CGWAttrVFlip = " CGWAttrFmt "\n", CGWAttrVFlip);
    printf("CGWAttrHFlip = " CGWAttrFmt "\n", CGWAttrHFlip);
    printf("CGWAttrBaseUnusedBit = " CGWAttrFmt "\n", CGWAttrBaseUnusedBit);
    printf("CGWAttrUnusedMask = " CGWAttrFmt "\n", CGWAttrUnusedMask);
    printf("\n");
    
    for ( i = 0; i < 8; i++ ) {
        printf("CGW_BASE_TILE_PIXEL_OFFSET(%d, 0) = %d\n", i, CGW_BASE_TILE_PIXEL_OFFSET(i, 0));
        printf("CGW_BASE_TILE_PIXEL_INDEX(%d, 0) = %d\n", i, CGW_BASE_TILE_PIXEL_INDEX(i, 0));
        printf("CGW_BASE_TILE_PIXEL_MASK(%d, 0) = 0x%llX\n", i, (uint64_t)CGW_BASE_TILE_PIXEL_MASK(i, 0));
        printf("CGW_BASE_TILE_PIXEL_COARSE(%d, 0) = 0x%llX\n", i, (uint64_t)CGW_BASE_TILE_PIXEL_COARSE(a_tile, i, 0));
        printf("CGW_BASE_TILE_PIXEL(%d, 0) = 0x%X\n", i, CGW_BASE_TILE_PIXEL(a_tile, i, 0));
        printf("\n");
    }
    for ( i = 0; i < 4; i++ ) {
        printf("CGW_BASE_TILE_PIXEL_OFFSET(%d, 1) = %d\n", i, CGW_BASE_TILE_PIXEL_OFFSET(i, 1));
        printf("CGW_BASE_TILE_PIXEL_INDEX(%d, 1) = %d\n", i, CGW_BASE_TILE_PIXEL_INDEX(i, 1));
        printf("CGW_BASE_TILE_PIXEL_MASK(%d, 1) = 0x%llX\n", i, (uint64_t)CGW_BASE_TILE_PIXEL_MASK(i, 1));
        printf("CGW_BASE_TILE_PIXEL(%d, 1) = 0x%X\n", i, CGW_BASE_TILE_PIXEL(a_tile, i, 1));
        printf("\n");
    }
    printf("CGW_BASE_TILE_PIXEL_OFFSET(7, 3) = %d\n", CGW_BASE_TILE_PIXEL_OFFSET(7, 3));
    printf("CGW_BASE_TILE_PIXEL_INDEX(7, 3) = %d\n", CGW_BASE_TILE_PIXEL_INDEX(7, 3));
    printf("CGW_BASE_TILE_PIXEL_MASK(7, 3) = 0x%llX\n", (uint64_t)CGW_BASE_TILE_PIXEL_MASK(7, 3));
    printf("CGW_BASE_TILE_PIXEL(7, 3) = 0x%X\n", CGW_BASE_TILE_PIXEL(a_tile, 7, 3));
    printf("\n");
    printf("CGW_BASE_TILE_PIXEL_OFFSET(0, 4) = %d\n", CGW_BASE_TILE_PIXEL_OFFSET(0, 4));
    printf("CGW_BASE_TILE_PIXEL_INDEX(0, 4) = %d\n", CGW_BASE_TILE_PIXEL_INDEX(0, 4));
    printf("CGW_BASE_TILE_PIXEL_MASK(0, 4) = 0x%llX\n", (uint64_t)CGW_BASE_TILE_PIXEL_MASK(0, 4));
    printf("CGW_BASE_TILE_PIXEL(0, 4) = 0x%X\n", CGW_BASE_TILE_PIXEL(a_tile, 0, 4));
    printf("\n");
        
    printf("CGW_BASE_TILE_PIXEL_OFFSET(7, 7) = %d\n", CGW_BASE_TILE_PIXEL_OFFSET(7, 7));
    printf("CGW_BASE_TILE_PIXEL_INDEX(7, 7) = %d\n", CGW_BASE_TILE_PIXEL_INDEX(7, 7));
    printf("CGW_BASE_TILE_PIXEL_MASK(7, 7) = 0x%llX\n", (uint64_t)CGW_BASE_TILE_PIXEL_MASK(7, 7));
    printf("CGW_BASE_TILE_PIXEL(7, 7) = 0x%X\n", CGW_BASE_TILE_PIXEL(a_tile, 7, 7));
    printf("\n");
    
    fptr = fopen("example.tile", "r");
    if ( fptr ) {
        CGWDecoderRef       txt_decoder = CGWDecoderCreateWithTextFile(fptr, false);
        
        if ( ! CGWTileSetWithDecoder(txt_decoder, &a_tile) ) {
            fprintf(stderr, "ERROR:  failed to read tile data\n");
            memset(&a_tile, 0xE4, sizeof(a_tile));
        }
        CGWDecoderDestroy(txt_decoder);
    } else {
        fprintf(stderr, "ERROR:  failed to open example tile data file\n");
        memset(&a_tile, 0xE4, sizeof(a_tile));
    }
    CGWTilePixelIteratorBlockOpen(a_tile);
        printf("(%2d, %2d) = 0x%X\n", p_x, p_y, p_pixel);
    CGWTilePixelIteratorBlockClose();
    printf("\n");
    
    CGWTilePixelIteratorInit(a_tile, tpi);
    while ( tpi.n_pixels ) {
        CGWTilePixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
        printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
        printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
    }
    printf("\nReverse-order:\n");
    CGWTileReversePixelIteratorInit(a_tile, tpi);
    while ( tpi.n_pixels ) {
        CGWTileReversePixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
        printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
        printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
    }
    printf("\nV-flip (" CGWTileVFlipPixelIteratorTypeStr "):\n");
    CGWTileVFlipPixelIteratorInit(a_tile, tpi);
    while ( tpi.n_pixels ) {
        CGWTileVFlipPixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
        printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
        printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
    }
    printf("\nH-flip (" CGWTileHFlipPixelIteratorTypeStr "):\n");
    CGWTileHFlipPixelIteratorInit(a_tile, tpi);
    while ( tpi.n_pixels ) {
        CGWTileHFlipPixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
        printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
        printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
    }
    printf("\n");
    
    fptr = fopen("example.ttbl", "r");
    if ( fptr ) {
        CGWDecoderRef       txt_decoder = CGWDecoderCreateWithTextFile(fptr, false);
        
        if ( ! CGWTileTableCreateWithDecoder(txt_decoder, &a_tile_table) ) {
            fprintf(stderr, "ERROR:  failed to read tile table data\n");
        } else {
            printf("The tile table contains %u tiles\n", a_tile_table->n_tiles);
            CGWTileTablePixelIteratorInit(a_tile_table, 0x30, tpi);
            while ( tpi.n_pixels ) {
                CGWTilePixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
                printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
                printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
            }
            printf("\nReverse-order:\n");
            CGWTileTableReversePixelIteratorInit(a_tile_table, 0x30, tpi);
            while ( tpi.n_pixels ) {
                CGWTileTableReversePixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
                printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
                printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
            }

            printf("\nV-flip (" CGWTileVFlipPixelIteratorTypeStr "):\n");
            CGWTileTableVFlipPixelIteratorInit(a_tile_table, 0x30, tpi);
            while ( tpi.n_pixels ) {
                CGWTileTableVFlipPixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
                printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
                printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
            }
            printf("\nH-flip (" CGWTileHFlipPixelIteratorTypeStr "):\n");
            CGWTileTableHFlipPixelIteratorInit(a_tile_table, 0x30, tpi);
            while ( tpi.n_pixels ) {
                CGWTileTableHFlipPixelIteratorNext(tpi);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
                printf("[%5hu] (XX, XX) = 0x%X\n", tpi.n_pixels, tpi.pixel);
#else
                printf("[%5hu] (%2d, %2d) = 0x%X\n", tpi.n_pixels, tpi.x, tpi.y, tpi.pixel);
#endif
            }
            printf("\n");
        }
        CGWDecoderDestroy(txt_decoder);
    } else {
        fprintf(stderr, "ERROR:  failed to open example tile table data file\n");
    }
    printf("\n");
    
    printf("sizeof(CGWColor) = %luB\n", sizeof(CGWColor));
    printf("sizeof(CGWPalette) = %luB\n", sizeof(CGWPalette));
    printf("sizeof(CGWPaletteTable) = %luB\n", sizeof(CGWPaletteTable));
    printf("\n");
    
    fptr = fopen("example.plts", "r");
    if ( fptr ) {
        CGWDecoderRef       txt_decoder = CGWDecoderCreateWithTextFile(fptr, false);
        
        if ( ! CGWPaletteTableArrayCreateWithDecoder(txt_decoder, &the_palette_tables) ) {
            fprintf(stderr, "ERROR:  failed to read palette table array data\n");
        } else {
            printf("The palette table array contains %u tables\n", the_palette_tables->n_tables);
            for ( i = 0; i < the_palette_tables->n_tables; i++ ) {
                printf("    Table %d contains %d palettes\n", i, CGW_PALETTES_PER_TABLE);
                for ( j = 0; j < CGW_PALETTES_PER_TABLE; j++ ) {
                    printf("        Palette %d contains %d colors\n", j, CGW_COLORS_PER_PALETTE);
                    for ( k = 0; k < CGW_COLORS_PER_PALETTE; k++ ) {
                        printf("            Color %2d: %02X %02X %02X\n", k,
                            the_palette_tables->tables[i].palettes[j].colors[k].r,
                            the_palette_tables->tables[i].palettes[j].colors[k].g,
                            the_palette_tables->tables[i].palettes[j].colors[k].b);
                    }
                }
            }
        }
        CGWDecoderDestroy(txt_decoder);
    } else {
        fprintf(stderr, "ERROR:  failed to open example palette table array file\n");
    }
    printf("\n");
    
    if ( a_tile_table ) {
        CGWMetaTileTableRef             meta_table = CGWMetaTileTableCreate(4, 0, a_tile_table);
        CGWMetaTileTablePixelIterator   meta_iter, r_meta_iter, hflip_meta_iter, vflip_meta_iter;
        unsigned int                    n_pixels;
        
        meta_table->meta_tiles[0].tile_indices[0] = 0xB4;
        meta_table->meta_tiles[0].tile_indices[1] = 0xB5;
        meta_table->meta_tiles[0].tile_indices[2] = 0xB6;
        meta_table->meta_tiles[0].tile_indices[3] = 0xB7;
        
        n_pixels = CGWMetaTilePixelIteratorInit(meta_table, 0, meta_iter);
        printf("Meta tile iterator, %u pixels:\n", n_pixels);
        n_pixels = CGWMetaTileReversePixelIteratorInit(meta_table, 0, r_meta_iter);
        printf("Meta tile reverse iterator, %u pixels:\n", n_pixels);
        n_pixels = CGWMetaTileHFlipPixelIteratorInit(meta_table, 0, hflip_meta_iter);
        printf("Meta tile h-flip iterator, %u pixels:\n", n_pixels);
        n_pixels = CGWMetaTileVFlipPixelIteratorInit(meta_table, 0, vflip_meta_iter);
        printf("Meta tile v-flip iterator, %u pixels:\n", n_pixels);
        printf("    Base tiles:  [0x%02X, 0x%02X, 0x%02X, 0x%02X]\n\n",
                    meta_iter.tile_indices[0],
                    meta_iter.tile_indices[1],
                    meta_iter.tile_indices[2],
                    meta_iter.tile_indices[3]);
            printf("%30s       %-30s      |      %30s       %-30s\n", "Regular iterator", "Reverse iterator", "H-flip iterator", "V-flip iterator");
            printf("%1$30s       %1$30s      |      %1$30s       %1$30s\n", "==============================");
        while ( meta_iter.n_pixels ) {
            CGWMetaTilePixelIteratorNext(meta_iter);
            CGWMetaTileReversePixelIteratorNext(r_meta_iter);
            CGWMetaTileHFlipPixelIteratorNext(hflip_meta_iter);
            CGWMetaTileVFlipPixelIteratorNext(vflip_meta_iter);
#ifdef CGW_TILE_ITERATOR_NO_COORDS
            printf("[%5hu] [%5hu] (%2d, XX) = 0x%X       0x%X = (%2d, XX) [%5hu] [%5hu]      |      [%5hu] [%5hu] (%2d, %2d) = 0x%X       0x%X = (%2d, %2d) [%5hu] [%5hu]\n",
                meta_iter.n_pixels, meta_iter.n_pixels_row, meta_iter.x, meta_iter.pixel,
                r_meta_iter.pixel, r_meta_iter.x, r_meta_iter.n_pixels_row, r_meta_iter.n_pixels,
                hflip_meta_iter.n_pixels, hflip_meta_iter.n_pixels_row, hflip_meta_iter.x, hflip_meta_iter.y, hflip_meta_iter.pixel,
                vflip_meta_iter.pixel, vflip_meta_iter.x, vflip_meta_iter.y, vflip_meta_iter.n_pixels_row, vflip_meta_iter.n_pixels);
#else
            printf("[%5hu] [%5hu] (%2d, %2d) = 0x%X       0x%X = (%2d, %2d) [%5hu] [%5hu]      |      [%5hu] [%5hu] (%2d, %2d) = 0x%X       0x%X = (%2d, %2d) [%5hu] [%5hu]\n",
                meta_iter.n_pixels, meta_iter.n_pixels_row, meta_iter.x, meta_iter.y, meta_iter.pixel,
                r_meta_iter.pixel, r_meta_iter.x, r_meta_iter.y, r_meta_iter.n_pixels_row, r_meta_iter.n_pixels,
                hflip_meta_iter.n_pixels, hflip_meta_iter.n_pixels_row, hflip_meta_iter.x, hflip_meta_iter.y, hflip_meta_iter.pixel,
                vflip_meta_iter.pixel, vflip_meta_iter.x, vflip_meta_iter.y, vflip_meta_iter.n_pixels_row, vflip_meta_iter.n_pixels);
#endif
        }
        printf("\n");
        
        CGWTileMapRef                a_tile_map;
        CGWTileMapCellIterator       tmap_col_iter;
        CGWTileCellPtr               tmap_cell;
        
        fptr = fopen("example.tmap", "r");
        if ( fptr ) {
            CGWDecoderRef       txt_decoder = CGWDecoderCreateWithTextFile(fptr, false);
        
            if ( ! CGWTileMapCreateWithDecoder(txt_decoder, &a_tile_map) ) {
                fprintf(stderr, "ERROR:  failed to read tile map data\n");
            } else {
                printf("a_tile_map = %p | options = %08X | dimensions = (%u, %u)\n", a_tile_map, a_tile_map->options, a_tile_map->dimensions.w, a_tile_map->dimensions.h);
                switch ( CGWTileMapTileTableType(a_tile_map) ) {
                    case kCGWTileMapOptionsTileTableTypeBase:
                        printf("          tile_table.base = %p | n_tiles = %u | n_set = %u\n", a_tile_map->tile_table.base,
                                a_tile_map->tile_table.base->n_tiles, a_tile_map->tile_table.base->n_set);
                        break;
                    case kCGWTileMapOptionsTileTableTypeMeta:
                        printf("          tile_table.meta = %p | n_tiles = %u\n", a_tile_map->tile_table.meta,
                                a_tile_map->tile_table.meta->n_tiles);
                        break;
                }
                for ( k = 0, i = 0; i < a_tile_map->dimensions.w; i++ ) {
                    for ( j = 0; j < a_tile_map->dimensions.h; j++ ) {
#ifdef CGW_TILEMAP_ITERATOR_NO_COORDS
                        printf("[%05d] = (XX,XX) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", k, a_tile_map->tile_grid[k].tile_index,
                                    a_tile_map->tile_grid[k].attributes);
#else
                        printf("[%05d] = (%2d,%2d) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", k, i, j, a_tile_map->tile_grid[k].tile_index,
                                    a_tile_map->tile_grid[k].attributes);
#endif
                        k++;
                    }
                }
                printf("Column iterator:\n");
                CGWTileMapCellColumnIteratorInit(a_tile_map, 2, tmap_col_iter);
                while ( (tmap_cell = CGWTileMapCellColumnIteratorNext(tmap_col_iter)) ) {
#ifdef CGW_TILEMAP_ITERATOR_NO_COORDS
                    printf("(XX,XX) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_cell->tile_index, tmap_cell->attributes);
#else
                    printf("(%2d,%2d) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_col_iter.x, tmap_col_iter.y,
                            tmap_cell->tile_index, tmap_cell->attributes);
#endif
                }
                
                printf("Reverse column iterator:\n");
                CGWTileMapCellReverseColumnIteratorInit(a_tile_map, 2, tmap_col_iter);
                while ( (tmap_cell = CGWTileMapCellReverseColumnIteratorNext(tmap_col_iter)) ) {
#ifdef CGW_TILEMAP_ITERATOR_NO_COORDS
                    printf("(XX,XX) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_cell->tile_index, tmap_cell->attributes);
#else
                    printf("(%2d,%2d) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_col_iter.x, tmap_col_iter.y,
                            tmap_cell->tile_index, tmap_cell->attributes);
#endif
                }
                
                printf("Row iterator:\n");
                CGWTileMapCellRowIteratorInit(a_tile_map, 2, tmap_col_iter);
                while ( (tmap_cell = CGWTileMapCellRowIteratorNext(tmap_col_iter)) ) {
#ifdef CGW_TILEMAP_ITERATOR_NO_COORDS
                    printf("(XX,XX) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_cell->tile_index, tmap_cell->attributes);
#else
                    printf("(%2d,%2d) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_col_iter.x, tmap_col_iter.y,
                            tmap_cell->tile_index, tmap_cell->attributes);
#endif
                }
                
                printf("Reverse row iterator:\n");
                CGWTileMapCellReverseRowIteratorInit(a_tile_map, 2, tmap_col_iter);
                while ( (tmap_cell = CGWTileMapCellReverseRowIteratorNext(tmap_col_iter)) ) {
#ifdef CGW_TILEMAP_ITERATOR_NO_COORDS
                    printf("(XX,XX) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_cell->tile_index, tmap_cell->attributes);
#else
                    printf("(%2d,%2d) = " CGWTileTableIndexFmt ", " CGWAttrFmt "\n", tmap_col_iter.x, tmap_col_iter.y,
                            tmap_cell->tile_index, tmap_cell->attributes);
#endif
                }
                    
                CGWCursesInitStatus curses_rc;
                CGWSize             minsize = CGWSizeMake(80, 24), actsize;
                bool                color_ok;
                
                curses_rc = CGWCursesInit(minsize, &actsize);
                if ( curses_rc != kCGWCursesInitStatusOkay ) {
                    printf("Failed to start curses for minimum console size %u x %u (rc = %d)\n", minsize.w, minsize.h, curses_rc);
                    exit(1);
                }
                
                /* Activate the palette */
                color_ok = CGWPaletteTableSetCursesColorPairs(&a_tile_map->palette_table);
                if ( color_ok ) {    
                    WINDOW                          *game_window;
                    CGWStats                        frame_stats = CGWStatsMake();
                    CGWSize                         foreground_size = CGWSizeMake(actsize.w, 2 * actsize.h);
                    CGWPixelBufferRef               foreground = CGWPixelBufferCreate(foreground_size, kCGWPixelBufferHScale2x);
                    CGWSize                         background_size = actsize;
                    CGWPixelBufferRef               background = CGWPixelBufferCreate(background_size, kCGWPixelBufferHScale1x);
                    CGWCompositePixelBufferRef      pixbuf = CGWCompositePixelBufferCreate(
                                                                    kCGWCompositePixelBufferOptionsShouldDestroyBuffers,
                                                                    actsize,
                                                                    foreground,
                                                                    background,
                                                                    NULL);
#ifdef EVENT_LOGGING
                    FILE                            *logfptr = fopen("event.log", "w");
#endif

                    game_window = CGWCursesWindowCreate(CGWRectMake(0, 0, actsize.w, actsize.h));
                    if ( pixbuf ) {
                        CGWPixelBufferIterator  piter;
                        CGWTimingInterval       engine_timer = CGWTimingIntervalMakeWithFrequency(60.0);
                        bool                    is_running = true, should_redraw = false, should_draw = true;
                        int                     x, y;
                        int                     palette = 0;
                        int                     pair_base = CGWPaletteTableMapToPairBase(palette);
                        
                        CGWPixelBufferIteratorInit(background, CGWPointMake(0, 0), piter);
                        for ( y = 0; y < background_size.h; y++ ) {
                            for ( x = 0; x < background_size.w; x++ ) {
                                int         color = 1 + random() % 32;
                                
                                CGWPixelBufferIteratorSetNextCh(piter, ' ' | COLOR_PAIR(color));
                            }
                            CGWPixelBufferIteratorNextLine(piter);
                        }
                        
                        CGWPixelBufferIteratorInit(foreground, CGWPointMake(0, 0), piter);
                        for ( y = 0; y < foreground_size.h; y++ ) {
                            for ( x = 0; x < foreground_size.w; x++ ) {
                                int     c = (((foreground_size.h) - 1 - y) == (x / 6 + 12)) ? 1 : 
                                                ((((foreground_size.h) - 1 - y) == (x * x / 192)) ? 2 : 3);
                                
                                CGWPixelBufferIteratorSetNextCh(piter, (c == 3) ? CGWTransparentChar : (' ' | COLOR_PAIR(pair_base + c)));
                            }
                            CGWPixelBufferIteratorNextLine(piter);
                        }
                        
                        CGWCursesBeginUpdate(game_window);
                        CGWCompositePixelBufferCursesDraw(pixbuf, game_window, CGWPointMake(0, 0));
                        CGWCursesEndUpdates(game_window);
                        
                        CGWTimingIntervalStart(engine_timer);
                        while ( is_running ) {
                            CGWMicroseconds     now = CGWMicrosecondsMakeNow();
                            
                            if ( CGWTimingIntervalCheck(engine_timer, now) ) {
                                // Check for controls:
                                CGWControlsStatesRead(game_controls_states);
                                if ( CGWControlsStatesGet(game_controls_states, kControlQuit) ) {
#ifdef EVENT_LOGGING
                                    fprintf(logfptr, "%16.2f[%d]: QUIT\n", now, CGWControlsStatesGet(game_controls_states, kControlQuit));
#endif
                                    is_running = false;
                                } else {
                                    if ( CGWControlsStatesGet(game_controls_states, kControlPalette) == kCGWControlStatePressed ) {
                                        palette = (palette + 1) % CGW_PALETTES_PER_TABLE;
                                        pair_base = CGWPaletteTableMapToPairBase(palette);
                                        should_redraw = should_draw = true;
                                    }
                                    else if ( CGWControlsStatesGet(game_controls_states, kControlJump) == kCGWControlStatePressed ) {
#ifdef EVENT_LOGGING
                                        fprintf(logfptr, "%16.2f[%d]: %s FOREGROUND\n", now, kCGWControlStatePressed, CGWCompositePixelBufferLayerGetEnabled(pixbuf, 0) ? "DISABLE" : "ENABLE");
#endif
                                        CGWCompositePixelBufferLayerSetEnabled(pixbuf, 0, !CGWCompositePixelBufferLayerGetEnabled(pixbuf, 0));
                                        should_draw = true;
                                    }
                                    else if ( CGWControlsStatesGet(game_controls_states, kControlRun) == kCGWControlStatePressed) {
#ifdef EVENT_LOGGING
                                        fprintf(logfptr, "%16.2f[%d]: %s BACKGROUND\n", now, kCGWControlStatePressed, CGWCompositePixelBufferLayerGetEnabled(pixbuf, 1) ? "DISABLE" : "ENABLE");
#endif
                                        CGWCompositePixelBufferLayerSetEnabled(pixbuf, 1, !CGWCompositePixelBufferLayerGetEnabled(pixbuf, 1));
                                        should_draw = true;
                                    }
                                    else {
                                        if ( CGWControlsStatesGet(game_controls_states, kControlDown) ) {
#ifdef EVENT_LOGGING
                                            fprintf(logfptr, "%16.2f[%d]: y++\n", now, CGWControlsStatesGet(game_controls_states, kControlDown));
#endif
                                            pixbuf->layers[0].display_rect.origin.y++;
                                            should_draw = true;
                                        }
                                        if ( CGWControlsStatesGet(game_controls_states, kControlUp) ) {
#ifdef EVENT_LOGGING
                                            fprintf(logfptr, "%16.2f[%d]: y--\n", now, CGWControlsStatesGet(game_controls_states, kControlUp));
#endif
                                            pixbuf->layers[0].display_rect.origin.y--;
                                            should_draw = true;
                                        }
                                        if ( CGWControlsStatesGet(game_controls_states, kControlLeft) ) {
#ifdef EVENT_LOGGING
                                            fprintf(logfptr, "%16.2f[%d]: x--\n", now, CGWControlsStatesGet(game_controls_states, kControlLeft));
#endif
                                            pixbuf->layers[0].display_rect.origin.x--;
                                            should_draw = true;
                                        }
                                        if ( CGWControlsStatesGet(game_controls_states, kControlRight) ) {
#ifdef EVENT_LOGGING
                                            fprintf(logfptr, "%16.2f[%d]: x++\n", now, CGWControlsStatesGet(game_controls_states, kControlRight));
#endif
                                            pixbuf->layers[0].display_rect.origin.x++;
                                            should_draw = true;
                                        }
                                    }
                                }
                                if ( should_redraw ) {
                                    CGWPixelBufferIteratorInit(foreground, CGWPointMake(0, 0), piter);
                                    for ( y = 0; y < foreground_size.h; y++ ) {
                                        for ( x = 0; x < foreground_size.w; x++ ) {
                                            int     c = (((foreground_size.h) - 1 - y) == (x / 6 + 12)) ? 1 : 
                                                            ((((foreground_size.h) - 1 - y) == (x * x / 192)) ? 2 : 3);
                                            
                                            CGWPixelBufferIteratorSetNextCh(piter, (c == 3) ? CGWTransparentChar : (' ' | COLOR_PAIR(pair_base + c)));
                                        }
                                        CGWPixelBufferIteratorNextLine(piter);
                                    }
                                }
                                if ( should_draw ) {
#ifdef EVENT_LOGGING
                                    CGWTimingInterval       frame_timer = CGWTimingIntervalMake(0.0);
                                    
                                    CGWTimingIntervalStart(frame_timer);
#endif
                                    CGWCursesBeginUpdate(game_window);
                                    CGWCompositePixelBufferCursesDraw(pixbuf, game_window, CGWPointMake(0, 0));
                                    CGWCursesEndUpdates(game_window);
                                    should_draw = false;
#ifdef EVENT_LOGGING
                                    CGWTimingIntervalStopNow(frame_timer);
                                    CGWStatsUpdate(&frame_stats, frame_timer.accumulated_time);
                                    fprintf(logfptr, "%16.2f[X]: DRAW @ %6.2f µs = %6.2f fps (avg %6.2f fps)\n", now, frame_timer.accumulated_time, 1e6 / frame_timer.accumulated_time, 1e6 / frame_stats.average);
#endif
                                }
                            }
                        }
                        
                        CGWCompositePixelBufferDestroy(pixbuf);
                    }
                    CGWCursesWindowDestroy(game_window);
                }
                CGWCursesShutdown();
                printf("Curses init okay, console size was %u x %u.  And color_ok = %d\n", actsize.w, actsize.h, (int)color_ok);
                
                CGWTileMapDestroy(a_tile_map);
                printf("\n");
            }
        }
        CGWMetaTileTableDestroy(meta_table);
        CGWTileTableDestroy(a_tile_table);
    }
    
    CGWStats        fps = CGWStatsMake();
    
    CGWStatsUpdate(&fps, 1.0);
    printf("n = %u, avg = %g, var = %g, std dev = %g, min = %g, max = %g\n",
        fps.n, fps.average, fps.variance, sqrt(fps.variance), fps.min, fps.max);
    CGWStatsUpdate(&fps, 2.0);
    printf("n = %u, avg = %g, var = %g, std dev = %g, min = %g, max = %g\n",
        fps.n, fps.average, fps.variance, sqrt(fps.variance), fps.min, fps.max);
    CGWStatsUpdate(&fps, 3.0);
    printf("n = %u, avg = %g, var = %g, std dev = %g, min = %g, max = %g\n",
        fps.n, fps.average, fps.variance, sqrt(fps.variance), fps.min, fps.max);
    CGWStatsUpdate(&fps, 4.0);
    printf("n = %u, avg = %g, var = %g, std dev = %g, min = %g, max = %g\n",
        fps.n, fps.average, fps.variance, sqrt(fps.variance), fps.min, fps.max);
    CGWStatsUpdate(&fps, 5.0);
    printf("n = %u, avg = %g, var = %g, std dev = %g, min = %g, max = %g\n",
        fps.n, fps.average, fps.variance, sqrt(fps.variance), fps.min, fps.max);
    CGWStatsUpdate(&fps, 6.0);
    printf("n = %u, avg = %g, var = %g, std dev = %g, min = %g, max = %g\n",
        fps.n, fps.average, fps.variance, sqrt(fps.variance), fps.min, fps.max);
    
    CGWControlsSummarize(game_controls);
    CGWControlsDestroy(game_controls);
    
    return 0;
}
