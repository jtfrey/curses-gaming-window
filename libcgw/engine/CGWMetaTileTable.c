/**
 * @file CGWMetaTileTable.c
 * @brief Curses Gaming Window, Table of tiles composed of tiles
 * 
 * Source code to accompany CGWMetaTileTable.h.
 *
 */

#include "CGWMetaTileTable.h"

CGWMetaTileTableRef
CGWMetaTileTableCreate(
    unsigned int        n_tiles,
    CGWBitvector        options,
    CGWTileTableRef     base_tiles
)
{
    CGWMetaTileTable    *new_table = NULL;
    
    if ( n_tiles <= CGW_TILE_TABLE_MAX_INDEX ) {
        new_table = (CGWMetaTileTable*)calloc(1, sizeof(CGWMetaTileTable) + n_tiles * sizeof(CGWMetaTile));
        if ( new_table ) {
            new_table->n_tiles = n_tiles;
            new_table->options = CGWBitvectorGetBits(options, kCGWMetaTileTableOptionsMask);
            new_table->base_tiles = base_tiles;
        }
    }
    return new_table;
}

void
CGWMetaTileTableDestroy(
    CGWMetaTileTableRef tile_table
)
{
    if ( tile_table->base_tiles && CGWBitvectorAreSetBits(tile_table->options, kCGWMetaTileTableOptionsShouldDestroyBaseTileTable, kCGWMetaTileTableOptionsShouldDestroyBaseTileTable) ) {
        CGWTileTableDestroy(tile_table->base_tiles);
    }
    free((void*)tile_table);
}

/**
 * Create a CGWMetaTileTable using data from a decoder
 * A CGWMetaTileTable is read from the \p decoder.
 *
 * A signature word ('MTBL') precedes the meta tile data.
 * A header follows:
 *
 *     uint32_t         n_tiles
 *     <TTBL> …         the encoded base tile table
 *
 * A sequence of encoded CGWMetaTile structures follows.
 * @param decoder       the decoder from which data is
 *                      loaded
 * @param tile_table    pointer to a CGWMetaTileTableRef to
 *                      be filled-in with a reference to the
 *                      allocated object
 * @return              Boolean true if successful
 */
bool
CGWMetaTileTableCreateWithDecoder(
    CGWDecoderRef       decoder,
    CGWMetaTileTableRef *tile_table
)
{
    CGWMetaTileTableRef         new_table = NULL;
    CGWTileTableRef             base_table = NULL;
    uint32_t                    n_tiles;
    void*                       p = &n_tiles;
    size_t                      p_len = sizeof(n_tiles);
    CGWDecoderError             enc_err;
    int                         i;
    
    enc_err = CGWDecoderExpect(decoder, false, NULL, 0, kCGWDecoderItemSignature, 'MTBL', kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
            break;
        case kCGWDecoderErrorBadSignature:
            fprintf(stderr, "ERROR:  Invalid CGWMetaTileTable magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWMetaTileTable\n");
            return false;
    }
    
    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem32Bit, 1, kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
        case kCGWDecoderErrorBadSignature:
            break;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWMetaTileTable\n");
            return false;
    }
    
    if ( n_tiles == 0 ) {
        fprintf(stderr, "ERROR:  Invalid CGWMetaTileTable tile count %u read\n", n_tiles);
        return false;
    }
    if ( n_tiles > CGW_TILE_TABLE_MAX_INDEX ) {
        fprintf(stderr, "ERROR:  Invalid CGWMetaTileTable size %u read (exceeds %u)\n", n_tiles, (unsigned int)CGW_TILE_TABLE_MAX_INDEX);
        return false;
    }
    
    /*
     * Now decode the base table:
     */
    if ( ! CGWTileTableCreateWithDecoder(decoder, &base_table) ) {
        fprintf(stderr, "ERROR:  Failed to decode base tile table\n");
        return false;
    }
    
    /*
     * Allocate the object:
     */
    new_table = CGWMetaTileTableCreate(n_tiles, kCGWMetaTileTableOptionsShouldDestroyBaseTileTable, base_table);
    if ( new_table ) {
        for ( i = 0; i < n_tiles; i++ ) {
            p = &new_table->meta_tiles[i];
            p_len = sizeof(CGWMetaTile);
            enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, CGWTileTableIndexCodingType, CGW_META_TILE_WIDTH * CGW_META_TILE_HEIGHT, kCGWDecoderItemNull);
            switch ( enc_err ) {
                case kCGWDecoderErrorNone:
                case kCGWDecoderErrorInvalidItem:
                case kCGWDecoderErrorBadSignature:
                    break;
                case kCGWDecoderErrorInsufficientBytes:
                    fprintf(stderr, "ERROR:  Insufficient data for a CGWMetaTileTable\n");
                    return false;
            }
        }
    }
    *tile_table = new_table;
    return true;
}
