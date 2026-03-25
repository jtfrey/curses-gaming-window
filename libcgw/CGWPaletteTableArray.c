/**
 * @file CGWPaletteTableArray.c
 * @brief Curses Gaming Window, Palette table arrays
 * 
 * Source code accompanying CGWPaletteTableArray.h.
 *
 */

#include "CGWPaletteTableArray.h"

CGWPaletteTableArrayRef
CGWPaletteTableArrayCreate(
    uint32_t            n_tables
)
{
    CGWPaletteTableArray    *new_array = (CGWPaletteTableArray*)calloc(1,
                                                sizeof(CGWPaletteTableArray) + 
                                                    n_tables * sizeof(CGWPaletteTable));
    
    if ( new_array ) {
        new_array->n_tables = n_tables;
    }
    return new_array;
}

void
CGWPaletteTableArrayDestroy(
    CGWPaletteTableArrayRef table_array
)
{
    free((void*)table_array);
}

bool
CGWPaletteTableArrayCreateWithDecoder(
    CGWDecoderRef           decoder,
    CGWPaletteTableArrayRef *out_array
)
{
    uint32_t            n_tables;
    void*               p = &n_tables;
    size_t              p_len = sizeof(n_tables);
    CGWDecoderError     enc_err;
    int                 i;
    
    enc_err = CGWDecoderExpect(decoder, false, NULL, 0, kCGWDecoderItemSignature, 'PLTS', kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
            break;
        case kCGWDecoderErrorBadSignature:
            fprintf(stderr, "ERROR:  Invalid CGWPaletteTableArray magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWPaletteTableArray\n");
            return false;
    }
    
    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem32Bit, 1, kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
        case kCGWDecoderErrorBadSignature:
            break;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWPaletteTableArray\n");
            return false;
    }
    if ( n_tables > 0 ) {
        CGWPaletteTableArrayRef new_array = CGWPaletteTableArrayCreate(n_tables);
        if ( new_array ) {
            for ( i = 0; i < n_tables; i++ ) {
                enc_err = CGWPaletteTableSetWithDecoder(decoder, &new_array->tables[i]);
                switch ( enc_err ) {
                    case kCGWDecoderErrorNone:
                    case kCGWDecoderErrorInvalidItem:
                    case kCGWDecoderErrorBadSignature:
                        break;
                    case kCGWDecoderErrorInsufficientBytes:
                        fprintf(stderr, "ERROR:  Insufficient data for a CGWPaletteTableArray\n");
                        CGWPaletteTableArrayDestroy(new_array);
                        return false;
                }
            }
            *out_array = new_array;
        } else {
            fprintf(stderr, "ERROR:  Unable to allocate new CGWPaletteTableArray with %u tables\n", n_tables);
            return false;
        }
    } else {
        fprintf(stderr, "ERROR:  Invalid palette table count of zero in CGWPaletteTableArray\n");
        return false;
    }
    return true;
}

bool
CGWPaletteTableArrayTranslateGamuts(
    CGWPaletteTableArrayRef palette_tables,
    CGWColorGamutNESIds     from,
    CGWColorGamutNESIds     to
)
{
    int                 p_i = 0;
    
    /* Walk through the from gamut to find the palette color: */
    while ( p_i < palette_tables->n_tables ) {
        if ( ! CGWPaletteTableTranslateGamuts(&palette_tables->tables[p_i], from, to) ) return false;
        p_i++;
    }
    return true;
}