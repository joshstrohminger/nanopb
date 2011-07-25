#ifndef _PB_H_
#define _PB_H_

#include <stdint.h>
#include <stddef.h> // size_t

/* Lightweight input stream.
 * If buf is NULL, read but don't store bytes. */
typedef struct _pb_istream_t pb_istream_t;
struct _pb_istream_t
{
    bool (*callback)(pb_istream_t *stream, char *buf, size_t count);
    void *state; // Free field for use by callback implementation
    size_t bytes_left;
};

static inline bool pb_read(pb_istream_t *stream, char *buf, size_t count)
{
    bool status = stream->callback(stream, buf, count);
    stream->bytes_left -= count;
    return status;
}

/* Lightweight output stream. */
typedef struct _pb_ostream_t pb_ostream_t;
struct _pb_ostream_t
{
    bool (*callback)(pb_ostream_t *stream, const char *buf, size_t count);
    void *state; // Free field for use by callback implementation
    size_t bytes_written;
};

static inline bool pb_write(pb_ostream_t *stream, const char *buf, size_t count)
{
    bool status = stream->callback(stream, buf, count);
    stream->bytes_written += count;
    return status;
}

/* List of possible decode/encode action types */

typedef enum {
    // Special case. Sets boolean field to true, continues parsing the data.
    PB_ACT_HAS,
    
    // Standard integer types
    PB_ACT_UINT32,
    PB_ACT_SINT32,
    PB_ACT_INT32,
    PB_ACT_FIXED32,
    PB_ACT_SFIXED32,
    PB_ACT_UINT64,
    PB_ACT_SINT64,
    PB_ACT_INT64,
    PB_ACT_FIXED64,
    PB_ACT_SFIXED64,
    PB_ACT_BOOL,
    
    // Standard float types
    PB_ACT_FLOAT,
    PB_ACT_DOUBLE,
    
    // Constant-sized array
    PB_ACT_BYTES,
    
    // Constant-sized array, with null termination
    PB_ACT_STRING,
    
    // Callback function pointer in field value
    PB_ACT_SUBMESSAGE,
    
    PB_LAST_ACT
} pb_action_t;

// This structure is used in constants to specify struct fields.
typedef struct {
    int field_number;
    uint16_t offset;
    pb_action_t action;
    uint8_t fieldsize;
} pb_field_t;

#define PB_LAST_FIELD {0,0,0,0}

/* --- Types to use inside generated structures. --- */

// Byte array and size word.
// Note: because of variable length array, this type cannot be directly used.
// Autogenerated code declares the same type of fields but with explicit length.
typedef struct {
    size_t size;
    char bytes[];
} pb_bytearray_t;

// This structure is used for giving the callback function.
typedef struct _pb_callback_t pb_callback_t;
struct _pb_callback_t {
    union {
        bool (*decode)(pb_istream_t *stream, const pb_field_t *field, void *arg);
        bool (*encode)(pb_ostream_t *stream, const pb_field_t *field, void *arg);
    } funcs;
    
    // Free arg for use by callback
    void *arg;
};

#endif