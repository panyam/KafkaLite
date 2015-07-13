

#ifndef __KL_ITERATOR_H__
#define __KL_ITERATOR_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a new iterator over the topic from a given message index.
 */
extern KLIterator *kl_iterator_new(KLContext *context, const char *topic, int offset);

/**
 * Destroys and closes the iterator
 */
extern void kl_iterator_destroy(KLIterator *iterator);

/**
 * Gets the current index pointed by the iterator.
 */
extern size_t kl_iterator_index(KLIterator *iterator);

/**
 * Gets the current offset in the iterator.
 */
extern size_t kl_iterator_offset(KLIterator *iterator);

/**
 * Gets the size of the message currently pointed by the iterator
 */
extern size_t kl_iterator_msgsize(KLIterator *iterator);

/**
 * Moves the iterator forward if there are any items left.
 * Optionally can block for more items to be produced while going forward.
 */
extern bool kl_iterator_forward(KLIterator *iterator);

/**
 * Moves the iterator backard if there are any items left.
 * Returns true if rewinding was successful otherwise false.
 */
extern bool kl_iterator_rewind(KLIterator *iterator);

/**
 * Gets the metadata of the current message pointed by the iterator.
 */
extern KLMessageMetadata *const kl_iterator_metadata(KLIterator *iterator);

/**
 * Gets the current message pointed by the iterator.  The caller must 
 * provide a buffer big enough to accomodate the data (whose size can be 
 * fetched from the kl_iterator_metadata method).
 */
extern size_t kl_iterator_message(KLIterator *iterator, KLMessage *message);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

