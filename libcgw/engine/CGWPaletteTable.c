/**
 * @file CGWPaletteTable.c
 * @brief Curses Gaming Window, Palette tables
 * 
 * Source code accompanying CGWPaletteTable.h.
 *
 */

#include "CGWPaletteTable.h"

short int CGWCursesColorPairBW = -1;
short int CGWCursesColorPairWB = -1;

bool
CGWPaletteTableSetWithDecoder(
    CGWDecoderRef       decoder,
    CGWPaletteTablePtr  out_table
)
{
    uint32_t            n_palettes;
    void*               p = &n_palettes;
    size_t              p_len = sizeof(n_palettes);
    CGWDecoderError     enc_err;
    int                 i;
    
    enc_err = CGWDecoderExpect(decoder, false, NULL, 0, kCGWDecoderItemSignature, 'PLTT', kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
            break;
        case kCGWDecoderErrorBadSignature:
            fprintf(stderr, "ERROR:  Invalid CGWPaletteTable magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWPaletteTable\n");
            return false;
    }
    
    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem32Bit, 1, kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
        case kCGWDecoderErrorBadSignature:
            break;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWPaletteTable\n");
            return false;
    }
    if ( n_palettes == CGW_PALETTES_PER_TABLE ) {
        for ( i = 0; i < n_palettes; i++ ) {
            enc_err = CGWPaletteSetWithDecoder(decoder, &out_table->palettes[i]);
            switch ( enc_err ) {
                case kCGWDecoderErrorNone:
                case kCGWDecoderErrorInvalidItem:
                case kCGWDecoderErrorBadSignature:
                    break;
                case kCGWDecoderErrorInsufficientBytes:
                    fprintf(stderr, "ERROR:  Insufficient data for a CGWPaletteTable\n");
                    return false;
            }
        }
    } else {
        fprintf(stderr, "ERROR:  Invalid palette count %u in CGWPaletteTable\n", n_palettes);
        return false;
    }
    return true;
}

bool
CGWPaletteTableSetCursesColorPairs(
    CGWPaletteTablePtr  palette_table
)
{
#define COLOR_8B_TO_10B(X)      ((unsigned int)((X*1000)/255))

    short int           cc_i = 0;   // curses color index
    unsigned int        p_i = 0;    // palette index
    int                 rc = 0;
    
    while ( p_i < CGW_PALETTES_PER_TABLE ) {
        unsigned int    c_i = 0;    // color index within palette
        
        while ( (rc == 0) && (c_i < CGW_COLORS_PER_PALETTE) ) {
            rc = init_color(cc_i,
                    COLOR_8B_TO_10B(palette_table->palettes[p_i].colors[c_i].r),
                    COLOR_8B_TO_10B(palette_table->palettes[p_i].colors[c_i].g),
                    COLOR_8B_TO_10B(palette_table->palettes[p_i].colors[c_i].b));
            if ( rc ) {
                fprintf(stderr, "ERROR:  failure in init_color(%u, ...) (rc = %d)\n",
                            cc_i, rc);
                break;
            }
            rc = init_pair(cc_i + 1, cc_i, cc_i);
            if ( rc ) {
                fprintf(stderr, "ERROR:  failure in init_pair(%u, ...) (rc = %d)\n",
                            cc_i + 1, rc);
                break;
            }
            cc_i++;
            c_i++;
        }
        p_i++;
    }
    init_color(cc_i, 0, 0, 0);
    init_color(cc_i + 1, 1000, 1000, 1000);
    init_pair((CGWCursesColorPairBW = cc_i), cc_i, cc_i + 1);
    init_pair((CGWCursesColorPairWB = cc_i + 1), cc_i + 1, cc_i);
    return (rc == 0) ? true : false;

#undef COLOR_8B_TO_10B
}

bool
CGWPaletteTableTranslateGamuts(
    CGWPaletteTablePtr      palette_table,
    CGWColorGamutNESIds     from,
    CGWColorGamutNESIds     to
)
{
    unsigned int            p_i = 0;
    
    while ( p_i < CGW_PALETTES_PER_TABLE ) {
        if ( ! CGWPaletteTranslateGamuts(&palette_table->palettes[p_i], from, to) ) return false;
        p_i++;
    }
    return true;
}
