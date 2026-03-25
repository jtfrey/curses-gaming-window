/**
 * @file CGWDecoder.h
 * @brief Curses Gaming Window, Object Encoding
 * 
 * An API that marshalls binary data between the runtime and
 * storage environments.
 *
 */

#ifndef __CGWDECODER_H__
#define __CGWDECODER_H__

#include "CGWUtil.h"

/**
 * Opaque reference to an object decoder
 * Decoders are internally structured like a class cluster:
 * each concrete implementation has its own member data and
 * methods.  The organzation of those fields is hidden from
 * callers and accessor functions are made available to access
 * them.
 */
typedef struct CGWDecoder * CGWDecoderRef;

/**
 * Create an decoder that reads a sequence of bytes in memory
 * Create an decoder that wraps the sequence of \p n_bytes starting at
 * \p base_pointer in memory.
 * @param base_pointer  pointer to a sequence of bytes
 * @param n_bytes       number of bytes present in the sequence
 * @return              the new CGWDecoder or NULL is any error occurred
 */
CGWDecoderRef CGWDecoderCreateWithBytes(const void *base_pointer, size_t n_bytes);

/**
 * Create an decoder that reads a sequence of bytes in a file
 * Create an decoder that consumes bytes from a C FILE stream
 * \p fptr.  Binary i/o is performed:  e.g. an integer is read as
 * a raw sequence of bytes that may be byte-swapped.
 * @param fptr              the C FILE stream to read
 * @param should_not_close  when the CGWDecoder is destroyed, do NOT
 *                          close the C FILE stream
 * @return                  the new CGWDecoder or NULL if any error
 *                          occurred
 */
CGWDecoderRef CGWDecoderCreateWithFile(FILE *fptr, bool should_not_close);

/**
 * Create an decoder that reads a sequence of characters in a file
 * Create an decoder that consumes characters from a C FILE stream
 * \p fptr.  An integer is read as text and converted to a binary
 * representation (a'la \ref scanf()) so byte-swapping is never
 * performed on a text file decoder.
 * @param fptr              the C FILE stream to read
 * @param should_not_close  when the CGWDecoder is destroyed, do NOT
 *                          close the C FILE stream
 * @return                  the new CGWDecoder or NULL if any error
 *                          occurred
 */
CGWDecoderRef CGWDecoderCreateWithTextFile(FILE *fptr, bool should_not_close);

/**
 * Destroy an decoder
 * Shutdown and deallocate a previously-created decoder.
 * @param decoder       the CGWDecoder to destroy
 */
void CGWDecoderDestroy(CGWDecoderRef decoder);

/**
 * Reset an decoder to the beginning of its data
 * For a file-based decoder, the file read pointer is
 * repositioned to the start of the file; for a byte-based
 * decoder, the internal pointer to the next read location
 * is reset to the base pointer.
 * @param decoder       the decoder to reset
 * @return              boolean true if successful, false
 *                      othrwise
 */
bool CGWDecoderRewind(CGWDecoderRef decoder);

/**
 * Get the total number of bytes in the decoder
 * For a file-based decoder, returns the total length of
 * the file; for a byte-based decoder, returns the
 * size that was included as part of the decoder creation.
 * @param decoder       the decoder to query
 * @return              byte count (or a negative value
 *                      on error)
 */
ssize_t CGWDecoderGetTotalLength(CGWDecoderRef decoder);

/**
 * Get the current read offset within the decoder
 * Returns the index of the current read postion relative
 * to the start of the decoder's data.
 * @param decoder       the decoder to query
 * @return              offset index (or a negative value
 *                      on error)
 */
off_t CGWDecoderGetPosition(CGWDecoderRef decoder);

/**
 * Set the current read offset within the decoder
 * Attempt to reposition the decoder to next read at an
 * offset of \p offset from the start of its data.
 * @param decoder       the decoder to query
 * @param offset        index relative to the start of
 *                      data
 * @return              boolean true if successful,
 *                      false otherwise
 */
bool CGWDecoderSetPosition(CGWDecoderRef decoder, off_t offset);

/**
 * Read bytes from the decoder
 * Attempt to read \p count bytes from \p decoder, placing
 * them in a buffer starting at \p dst.  The caller must
 * ensure the buffer is at least \p count bytes in size.
 * @param decoder       the decoder from which to read
 * @param dst           pointer to the buffer that will
 *                      accept the bytes read
 * @param count         the number of bytes to read
 * @return              boolean true if successful, false
 *                      otherwise
 */
bool CGWDecoderGetBytes(CGWDecoderRef decoder, void *dst, size_t count);

/**
 * Items that can be read from an decoder
 * Enumerates the kinds of data that can be read from an
 * decoder.
 */
typedef enum {
    kCGWDecoderItemNull = 0,    /*!< sentinel that is used to end a list of items */
    kCGWDecoderItemSignature,   /*!< a 4-byte signature that identifies the data that follows */
    kCGWDecoderItem8Bit,        /*!< a single byte */
    kCGWDecoderItem16Bit,       /*!< a two-byte word */
    kCGWDecoderItem32Bit,       /*!< a four-byte word */
    kCGWDecoderItem64Bit,       /*!< an eight-byte word */
    kCGWDecoderItemUChar,       /*!< an unsigned character (textual data) */
    kCGWDecoderItemFloat,       /*!< a single-precision floating-point value */
    kCGWDecoderItemDouble,      /*!< a double-precision floating-point value */
    kCGWDecoderItemMax          /*!< sentinel that indicates the total number of items in this
                                     enumeration */
} CGWDecoderItem;

/**
 * Decoder errors
 * Enumeration of errors that can happen during decoder
 * operation.
 */
typedef enum {
    kCGWDecoderErrorNone = 0,           /*!< no error occurred */
    kCGWDecoderErrorBadSignature,       /*!< the expected signature was not found */
    kCGWDecoderErrorInvalidItem,        /*!< an invalid item kind was requested */
    kCGWDecoderErrorInsufficientBytes   /*!< the provided buffer does not contain
                                             enough room for the data being read */
} CGWDecoderError;

/**
 * Read value(s) from an decoder
 * The buffer at \p *dst that is \p *dst_len bytes in size is filled
 * with items from \p decoder as specified in the variable argument
 * list to the function.
 *
 * A kCGWDecoderItemSignature has a mandatory additional argument
 * that is the expected 32-bit signature.  The \p decoder must have
 * a 32-bit value available and it must match that expected signature.
 * The 32-bit signature is NOT read into \p *dst!  Byte-swapping
 * happens regardless of should_byte_swap.  E.g. to check that a
 * 'PLTT' signature is next in the decoder:
 *
 *     CGWDecoderExpect(an_decoder, &b, &b_len, true,
 *          kCGWDecoderItemSignature, 'PLTT',
 *          kCGWDecoderItemNull);
 *
 * A kCGWDecoderItem8Bit et al. item has a mandatory additional
 * argument that is the number of values of that type to pull from
 * \p decoder into \p *dst.  The \p decoder must have the expected
 * number of bytes available.  E.g. to read a sequence of 8 32-bit
 * values:
 *
 *     CGWDecoderExpect(an_decoder, &b, &b_len,
 *          kCGWDecoderItem32Bit, 8,
 *          kCGWDecoderItemNull);
 *
 * The sequence of items with their additional arguments MUST be
 * followed by a kCGWDecoderItemNull.
 *
 * The value of *dst and *dst_len are updated to reflect the
 * position in *dst following whatever this routine read
 *
 * @param decoder           the decoder from which to pull data
 * @param should_byte_swap  ensure multibyte values are byte-swapped to
 *                          host- from big-endian
 * @param dst               pointer to a pointer to a buffer to fill
 * @param dst_len           pointer to a count of the bytes in the buffer
 *                          to fill
 * @return                  an decoder error value
 */
CGWDecoderError CGWDecoderExpect(CGWDecoderRef decoder, bool should_byte_swap,
                                                    void **dst, size_t *dst_len, ...);

#endif /* __CGWDECODER_H__ */