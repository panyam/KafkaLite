
#include "klprivate.h"

// We only need read access to the data.   Two ways of doing iterators:
//
// 1. Do a repeated seek/read pattern by putting a lock around them for
// concurrent access - this is ok in a single threaded mode and requires 0
// memory.
//
// 2. But with multiple consumers, doing seeks each time and having to lock 
// till a read finishes is going to bottleneck the reads.  Instead use a
// memory mapped files so that the consumers can read data in parallel.
// 
// Obviously this is significantly harder to manage.  There needs to be a limit
// on how many pages can be in memory across all the consumers.  If the
// consumers in sync this is easy to maintain.  If they are going at different
// rates this needs to be managed so that we dont start getting too many
// pages in memory.   

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
	return iterator ? iterator->currMessageInfo.offset : 0;
}

/**
 * Gets the size of the message currently pointed by the iterator
 */
size_t kl_iterator_msgsize(KLIterator *iterator)
{
	return iterator ? iterator->currMessageInfo.size : 0;
}

/**
 * Moves the iterator forward if there are any items left.
 * Optionally can block for more items to be produced while going forward.
 */
bool kl_iterator_forward(KLIterator *iterator, bool block)
{
	if (!iterator)
		return false;

	if (iterator->currIndex + 1 >= iterator->topic->currIndex)
	{
		if (block)
		{
			// TODO: implement blocking until a message is produced.
		}
		return false;
	}

	iterator->currIndex++;

	// TODO: consider using a circular buffer to keep keep more 
	// than one message info in memory.
	kl_topic_get_message_info(iterator->topic, iterator->currIndex, &iterator->currMessageInfo, 1);
	return true;
}

/**
 * Moves the iterator backard if there are any items left.
 * Returns true if rewinding was successful otherwise false.
 */
bool kl_iterator_rewind(KLIterator *iterator, bool block)
{
	if (iterator->currIndex <= 0)
		return false;

	iterator->currIndex--;

	// TODO: consider using a circular buffer to keep keep more 
	// than one message info in memory.
	kl_topic_get_message_info(iterator->topic, iterator->currIndex, &iterator->currMessageInfo, 1);
	return true;
}

/**
 * Gets the metadata of the current message pointed by the iterator.
 */
KLMessageInfo kl_iterator_metadata(KLIterator *iterator)
{
	return iterator->currMessageInfo;
}

/**
 * Gets the current message pointed by the iterator.  The caller must 
 * provide a buffer big enough to accomodate the data (whose size can be 
 * fetched from the kl_iterator_current_metadata method).
 */
size_t kl_iterator_message(KLIterator *iterator, char *output)
{
	return 0;
}

