

#include "kafkalite.h"

struct KLBuffer
{
	size_t capacity;
	size_t length;
	char *data;
};

/**
 * Creates a new buffer.
 */
KLBuffer *kl_buffer_new(size_t capacity)
{
	KLBuffer *out = calloc(1, sizeof(KLBuffer));
	if (capacity <= 0)
		capacity = 1024 * 1024;
	out->length = 0;
	out->capacity = capacity;
	out->data = malloc(capacity);
	return out;
}

/**
 * Destroys an buffer.
 */
void kl_buffer_destroy(KLBuffer *buffer)
{
	if (buffer)
	{
		free(buffer->data);
		buffer->data = NULL;
		free(buffer);
	}
}

/**
 * Clears the buffer and resets to the position to the start.
 */
void kl_buffer_reset(KLBuffer *buffer)
{
	if (buffer)
		buffer->length = 0;
}

/**
 * Gets the number of bytes in the buffer.
 */
size_t kl_buffer_size(KLBuffer *buffer)
{
	return buffer ? buffer->length : 0;
}

/**
 * Gets the available capacity of the buffer.
 */
size_t kl_buffer_capacity(KLBuffer *buffer)
{
	return buffer ? buffer->capacity : 0;
}

/**
 * Ensures that there are enough space for the extra number of 
 * bytes in the buffer.
 */
bool kl_buffer_ensure_capacity(KLBuffer *buffer, size_t extraBytes)
{
	if (!buffer)
		return false;
	int newCapacity = buffer->length + extraBytes;
 	if (buffer->capacity < newCapacity)
	{
		newCapacity *= 1.25;
		buffer->data = realloc(buffer->data, newCapacity);
		buffer->capacity = newCapacity;
	}
	return true;
}

/**
 * Returns the underlying bytes of the buffer.
 */
const char *kl_buffer_bytes(KLBuffer *buffer)
{
	return buffer ? buffer->data : NULL;
}

/**
 * Appends data to the end of the buffer.
 */
void kl_buffer_append(KLBuffer *buffer, const char *data, size_t datasize)
{
	if (kl_buffer_ensure_capacity(buffer, datasize))
	{
		memcpy(buffer->data + buffer->length, data, datasize);
		buffer->length += datasize;
	}
}
