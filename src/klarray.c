
#include "kafkalite.h"

struct KLArray
{
	size_t elemSize;
	size_t capacity;
	size_t count;
	char *buffer;
};

/**
 * Creates a new array.
 */
KLArray *kl_array_new(size_t elemSize, size_t capacity)
{
	KLArray *out = calloc(1, sizeof(KLArray));
	out->elemSize = elemSize;
	if (capacity <= 0)
		capacity = 32;
	out->capacity = capacity;
	out->buffer = calloc(capacity, elemSize);
	return out;
}

/**
 * Destroys an array.
 */
void kl_array_destroy(KLArray *array)
{
	if (array)
	{
		free(array->buffer);
		array->buffer = NULL;
		free(array);
	}
}

/**
 * Gets the number of elements in the array.
 */
size_t kl_array_count(KLArray *array)
{
	return array ? array->count : 0;
}

/**
 * Gets the capacity of the array (in number of elements).
 */
size_t kl_array_capacity(KLArray *array)
{
	return array ? array->capacity : 0;
}

/**
 * Ensures that there are enough space for the extra number of elements in the
 * array.
 */
bool kl_array_ensure_capacity(KLArray *array, size_t extraCapacity)
{
	if (array == NULL)
		return 0;
	size_t newCapacity = array->count + extraCapacity;
 	if (array->capacity < newCapacity)
	{
		newCapacity *= 1.25;
		array->buffer = realloc(array->buffer, newCapacity * array->elemSize);
		array->capacity = newCapacity;
	}
	return 1;
}

/**
 * Returns the size of each element in the array
 */
size_t kl_array_element_size(KLArray *array)
{
	return array ? array->elemSize : 0;
}

/**
 * Returns a pointer to the element at a particular index in the array.
 */
void *kl_array_element_at(KLArray *array, off_t index)
{
	return array ? array->buffer + (array->elemSize * index) : NULL;
}

/**
 * Returns a random element.
 */
void *kl_array_random(KLArray *array)
{
	if (!array)
		return NULL;

    off_t msgIndex = rand() % array->count;
	return array->buffer + (array->elemSize * msgIndex);
}

/**
 * Removes an element at a given index.
 */
void kl_array_remove_at(KLArray *array, off_t index)
{
	if (array && index >= 0 && index < array->count)
	{
		if (index != array->count - 1)
		{
			off_t srcIndex = array->elemSize * (index + 1);
			off_t dstIndex = array->elemSize * index;
			off_t remElems = array->count - (index + 1);
			memcpy(array->buffer + dstIndex, array->buffer + srcIndex, remElems * array->elemSize);
		}
		array->count--;
	}
}

/**
 * Inserts a value at a given index and returns the pointer to it 
 * so the caller can fill it as they see fit.
 */
void *kl_array_insert_at(KLArray *array, off_t index)
{
	if (!kl_array_ensure_capacity(array, 1))
		return NULL;

	void *out = NULL;
	if (index < 0 || index > array->count)
	{
		out = array->buffer + (array->elemSize * array->count);
	} else {
		off_t srcIndex = array->elemSize * index;
		off_t dstIndex = array->elemSize * (index + 1);
		off_t remElems = array->count - index;
		memcpy(array->buffer + dstIndex, array->buffer + srcIndex, remElems * array->elemSize);
	}
	array->count++;
	return out;
}

/**
 * Returns a pointer to the first element in the array.
 */
void *kl_array_front(KLArray *array)
{
	return array ? array->buffer : NULL;
}

/**
 * Returns a pointer to the last element in the array.
 */
void *kl_array_back(KLArray *array)
{
	return array ? (array->buffer + (array->elemSize * (array->count - 1))) : NULL;
}

