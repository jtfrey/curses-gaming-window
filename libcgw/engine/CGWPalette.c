/**
 * @file CGWPalette.c
 * @brief Curses Gaming Window, Color Palettes
 * 
 * Source code accompanying CGWPalette.h.
 *
 */
 
#include "CGWPalette.h"

enum {
    kCGWPaletteTypeRGB = 0,
    kCGWPaletteTypeIndexed = 1
};

bool
CGWPaletteSetWithDecoder(
    CGWDecoderRef       decoder,
    CGWPalettePtr       out_palette
)
{
    CGWDecoderError     enc_err;
    uint8_t             sub_type;
    void                *dst = (void*)&sub_type;
    size_t              dst_len = sizeof(sub_type);
    int                 i;
    
    enc_err = CGWDecoderExpect(decoder, false, NULL, 0, kCGWDecoderItemSignature, 'PLT_', kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
            break;
        case kCGWDecoderErrorBadSignature:
            fprintf(stderr, "ERROR:  Invalid CGWPalette magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWPalette\n");
            return false;
    }
    
    enc_err = CGWDecoderExpect(decoder, true, &dst, &dst_len, kCGWDecoderItem8Bit, 1, kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
        case kCGWDecoderErrorBadSignature:
            break;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWPalette\n");
            return false;
    }
    switch ( sub_type ) {
        case kCGWPaletteTypeRGB: {
            for ( i = 0; i < CGW_COLORS_PER_PALETTE; i++ ) {
                enc_err = CGWColorSetWithDecoder(decoder, &out_palette->colors[i]);
                switch ( enc_err ) {
                    case kCGWDecoderErrorNone:
                    case kCGWDecoderErrorInvalidItem:
                    case kCGWDecoderErrorBadSignature:
                        break;
                    case kCGWDecoderErrorInsufficientBytes:
                        fprintf(stderr, "ERROR:  Insufficient data for RGBA CGWPalette\n");
                        return false;
                }
            }
            break;
        }
        case kCGWPaletteTypeIndexed: {
            uint8_t     bytes[1 + CGW_COLORS_PER_PALETTE], *dst = bytes;
            
            dst_len = sizeof(bytes);
            enc_err = CGWDecoderExpect(decoder, true, (void**)&dst, &dst_len, kCGWDecoderItem8Bit, 5, kCGWDecoderItemNull);
            switch ( enc_err ) {
                case kCGWDecoderErrorNone:
                case kCGWDecoderErrorInvalidItem:
                case kCGWDecoderErrorBadSignature:
                    break;
                case kCGWDecoderErrorInsufficientBytes:
                    fprintf(stderr, "ERROR:  Insufficient data for gamut-indexed CGWPalette\n");
                    return false;
            }
            if ( bytes[0] >= kCGWColorGamutNES_Max ) {
                fprintf(stderr, "ERROR:  Unknown NES gamut id %hhu\n", bytes[0]);
                return false;
            }
            for ( i = 0; i < CGW_COLORS_PER_PALETTE; i++ ) {
                if ( bytes[i + 1] >= 64 ) {
                    fprintf(stderr, "ERROR:  Invalid NES gamut index %hhu\n", bytes[i + 1]);
                    return false;
                }
                out_palette->colors[i] = CGWColorMakeWithIndexToBuiltInGamut(bytes[i + 1], bytes[0]);
            }
            break;
        }
        default: {
            fprintf(stderr, "ERROR:  Invalid CGWPalette magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        }
    }
    return true;
}

bool
CGWPaletteTranslateGamuts(
    CGWPalettePtr       the_palette,
    CGWColorGamutNESIds from,
    CGWColorGamutNESIds to
)
{
    int                 color_index = 0;
    
    /* Walk through the from gamut to find the palette color: */
    while ( color_index < 4 ) {
        if ( ! CGWColorTranslateGamuts(&the_palette->colors[color_index], from, to) ) return false;
        color_index++;
    }
    return true;
}
