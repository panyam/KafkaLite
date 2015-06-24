
#include "kafkalite.h"

struct KLArray
{
	size_t elemSize;
	int capacity;
	int count;
	char *buffer;
};

/**
 * Creates a new array.
 */
KLArray *kl_array_new(size_t elemSize, int capacity)
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
unsigned kl_array_count(KLArray *array)
{
	return array ? array->count : 0;
}

/**
 * Gets the capacity of the array (in number of elements).
 */
unsigned kl_array_capacity(KLArray *array)
{
	return array ? array->capacity : 0;
}

/**
 * Ensures that there are enough space for the extra number of elements in the
 * array.
 */
bool kl_array_ensure_capacity(KLArray *array, unsigned extraCapacity)
{
	if (array == NULL)
		return 0;
	int newCapacity = array->count + extraCapacity;
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
void *kl_array_element_at(KLArray *array, int index)
{
	return array ? array->buffer + (array->elemSize * index) : NULL;
}

/**
 * Removes an element at a given index.
 */
void kl_array_remove_at(KLArray *array, int index)
{
	if (array && index >= 0 && index < array->count)
	{
		if (index != array->count - 1)
		{
			unsigned srcIndex = array->elemSize * (index + 1);
			unsigned dstIndex = array->elemSize * index;
			unsigned remElems = array->count - (index + 1);
			memcpy(array->buffer + dstIndex, array->buffer + srcIndex, remElems * array->elemSize);
		}
		array->count--;
	}
}

/**
 * Inserts a value at a given index and returns the pointer to it 
 * so the caller can fill it as they see fit.
 */
void *kl_array_insert_at(KLArray *array, int index)
{
	if (!kl_array_ensure_capacity(array, 1))
		return NULL;

	void *out = NULL;
	if (index < 0 || index > array->count)
	{
		out = array->buffer + (array->elemSize * array->count);
	} else {
		unsigned srcIndex = array->elemSize * index;
		unsigned dstIndex = array->elemSize * (index + 1);
		unsigned remElems = array->count - index;
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

