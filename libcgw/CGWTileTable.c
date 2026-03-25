/**
 * @file CGWTileTable.c
 * @brief Curses Gaming Window, Table of tiles
 * 
 * Source code to accompany CGWTileTable.h
 *
 */

#include "CGWTileTable.h"

CGWTileTableRef
CGWTileTableCreate(
    unsigned int    n_tiles
)
{
    CGWTileTable    *new_table = NULL;
    
    if ( n_tiles <= CGW_TILE_TABLE_MAX_INDEX ) {
        new_table = (CGWTileTable*)calloc(1, sizeof(CGWTileTable) + n_tiles * sizeof(CGWTile));
        if ( new_table ) {
            new_table->n_tiles = new_table->n_set = n_tiles;
        }
    }
    return new_table;
}

void
CGWTileTableDestroy(
    CGWTileTableRef tile_table
)
{
    free((void*)tile_table);
}

enum {
    kCGWTileTableCodingSubTypeFull = 0,
    kCGWTileTableCodingSubTypeIndexed = 1,
    kCGWTileTableCodingSubTypeMax = 2
};

typedef struct __attribute__((packed)) {
    uint32_t        n_tiles;
    uint16_t        bits_per_pixel;
    uint32_t        width;
    uint32_t        height;
    uint16_t        sub_type;
} CGWTileTableCodingHeader;

bool
CGWTileTableCreateWithDecoder(
    CGWDecoderRef               decoder,
    CGWTileTableRef             *tile_table
)
{
    CGWTileTableRef             new_table = NULL;
    CGWTileTableCodingHeader    header;
    void*                       p = &header;
    size_t                      p_len = sizeof(header);
    CGWDecoderError             enc_err;
    int                         i, n_set = 0;
    
    enc_err = CGWDecoderExpect(decoder, false, NULL, 0, kCGWDecoderItemSignature, 'TTBL', kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
            break;
        case kCGWDecoderErrorBadSignature:
            fprintf(stderr, "ERROR:  Invalid CGWTileTable magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWTileTable\n");
            return false;
    }
    
    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem32Bit, 1, kCGWDecoderItem16Bit, 1, kCGWDecoderItem32Bit, 2, kCGWDecoderItem16Bit, 1, kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
        case kCGWDecoderErrorBadSignature:
            break;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWTileTable\n");
            return false;
    }
    if ( header.n_tiles > CGW_TILE_TABLE_MAX_INDEX ) {
        fprintf(stderr, "ERROR:  Invalid CGWTileTable size %u read (exceeds %u)\n", header.n_tiles, (unsigned int)CGW_TILE_TABLE_MAX_INDEX);
        return false;
    }
    if ( header.bits_per_pixel != CGW_BITS_PER_PIXEL ) {
        fprintf(stderr, "ERROR:  Invalid CGWTileTable bpp %hu read (expected %d)\n", header.bits_per_pixel, (int)CGW_BITS_PER_PIXEL);
        return false;
    }
    if ( header.width != CGW_BASE_TILE_WIDTH ) {
        fprintf(stderr, "ERROR:  Invalid CGWTileTable width %u read (expected %d)\n", header.width, (int)CGW_BASE_TILE_WIDTH);
        return false;
    }
    if ( header.height != CGW_BASE_TILE_HEIGHT ) {
        fprintf(stderr, "ERROR:  Invalid CGWTileTable height %u read (expected %d)\n", header.height, (int)CGW_BASE_TILE_HEIGHT);
        return false;
    }
    if ( header.sub_type >= kCGWTileTableCodingSubTypeMax ) {
        fprintf(stderr, "ERROR:  Invalid CGWTileTable sub-type %hu read\n", header.sub_type);
        return false;
    }
    new_table = CGWTileTableCreate(header.n_tiles);
    if ( new_table ) {
        switch ( header.sub_type ) {
            case kCGWTileTableCodingSubTypeFull: {
                for ( i = 0; i < header.n_tiles; i++ ) {
                    if ( ! CGWTileSetWithDecoder(decoder, &new_table->tiles[i]) ) return false;
                }
                break;
            }
            case kCGWTileTableCodingSubTypeIndexed: {
                for ( i = 0; i < header.n_tiles; i++ ) {
                    uint16_t        tile_index;
                    
                    p = &tile_index;
                    p_len = sizeof(tile_index);
                    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem16Bit, 1, kCGWDecoderItemNull);
                    switch ( enc_err ) {
                        case kCGWDecoderErrorNone:
                        case kCGWDecoderErrorInvalidItem:
                        case kCGWDecoderErrorBadSignature:
                            break;
                        case kCGWDecoderErrorInsufficientBytes:
                            fprintf(stderr, "ERROR:  Insufficient data for a CGWTileTable (tile %d)\n", i);
                            CGWTileTableDestroy(new_table);
                            return false;
                    }
                    if ( tile_index == 0xFFFF ) break;
                    if ( tile_index >= new_table->n_tiles ) {
                        fprintf(stderr, "ERROR:  Invalid tile index %hu for a CGWTileTable\n", tile_index);
                        CGWTileTableDestroy(new_table);
                        return false;
                    }
                    if ( ! CGWTileSetWithDecoder(decoder, &new_table->tiles[tile_index]) ) return false;
                    n_set++;
                }
                new_table->n_set = n_set;
                break;
            }
        }
    }
    *tile_table = new_table;
    return true;
}
