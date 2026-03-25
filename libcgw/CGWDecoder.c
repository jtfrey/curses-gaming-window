/**
 * @file CGWDecoder.c
 * @brief Curses Gaming Window, Object Decoder
 * 
 * Source code to accompany CGWDecoder.h.
 *
 */

#include "CGWDecoder.h"

static size_t __CGWDecoderItemByteSize[] = {
        0,
        sizeof(uint32_t),
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t),
        sizeof(uint64_t),
        sizeof(unsigned char),
        sizeof(float),
        sizeof(double),
        0
    };

/**/

typedef void (*CGWDecoderDestroyCallback)(CGWDecoderRef decoder);
typedef bool (*CGWDecoderRewindCallback)(CGWDecoderRef decoder);
typedef ssize_t (*CGWDecoderGetTotalLengthCallback)(CGWDecoderRef decoder);
typedef off_t (*CGWDecoderGetPositionCallback)(CGWDecoderRef decoder);
typedef bool (*CGWDecoderSetPositionCallback)(CGWDecoderRef decoder, off_t offset);
typedef bool (*CGWDecoderGetBytesCallback)(CGWDecoderRef decoder, void *dst, size_t count, CGWDecoderItem type_hint);

typedef struct {
    const char                          *subtype;
    bool                                should_not_byte_swap;
    CGWDecoderDestroyCallback           destroy;
    CGWDecoderRewindCallback            rewind;
    CGWDecoderGetTotalLengthCallback    getTotalLength;
    CGWDecoderGetPositionCallback       getPosition;
    CGWDecoderSetPositionCallback       setPosition;
    CGWDecoderGetBytesCallback          getBytes;
} CGWDecoderCallbacks;

/**/

typedef struct CGWDecoder {
    const CGWDecoderCallbacks   *callbacks;
} CGWDecoder;

/**/

typedef struct {
    CGWDecoder     base;
    const void      *start_pointer;
    const void      *cur_pointer;
    const void      *end_pointer;
} CGWDecoderBytes;

bool
CGWDecoderBytesRewind(
    CGWDecoderRef       decoder
)
{
    CGWDecoderBytes     *THE_BUFFER = (CGWDecoderBytes*)decoder;
    
    THE_BUFFER->cur_pointer = THE_BUFFER->start_pointer;
    return true;
}

ssize_t
CGWDecoderBytesGetTotalLength(
    CGWDecoderRef       decoder
)
{
    CGWDecoderBytes     *THE_BUFFER = (CGWDecoderBytes*)decoder;
    return THE_BUFFER->end_pointer - THE_BUFFER->start_pointer;
}

off_t
CGWDecoderBytesGetPosition(
    CGWDecoderRef       decoder
)
{
    CGWDecoderBytes     *THE_BUFFER = (CGWDecoderBytes*)decoder;
    return (off_t)(THE_BUFFER->cur_pointer - THE_BUFFER->start_pointer);
}

bool
CGWDecoderBytesSetPosition(
    CGWDecoderRef       decoder,
    off_t               offset
)
{
    CGWDecoderBytes     *THE_BUFFER = (CGWDecoderBytes*)decoder;
    if ( THE_BUFFER->start_pointer + offset <= THE_BUFFER->end_pointer ) {
        THE_BUFFER->cur_pointer = THE_BUFFER->start_pointer + offset;
        return true;
    }
    return false;
}

bool
CGWDecoderBytesGetBytes(
    CGWDecoderRef       decoder,
    void                *dst,
    size_t              count,
    CGWDecoderItem      type_hint
)
{
    CGWDecoderBytes     *THE_BUFFER = (CGWDecoderBytes*)decoder;
    
    if ( THE_BUFFER->cur_pointer + count <= THE_BUFFER->end_pointer ) {
        memcpy(dst, THE_BUFFER->cur_pointer, count);
        THE_BUFFER->cur_pointer += count;
        return true;
    }
    return false;
}

static const CGWDecoderCallbacks CGWDecoderBytesCallbacks = {
                .subtype = "byte-buffer",
                .should_not_byte_swap = false,
                .destroy = NULL,
                .rewind = CGWDecoderBytesRewind,
                .getTotalLength = CGWDecoderBytesGetTotalLength,
                .getPosition = CGWDecoderBytesGetPosition,
                .setPosition = CGWDecoderBytesSetPosition,
                .getBytes = CGWDecoderBytesGetBytes
            };

/**/

typedef struct {
    CGWDecoder      base;
    FILE            *fptr;
    bool            should_not_close;
} CGWDecoderFile;

void
CGWDecoderFileDestroy(
    CGWDecoderRef       decoder
)
{
    CGWDecoderFile      *THE_BUFFER = (CGWDecoderFile*)decoder;
    
    if ( ! THE_BUFFER->should_not_close ) fclose(THE_BUFFER->fptr);
}

bool
CGWDecoderFileRewind(
    CGWDecoderRef       decoder
)
{
    CGWDecoderFile      *THE_BUFFER = (CGWDecoderFile*)decoder;
    
    return (fseek(THE_BUFFER->fptr, 0, SEEK_SET) == 0) ? true : false;
}

ssize_t
CGWDecoderFileGetTotalLength(
    CGWDecoderRef       decoder
)
{
    CGWDecoderFile      *THE_BUFFER = (CGWDecoderFile*)decoder;
    struct stat         finfo;
    
    if ( fstat(fileno(THE_BUFFER->fptr), &finfo) == 0 ) return (ssize_t)finfo.st_size;
    return -1;
}

off_t
CGWDecoderFileGetPosition(
    CGWDecoderRef       decoder
)
{
    CGWDecoderFile      *THE_BUFFER = (CGWDecoderFile*)decoder;
    return ftello(THE_BUFFER->fptr);
}

bool
CGWDecoderFileSetPosition(
    CGWDecoderRef       decoder,
    off_t               offset
)
{
    CGWDecoderFile      *THE_BUFFER = (CGWDecoderFile*)decoder;
    return (fseeko(THE_BUFFER->fptr, offset, SEEK_SET) == 0) ? true : false;
}

bool
CGWDecoderFileGetBytes(
    CGWDecoderRef       decoder,
    void                *dst,
    size_t              count,
    CGWDecoderItem      type_hint
)
{
    CGWDecoderFile      *THE_BUFFER = (CGWDecoderFile*)decoder;
    ssize_t             n_read = fread(dst, count, 1, THE_BUFFER->fptr);
    return (n_read == 1) ? true : false;
}

static const CGWDecoderCallbacks CGWDecoderFileCallbacks = {
                .subtype = "file-pointer",
                .should_not_byte_swap = false,
                .destroy = CGWDecoderFileDestroy,
                .rewind = CGWDecoderFileRewind,
                .getTotalLength = CGWDecoderFileGetTotalLength,
                .getPosition = CGWDecoderFileGetPosition,
                .setPosition = CGWDecoderFileSetPosition,
                .getBytes = CGWDecoderFileGetBytes
            };

/**/

bool
CGWDecoderTextFileGetBytes(
    CGWDecoderRef       decoder,
    void                *dst,
    size_t              count,
    CGWDecoderItem      type_hint
)
{
    CGWDecoderFile      *THE_BUFFER = (CGWDecoderFile*)decoder;
    int                 rc;
    
    switch ( type_hint ) {
        case kCGWDecoderItemNull:
        case kCGWDecoderItemMax:
            break;
            
        case kCGWDecoderItemSignature: {
            if ( count % 4 == 0 ) {
                char        signature[4] = { 0, 0, 0, 0 };
                uint32_t    *DST = (uint32_t*)dst;
            
                count = (count / 4);
                while ( count ) {
                    rc = fscanf(THE_BUFFER->fptr, " %4c", signature);
                    if ( rc == 1 ) {
                        memcpy(DST++, signature, 4);
                    } else {
                        break;
                    }
                    count--;
                }
                if ( count == 0 ) return true;
            }
            break;
        }
        
        case kCGWDecoderItemUChar: {
            unsigned char   *cbuf = (unsigned char*)dst;
            char            fmt[24];
            
            snprintf(fmt, sizeof(fmt), " %%%luc", count);
            memset(cbuf, 0, count);
            rc = fscanf(THE_BUFFER->fptr, fmt, cbuf);
            if ( rc == 1 ) return true;
            break;
        }
        
        case kCGWDecoderItemFloat: {
            float       *DST = (float*)dst;
            
            while ( count ) {
                rc = fscanf(THE_BUFFER->fptr, "%g", DST++);
                if ( rc != 1 ) break;
                count--;
            }
            if ( count == 0 ) return true;
            break;
        }
        
        case kCGWDecoderItemDouble: {
            double      *DST = (double*)dst;
            
            while ( count ) {
                rc = fscanf(THE_BUFFER->fptr, "%lg", DST++);
                if ( rc != 1 ) break;
                count--;
            }
            if ( count == 0 ) return true;
            break;
        }
        
        case kCGWDecoderItem8Bit: {
            uint8_t     *DST = (uint8_t*)dst;
            
            while ( count ) {
                rc = fscanf(THE_BUFFER->fptr, "%hhi", DST++);
                if ( rc != 1 ) break;
                count--;
            }
            if ( count == 0 ) return true;
            break;
        }
        
        case kCGWDecoderItem16Bit: {
            uint16_t    *DST = (uint16_t*)dst;
            
            if ( count % sizeof(uint16_t) == 0 ) {
                count /= sizeof(uint16_t);
                while ( count ) {
                    rc = fscanf(THE_BUFFER->fptr, "%hi", DST++);
                    if ( rc != 1 ) break;
                    count--;
                }
                if ( count == 0 ) return true;
            }
            break;
        }
        
        case kCGWDecoderItem32Bit: {
            uint32_t    *DST = (uint32_t*)dst;
            
            if ( count % sizeof(uint32_t) == 0 ) {
                count /= sizeof(uint32_t);
                while ( count ) {
                    rc = fscanf(THE_BUFFER->fptr, "%i", DST++);
                    if ( rc != 1 ) break;
                    count--;
                }
                if ( count == 0 ) return true;
            }
            break;
        }
        
        case kCGWDecoderItem64Bit: {
            uint64_t    *DST = (uint64_t*)dst;
            
            if ( count % sizeof(uint64_t) == 0 ) {
                count /= sizeof(uint64_t);
                while ( count ) {
                    rc = fscanf(THE_BUFFER->fptr, "%lli", DST++);
                    if ( rc != 1 ) break;
                    count--;
                }
                if ( count == 0 ) return true;
            }
            break;
        }
        
    }
    return false;
    
    
    ssize_t             n_read = fread(dst, count, 1, THE_BUFFER->fptr);
    return (n_read == 1) ? true : false;
}

static const CGWDecoderCallbacks CGWDecoderTextFileCallbacks = {
                .subtype = "text-file-pointer",
                .should_not_byte_swap = true,
                .destroy = CGWDecoderFileDestroy,
                .rewind = CGWDecoderFileRewind,
                .getTotalLength = CGWDecoderFileGetTotalLength,
                .getPosition = CGWDecoderFileGetPosition,
                .setPosition = CGWDecoderFileSetPosition,
                .getBytes = CGWDecoderTextFileGetBytes
            };

/**/

CGWDecoderRef
CGWDecoderCreateWithBytes(
    const void  *base_pointer,
    size_t      n_bytes
)
{
    CGWDecoderBytes     *new_buffer = (CGWDecoderBytes*)malloc(sizeof(CGWDecoderBytes));
    
    if ( new_buffer ) {
        new_buffer->base.callbacks = &CGWDecoderBytesCallbacks;
        new_buffer->start_pointer = new_buffer->cur_pointer = base_pointer;
        new_buffer->end_pointer = base_pointer + n_bytes;
    }
    return (CGWDecoderRef)new_buffer;
}

CGWDecoderRef
CGWDecoderCreateWithFile(
    FILE        *fptr,
    bool        should_not_close
)
{
    CGWDecoderFile      *new_buffer = (CGWDecoderFile*)malloc(sizeof(CGWDecoderFile));
    
    if ( new_buffer ) {
        new_buffer->base.callbacks = &CGWDecoderFileCallbacks;
        new_buffer->fptr = fptr;
        new_buffer->should_not_close = should_not_close;
    }
    return (CGWDecoderRef)new_buffer;
}

CGWDecoderRef
CGWDecoderCreateWithTextFile(
    FILE        *fptr,
    bool        should_not_close
)
{
    CGWDecoderFile      *new_buffer = (CGWDecoderFile*)malloc(sizeof(CGWDecoderFile));
    
    if ( new_buffer ) {
        new_buffer->base.callbacks = &CGWDecoderTextFileCallbacks;
        new_buffer->fptr = fptr;
        new_buffer->should_not_close = should_not_close;
    }
    return (CGWDecoderRef)new_buffer;
}

void
CGWDecoderDestroy(
    CGWDecoderRef   decoder
)
{
    if ( decoder->callbacks->destroy ) decoder->callbacks->destroy(decoder);
    free((void*)decoder);
}

bool
CGWDecoderRewind(
    CGWDecoderRef       decoder
)
{
    return ( decoder->callbacks->rewind ? decoder->callbacks->rewind(decoder) : false );
}

ssize_t
CGWDecoderGetTotalLength(
    CGWDecoderRef       decoder
)
{
    return ( decoder->callbacks->getTotalLength ? decoder->callbacks->getTotalLength(decoder) : -1 );
}

off_t
CGWDecoderGetPosition(
    CGWDecoderRef       decoder
)
{
    return ( decoder->callbacks->getPosition ? decoder->callbacks->getPosition(decoder) : -1 );
}

bool
CGWDecoderSetPosition(
    CGWDecoderRef       decoder,
    off_t               offset
)
{
    return ( decoder->callbacks->setPosition ? decoder->callbacks->setPosition(decoder, offset) : false );
}

bool
CGWDecoderGetBytes(
    CGWDecoderRef       decoder,
    void                *dst,
    size_t              count
)
{
    if ( ! dst ) return kCGWDecoderErrorInsufficientBytes;
    return ( decoder->callbacks->getBytes ? decoder->callbacks->getBytes(decoder, dst, count, kCGWDecoderItem8Bit) : false );
}

/**/

CGWDecoderError
CGWDecoderExpect(
    CGWDecoderRef       decoder,
    bool                should_byte_swap,
    void                **dst,
    size_t              *dst_len,
    ...
)
{
#define CGWDECODER_GET_BYTES(P, L) \
            ( decoder->callbacks->getBytes ? decoder->callbacks->getBytes(decoder, (P), (L), next_item) : false ) 
            
    void                *p = dst ? *dst : NULL;
    size_t              p_len = dst_len ? *dst_len : 0;
    CGWDecoderItem      next_item;
    va_list             argv;   
    CGWDecoderError     rc = kCGWDecoderErrorNone;
    
    /* Override byte swapping if the decoder doesn't call for it: */
    if ( decoder->callbacks->should_not_byte_swap ) should_byte_swap = false;

    va_start(argv, dst_len);
    while ( ! rc && ((next_item = va_arg(argv, CGWDecoderItem)) != kCGWDecoderItemNull) ) {
        if ( next_item >= kCGWDecoderItemMax ) {
            rc = kCGWDecoderErrorInvalidItem;
            break;
        }
        if ( next_item == kCGWDecoderItemSignature ) {
            uint32_t        read_signature;
            uint32_t        expect_signature = va_arg(argv, uint32_t);
            
            if ( CGWDECODER_GET_BYTES(&read_signature, sizeof(read_signature)) ) {
                if ( CGWEndianToHost32Bit(read_signature) != expect_signature ) {
                    rc = kCGWDecoderErrorBadSignature;
                }
            } else {
                rc = kCGWDecoderErrorInsufficientBytes;
            }
        } else {
            /* For sized types, there's an additional argument that is
             * a number of items to read: */
            int         n_items = va_arg(argv, int);
            
            if ( n_items > 0 ) {
                if ( CGWDECODER_GET_BYTES(p, n_items * __CGWDecoderItemByteSize[next_item]) ) {
                    if ( should_byte_swap && (CGEndianDetect() == kCGWEndianLE) ) {
                        switch ( next_item ) {
                            case kCGWDecoderItemNull:
                            case kCGWDecoderItemSignature:
                            case kCGWDecoderItemMax:
                                /* Will not get here, just here to shut-up clang */
                                break;
                            case kCGWDecoderItemUChar:
                            case kCGWDecoderItem8Bit: {
                                p += n_items;
                                p_len -= n_items;
                                break;
                            }
                            case kCGWDecoderItem16Bit: {
                                uint16_t    *i = (uint16_t*)p;
                                while ( n_items-- ) {
                                    *i = CGWEndianSwap16Bit(*i);
                                    i++, p_len -= sizeof(uint16_t);
                                }
                                p = (void*)i;
                                break;
                            }
                            case kCGWDecoderItemFloat:
                            case kCGWDecoderItem32Bit: {
                                uint32_t    *i = (uint32_t*)p;
                                while ( n_items-- ) {
                                    *i = CGWEndianSwap32Bit(*i);
                                    i++, p_len -= sizeof(uint32_t);
                                }
                                p = (void*)i;
                                break;
                            }
                            case kCGWDecoderItemDouble:
                            case kCGWDecoderItem64Bit: {
                                uint64_t    *i = (uint64_t*)p;
                                while ( n_items-- ) {
                                    *i = CGWEndianSwap64Bit(*i);
                                    i++, p_len -= sizeof(uint64_t);
                                }
                                p = (void*)i;
                                break;
                            }
                        }
                    } else {
                        p += n_items * __CGWDecoderItemByteSize[next_item];
                        p_len -= n_items * __CGWDecoderItemByteSize[next_item];
                    }
                } else {
                    rc = kCGWDecoderErrorInsufficientBytes;
                }
            }
        }
    }
    va_end(argv);
    if ( rc == kCGWDecoderErrorNone ) {
        if ( dst ) *dst = p;
        if ( dst_len ) *dst_len = p_len;
    }
    return rc;

#undef CGWDECODER_GET_BYTES
}

