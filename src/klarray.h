
#ifndef __KL_ARRAY_LIST_H__
#define __KL_ARRAY_LIST_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a new array.
 */
extern KLArray *kl_array_new(UInt64 elemSize, UInt64 capacity);

/**
 * Destroys an array.
 */
extern void kl_array_destroy(KLArray *array);

/**
 * Gets the number of elements in the array.
 */
extern UInt64 kl_array_count(KLArray *array);

/**
 * Gets the capacity of the array (in number of elements).
 */
extern UInt64 kl_array_capacity(KLArray *array);

/**
 * Ensures that there are enough space for the extra number of elements in the
 * array.
 */
extern bool kl_array_ensure_capacity(KLArray *array, UInt64 extraCapacity);

/**
 * Returns the size of each element in the array
 */
extern UInt64 kl_array_element_size(KLArray *array);

/**
 * Returns a pointer to the element at a particular index in the array.
 */
extern void *kl_array_element_at(KLArray *array, Int64 index);

/**
 * Returns a random element.
 */
extern void *kl_array_random(KLArray *array);

/**
 * Removes an element at a given index.
 */
extern void kl_array_remove_at(KLArray *array, Int64 index);

/**
 * Inserts a value at a given index and returns the pointer to it 
 * so the caller can fill it as they see fit.
 */
extern void *kl_array_insert_at(KLArray *array, Int64 index);

/**
 * Returns a pointer to the first element in the array.
 */
extern void *kl_array_front(KLArray *array);

/**
 * Returns a pointer to the last element in the array.
 */
extern void *kl_array_back(KLArray *array);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

