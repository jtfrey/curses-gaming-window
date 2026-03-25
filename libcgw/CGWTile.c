/**
 * @file CGWTile.c
 * @brief Curses Gaming Window, Tiles
 * 
 * Source code accompanying CGWTile.h.
 *
 */

#include "CGWTile.h"

enum {
    kCGWTileCodingSubTypePacked = 0,
    kCGWTileCodingSubTypeUnpacked = 1
};

typedef struct __attribute__((packed)) {
    uint16_t        sub_type;
    uint16_t        bits_per_pixel;
    uint32_t        width;
    uint32_t        height;
} CGWTileCodingHeader;

bool
CGWTileSetWithDecoder(
    CGWDecoderRef   decoder,
    CGWTilePtr      out_tile
)
{
    CGWTileCodingHeader header;
    void*               p = &header;
    size_t              p_len = sizeof(header);
    CGWDecoderError     enc_err;
    int                 i;
    
    enc_err = CGWDecoderExpect(decoder, false, NULL, 0, kCGWDecoderItemSignature, 'TILE', kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
            break;
        case kCGWDecoderErrorBadSignature:
            fprintf(stderr, "ERROR:  Invalid CGWTile magic word at offset %lld\n", CGWDecoderGetPosition(decoder));
            return false;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWTile\n");
            return false;
    }
    
    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem16Bit, 2, kCGWDecoderItem32Bit, 2, kCGWDecoderItemNull);
    switch ( enc_err ) {
        case kCGWDecoderErrorNone:
        case kCGWDecoderErrorInvalidItem:
        case kCGWDecoderErrorBadSignature:
            break;
        case kCGWDecoderErrorInsufficientBytes:
            fprintf(stderr, "ERROR:  Insufficient data for a CGWTile\n");
            return false;
    }
    if ( header.bits_per_pixel != CGW_BITS_PER_PIXEL ) {
        fprintf(stderr, "ERROR:  Invalid CGWTile bpp %u read (expected %d)\n", header.bits_per_pixel, CGW_BITS_PER_PIXEL);
        return false;
    }
    if ( header.width != CGW_BASE_TILE_WIDTH ) {
        fprintf(stderr, "ERROR:  Invalid CGWTile width %u read (expected %d)\n", header.width, CGW_BASE_TILE_WIDTH);
        return false;
    }
    if ( header.height != CGW_BASE_TILE_HEIGHT ) {
        fprintf(stderr, "ERROR:  Invalid CGWTile height %u read (expected %d)\n", header.height, CGW_BASE_TILE_HEIGHT);
        return false;
    }
    
    switch ( header.sub_type ) {
        default:
            fprintf(stderr, "ERROR:  Invalid CGWTile sub-type %u read\n", header.sub_type);
            return false;
            
        case kCGWTileCodingSubTypePacked: {
            CGWDecoderItem  p_type;
            
            p = &out_tile->pixels[0];
            p_len = sizeof(out_tile->pixels);
            switch ( sizeof(CGWPackedPixels) ) {
                case 1:
                    p_type = kCGWDecoderItem8Bit;
                    break;
                case 2:
                    p_type = kCGWDecoderItem16Bit;
                    break;
                case 4:
                    p_type = kCGWDecoderItem32Bit;
                    break;
                case 8:
                    p_type = kCGWDecoderItem64Bit;
                    break;
                default:
                    fprintf(stderr, "ERROR:  Unhandled CGWPackedPixels byte size in CGWTileSetWithDecoder\n");
                    return false;
            }
            enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, p_type, CGW_BASE_TILE_PACKED_PIXEL_COUNT, kCGWDecoderItemNull);
            switch ( enc_err ) {
                case kCGWDecoderErrorNone:
                case kCGWDecoderErrorInvalidItem:
                case kCGWDecoderErrorBadSignature:
                    break;
                case kCGWDecoderErrorInsufficientBytes:
                    fprintf(stderr, "ERROR:  Insufficient data for a CGWTile\n");
                    return false;
            }
            break;
        }
            
        case kCGWTileCodingSubTypeUnpacked: {
            if ( header.bits_per_pixel <= 8 ) {
                for ( i = 0; i < CGW_BASE_TILE_PACKED_PIXEL_COUNT; i++ ) {
                    uint8_t             unpacked_pixels[CGW_PIXELS_PER_ATOM];
                    CGWPackedPixels     packed_pixels = 0;
                    int                 j;
                    
                    p = &unpacked_pixels[0];
                    p_len = sizeof(unpacked_pixels);
                    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem8Bit, CGW_PIXELS_PER_ATOM, kCGWDecoderItemNull);
                    switch ( enc_err ) {
                        case kCGWDecoderErrorNone:
                        case kCGWDecoderErrorInvalidItem:
                        case kCGWDecoderErrorBadSignature:
                            break;
                        case kCGWDecoderErrorInsufficientBytes:
                            fprintf(stderr, "ERROR:  Insufficient data for a CGWTile\n");
                            return false;
                    }
                    j = CGW_PIXELS_PER_ATOM;
                    while ( j-- > 0 ) {
                        packed_pixels = (packed_pixels << CGW_BITS_PER_PIXEL) | (unpacked_pixels[j] & CGW_PIXEL_MASK);
                    }
                    out_tile->pixels[i] = packed_pixels;
                }
            }
            else if ( header.bits_per_pixel <= 16 ) {
                for ( i = 0; i < CGW_BASE_TILE_PACKED_PIXEL_COUNT; i++ ) {
                    uint16_t            unpacked_pixels[CGW_PIXELS_PER_ATOM];
                    CGWPackedPixels     packed_pixels = 0;
                    int                 j;
                    
                    p = &unpacked_pixels[0];
                    p_len = sizeof(unpacked_pixels);
                    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem16Bit, CGW_PIXELS_PER_ATOM, kCGWDecoderItemNull);
                    switch ( enc_err ) {
                        case kCGWDecoderErrorNone:
                        case kCGWDecoderErrorInvalidItem:
                        case kCGWDecoderErrorBadSignature:
                            break;
                        case kCGWDecoderErrorInsufficientBytes:
                            fprintf(stderr, "ERROR:  Insufficient data for a CGWTile\n");
                            return false;
                    }
                    j = CGW_PIXELS_PER_ATOM;
                    while ( j-- > 0 ) {
                        packed_pixels = (packed_pixels << CGW_BITS_PER_PIXEL) | (unpacked_pixels[j] & CGW_PIXEL_MASK);
                    }
                    out_tile->pixels[i] = packed_pixels;
                }
            }
            else if ( header.bits_per_pixel <= 32 ) {
                for ( i = 0; i < CGW_BASE_TILE_PACKED_PIXEL_COUNT; i++ ) {
                    uint32_t            unpacked_pixels[CGW_PIXELS_PER_ATOM];
                    CGWPackedPixels     packed_pixels = 0;
                    int                 j;
                    
                    p = &unpacked_pixels[0];
                    p_len = sizeof(unpacked_pixels);
                    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem32Bit, CGW_PIXELS_PER_ATOM, kCGWDecoderItemNull);
                    switch ( enc_err ) {
                        case kCGWDecoderErrorNone:
                        case kCGWDecoderErrorInvalidItem:
                        case kCGWDecoderErrorBadSignature:
                            break;
                        case kCGWDecoderErrorInsufficientBytes:
                            fprintf(stderr, "ERROR:  Insufficient data for a CGWTile\n");
                            return false;
                    }
                    j = CGW_PIXELS_PER_ATOM;
                    while ( j-- > 0 ) {
                        packed_pixels = (packed_pixels << CGW_BITS_PER_PIXEL) | (unpacked_pixels[j] & CGW_PIXEL_MASK);
                    }
                    out_tile->pixels[i] = packed_pixels;
                }
            }
            else if ( header.bits_per_pixel <= 64 ) {
                for ( i = 0; i < CGW_BASE_TILE_PACKED_PIXEL_COUNT; i++ ) {
                    uint64_t            unpacked_pixels[CGW_PIXELS_PER_ATOM];
                    CGWPackedPixels     packed_pixels = 0;
                    int                 j;
                    
                    p = &unpacked_pixels[0];
                    p_len = sizeof(unpacked_pixels);
                    enc_err = CGWDecoderExpect(decoder, true, &p, &p_len, kCGWDecoderItem64Bit, CGW_PIXELS_PER_ATOM, kCGWDecoderItemNull);
                    switch ( enc_err ) {
                        case kCGWDecoderErrorNone:
                        case kCGWDecoderErrorInvalidItem:
                        case kCGWDecoderErrorBadSignature:
                            break;
                        case kCGWDecoderErrorInsufficientBytes:
                            fprintf(stderr, "ERROR:  Insufficient data for a CGWTile\n");
                            return false;
                    }
                    j = CGW_PIXELS_PER_ATOM;
                    while ( j-- > 0 ) {
                        packed_pixels = (packed_pixels << CGW_BITS_PER_PIXEL) | (unpacked_pixels[j] & CGW_PIXEL_MASK);
                    }
                    out_tile->pixels[i] = packed_pixels;
                }
            }
            else {
                fprintf(stderr, "ERROR:  Unhandled byte size %lu for CGWPixel\n", sizeof(CGWPixel));
                return false;
            }
            break;
        }
    }
    return true;
}
