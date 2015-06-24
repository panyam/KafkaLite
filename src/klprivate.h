
#ifndef __KL_PRIVATE_H__
#define __KL_PRIVATE_H__

#include "kafkalite.h"

struct KLContext
{
	/**
	 * Base dir where all messages are written to.
	 */
	char *baseDir;

	/**
	 * Directory where topics are saved.
	 */
	char *topicsDir;

	/**
	 * The factory to manage mutexes.
	 */
	KLMutexFactory *mutexFactory;

	/**
	 * Topics in this group.
	 */
	KLArray *topics;

	/**
	 * The mutex to safeguard the list of topics in this group.
	 */
	void *topicsMutex;
};

struct KLTopic
{
	/**
	 * Context to which this topic belongs.
	 */
	KLContext *context;

	/**
	 * Name of the topic.
	 */
	const char *name;

	/**
	 * Number of messages in the topic.
	 */
	size_t numMessages;

	/**
	 * Reference count of this topic.
	 */
	int refCount;

	FILE *dataFile;
	FILE *indexFile;

	/**
	 * Data is copied to this buffer and only flushed to disk if this buffer
	 * is exceeded.
	 */
	KLBuffer *dataBuffer;
	KLBuffer *indexBuffer;

	/**
	 * Current message offset.
	 */
	size_t currOffset;
};

#endif

