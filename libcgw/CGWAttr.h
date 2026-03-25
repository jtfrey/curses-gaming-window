/**
 * @file CGWAttr.h
 * @brief Curses Gaming Window, Graphics attributes
 * 
 * A header file that defines graphics attributes bit
 * vectors.
 *
 */

#ifndef __CGWATTR_H__
#define __CGWATTR_H__

#include "CGWConfig.h"

#if CGW_ATTR_BITS > 32
#   error Graphics attributes exceeding 32-bits are not implemented.
#elif CGW_ATTR_BITS > 16
/**
 * Type of graphics attributes
 * The bit-depth of the type is influenced by the configured value
 * of \ref CGW_ATTR_BITS.  It can be 32-, 16-, or 8-bit.
 */
#   define CGWAttr uint32_t
/**
 * Type used when decoding attributes
 * When a graphics attribute is read from a decoding stream,
 * this type will be used.
 */
#   define CGWAttrCodingType kCGWDecoderItem32Bit
/**
 * Format string associated with graphics attributes
 * Use this string fragment when displaying graphics attributes
 * in printf() functions.
 */
#   define CGWAttrFmt "0x%08X"
#elif CGW_ATTR_BITS > 8
#   define CGWAttr uint16_t
#   define CGWAttrCodingType kCGWDecoderItem16Bit
#   define CGWAttrFmt "0x%04X"
#else
#   define CGWAttr uint8_t
#   define CGWAttrCodingType kCGWDecoderItem8Bit
#   define CGWAttrFmt "0x%02X"
#endif

/**
 * Palette index graphics attribute mask
 * A palette index will always occupy the lowest bits of a
 * graphics attribute word.  This macro provides the bitmask
 * for that portion of the word.
 */
#define CGWAttrPaletteMask ((CGWAttr)CGW_PALETTE_MASK)

/**
 * Flip tile pixels vertically
 * The pixels in a graphics tile should be enumerated
 * from bottom-left to top-right, effectively reflecting
 * the image across an internal horizontal mirror
 * plane.
 *
 * Combined with \ref CGWAttrHFlip, the enumeration
 * will instead run from bottom-right to top-left.
 */
#define CGWAttrVFlip (CGWAttrPaletteMask + 1)

/**
 * Flip tile pixels horizontally
 * The pixels in a graphics tile should be enumerated
 * from top-right to bottom-left, effectively reflecting
 * the image across an internal vertical mirror
 * plane.
 *
 * Combined with \ref CGWAttrVFlip, the enumeration
 * will instead run from bottom-right to top-left.
 */
#define CGWAttrHFlip (CGWAttrVFlip << 1)

/**
 * First unused attribute bit
 * This macro holds the lowest bit that is not part of the
 * palette index.  It can be used as the basis for all
 * additional graphics attributes.
 */
#define CGWAttrBaseUnusedBit (CGWAttrHFlip << 1)

/**
 * Unused bits graphics attribute mask
 * This macro provides the bitmask for the portion of a
 * graphics attribute word other than the palette index.
 */
#define CGWAttrUnusedMask ((CGWAttr)(~(CGWAttrBaseUnusedBit - 1)))

#endif /* __CGWATTR_H__ */