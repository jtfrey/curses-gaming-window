/**
 * @file CGWTileMap.h
 * @brief Curses Gaming Window, Tiles
 * 
 * Types and public functions that deal with 2D grids
 * of graphics tiles.
 *
 */

#include "CGWTileMap.h"
#include "CGWDecoder.h"

CGWTileMapRef
CGWTileMapCreate(
    CGWSizeI2D          dimensions,
    CGWBitvector        options,
    CGWPaletteTablePtr  palette_table,
    ...
)
{
    va_list             argv;
    unsigned int        n_tiles = dimensions.w * dimensions.h;
    CGWTileMap          *new_tmap = (CGWTileMap*)calloc(1, sizeof(CGWTileMap) + n_tiles * sizeof(CGWTileCell));
    
    if ( new_tmap ) {
        new_tmap->dimensions = dimensions;
        new_tmap->options = CGWBitvectorGetBits(options, kCGWTileMapOptionsMask);
        new_tmap->palette_table = *palette_table;
        va_start(argv, palette_table);
        switch ( CGWBitvectorGetBits(options, kCGWTileMapOptionsTileTableTypeMask) ) {
            case kCGWTileMapOptionsTileTableTypeBase:
                new_tmap->tile_table.base = va_arg(argv, CGWTileTableRef);
                break;
            case kCGWTileMapOptionsTileTableTypeMeta:
                new_tmap->tile_table.meta = va_arg(argv, CGWMetaTileTableRef);
                break;
        }
        va_end(argv);
    }
    return (CGWTileMapRef)new_tmap;
}

void
CGWTileMapDestroy(
    CGWTileMapRef   tile_map
)
{
    if ( CGWBitvectorAreSetBits(tile_map->options, kCGWTileMapOptionsShouldDestroyTileTable, kCGWTileMapOptionsShouldDestroyTileTable) ) {
        switch ( CGWTileMapTileTableType(tile_map) ) {
            case kCGWTileMapOptionsTileTableTypeBase:
                CGWTileTableDestroy(tile_map->tile_table.base);
                break;
            case kCGWTileMapOptionsTileTableTypeMeta:
                CGWMetaTileTableDestroy(tile_map->tile_table.meta);
                break;
        }
    }
    free((void*)tile_map);
}

typedef struct __attribute__((packed)) {
    uint32_t        width;
    uint32_t        height;
    uint32_t        options;
} CGWTileMapCodingHeader;

bool
CGWTileMapCreateWithDecoder(
    CGWDecoderRef       decoder,
    CGWTileMapRef       *tile_map
)
{
    CGWTileMapRef               new_map = NULL;
    CGWPaletteTable             palette_table;
    CGWTileMapCodingHeader      header;
    void*                       p = &header;
    size_t                      p_len = sizeof(header);
    CGWDecoderError             enc_err;
    int                         i;
    
    enc_err = CGWDecoderExpect(decoder, false, NULL, 0, kCGWDecoderItemSignature, 'TMAP', kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
            break;
        case kCGWDecoderErrorBadSignature:
            fprintf(stderr, "ERROR:  Invalid CGWTileMap magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWTileMap\n");
            return false;
    }
    
    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem32Bit, 3, kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
        case kCGWDecoderErrorBadSignature:
            break;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWTileMap\n");
            return false;
    }
    CGWBitvectorApplyMask(header.options, kCGWTileMapOptionsMask);
    CGWBitvectorSetBits(header.options, kCGWTileMapOptionsShouldDestroyTileTable, kCGWTileMapOptionsShouldDestroyTileTable);
    switch ( CGWBitvectorGetBits(header.options, kCGWTileMapOptionsTileTableTypeMask) ) {
        case kCGWTileMapOptionsTileTableTypeBase:
        case kCGWTileMapOptionsTileTableTypeMeta:
            break;
        default:
            fprintf(stderr, "ERROR:  Invalid tile type 0x%X CGWTileMap\n", CGWBitvectorGetBits(header.options, kCGWTileMapOptionsTileTableTypeMask));
            return false;
    }
    
    /* Grab the palette table: */
    if ( ! CGWPaletteTableSetWithDecoder(decoder, &palette_table) ) {
        fprintf(stderr, "ERROR:  Unable to decode palette table associated with CGWTileMap\n");
        return false;
    }
    
    new_map = CGWTileMapCreate(CGWSizeI2DMake(header.width, header.height), header.options, &palette_table);
    if ( ! new_map ) {
        fprintf(stderr, "ERROR:  Unable to allocate CGWTileMap\n");
        return false;
    }
    
    /* Read-in the tile table: */
    switch ( CGWBitvectorGetBits(header.options, kCGWTileMapOptionsTileTableTypeMask) ) {
        case kCGWTileMapOptionsTileTableTypeBase:
            if ( ! CGWTileTableCreateWithDecoder(decoder, &new_map->tile_table.base) ) {
                fprintf(stderr, "ERROR:  Failed to read base tile table for CGWTileMap\n");
                return false;
            }
            break;
            
        case kCGWTileMapOptionsTileTableTypeMeta:
            if ( ! CGWMetaTileTableCreateWithDecoder(decoder, &new_map->tile_table.meta) ) {
                fprintf(stderr, "ERROR:  Failed to read meta tile table for CGWTileMap\n");
                return false;
            }
            break;
    }
    
    /* Read-in the tile indices: */
    for ( i = 0; i < header.width * header.height; i ++ ) {
        p = &new_map->tile_grid[i];
        p_len = sizeof(CGWTileCell);
        enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, CGWTileTableIndexCodingType, 1, CGWAttrCodingType, 1, kCGWDecoderItemNull);
        switch ( enc_err ) {
            case kCGWDecoderErrorNone:
            case kCGWDecoderErrorInvalidItem:
            case kCGWDecoderErrorBadSignature:
                break;
            case kCGWDecoderErrorInsufficientBytes:
                fprintf(stderr, "ERROR:  Insufficient data for a CGWTileMap\n");
                return false;
        }
        
    }
    *tile_map = new_map;
    return true;
}
