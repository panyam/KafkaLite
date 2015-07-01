
#ifndef __KL_BUFFER_H__
#define __KL_BUFFER_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a new buffer with a given capacity.
 */
extern KLBuffer *kl_buffer_new(size_t capacity);

/**
 * Destroys an buffer.
 */
extern void kl_buffer_destroy(KLBuffer *buffer);

/**
 * Gets the number of bytes in the buffer.
 */
extern size_t kl_buffer_size(KLBuffer *buffer);

/**
 * Gets the available capacity of the buffer.
 */
extern size_t kl_buffer_capacity(KLBuffer *buffer);

/**
 * Ensures that there are enough space for the extra number of elements in the
 * buffer.
 */
extern bool kl_buffer_ensure_capacity(KLBuffer *buffer, size_t extraBytes);

/**
 * Returns the underlying bytes of the buffer.
 */
extern char *kl_buffer_bytes(KLBuffer *buffer);

/**
 * Appends data to the end of the buffer.
 */
extern void kl_buffer_append(KLBuffer *buffer, const char *data, size_t datasize);

/**
 * Copy a portion of the buffer to the output buffer and returns the number of
 * bytes copied.
 */
extern size_t kl_buffer_copy(KLBuffer *buffer, off_t srcOffset, char *output, size_t numBytes);

/**
 * Clears the buffer and resets to the position to the start.
 */
extern void kl_buffer_reset(KLBuffer *buffer);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

