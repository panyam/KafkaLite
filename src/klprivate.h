
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
	char *name;

	/**
	 * Number of messages in the topic.
	 */
	size_t currIndex;

	/**
	 * Current message offset.
	 */
	size_t currOffset;

	/**
	 * The last offset at which a flush happened.
	 */
	size_t flushedAtIndex;

	/**
	 * The last offset at which a flush happened.
	 */
	size_t flushedAtOffset;

	/**
	 * Seperate data, index and metadata file per topic.
	 */
	int dataFile;
	int indexFile;
	int metadataFile;

	/**
	 * Buffers to write data to prevent frequent writes to disk.
	 */
	KLBuffer *dataBuffer;
	KLBuffer *indexBuffer;

	/**
	 * Only flush to disk when the number of published messages exceeds this
	 * threshold (in bytes).
	 */
	size_t flushThreshold;

	/**
	 * Reference count of this topic.
	 */
	int refCount;

	/**
	 * A lock for the file positions for each of the topic files.
	 */
	void *filePosLock;
};

/**
 * Topic iterators.
 */
struct KLIterator
{
	KLTopic *topic;

	/**
	 * Whether the current entry is loaded or not.
	 */
	size_t loadedCurrent;

	/**
	 * Current offset pointed by the interator.
	 */
	size_t currIndex;

	/**
	 * A buffer holding info about 
	 */
	KLMessageInfo currMessageInfo;
};


#endif

