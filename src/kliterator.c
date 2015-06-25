
#include "klprivate.h"

/**
 * Creates a new iterator over the topic from a given message index.
 */
KLIterator *kl_iterator_new(KLContext *context, const char *topic, int offset)
{
	KLIterator *iterator = calloc(1, sizeof(KLIterator));
	iterator->topic = kl_topic_open(context, topic);
	return iterator;
}

/**
 * Destroys and closes the iterator
 */
void kl_iterator_destroy(KLIterator *iterator)
{
	if (iterator)
	{
		kl_topic_close(iterator->topic);
		free(iterator);
	}
}

/**
 * Gets the current index pointed by the iterator.
 */
size_t kl_iterator_index(KLIterator *iterator)
{
	return iterator ? iterator->currIndex : 0;
}

/**
 * Gets the current offset in the iterator.
 */
size_t kl_iterator_offset(KLIterator *iterator)
{
	return iterator ? iterator->currOffset : 0;
}

/**
 * Moves the iterator forward if there are any items left.
 * Optionally can block for more items to be produced while going forward.
 */
bool kl_iterator_forward(KLIterator *iterator, bool block)
{
	return false;
}

/**
 * Moves the iterator backard if there are any items left.
 * Returns true if rewinding was successful otherwise false.
 */
bool kl_iterator_rewind(KLIterator *iterator, bool block)
{
	if (iterator->currIndex <= 0)
		return false;
	return true;
}

/**
 * Gets the metadata of the current message pointed by the iterator.
 */
KLMessageInfo kl_iterator_current_metadata(KLIterator *iterator)
{
	return iterator->currMessageInfo;
}

/**
 * Gets the current message pointed by the iterator.  The caller must 
 * provide a buffer big enough to accomodate the data (whose size can be 
 * fetched from the kl_iterator_current_metadata method).
 */
size_t kl_iterator_current_message(KLIterator *iterator, char *output)
{
	return 0;
}

