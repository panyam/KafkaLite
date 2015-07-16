
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
     * The factory to manage locks etc.
     */
    KLLockManager *lockManager;

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
    off_t currIndex;

    /**
     * The last offset at which a flush happened.
     */
    off_t flushedAtIndex;

    /**
     * Current message offset.
     */
    size_t currOffset;

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
	off_t filePos;
	bool usePread;
	bool usePwrite;
};


#define NUM_MESSAGE_METADATAS	8192

/**
 * Topic iterators.
 */
struct KLIterator
{
    KLTopic *topic;

    /**
     * Current offset pointed by the interator.
     */
    off_t currIndex;

    /**
     * A buffer holding a bunch of message headers in one go
     */
    KLMessageMetadata messageMetadatas[NUM_MESSAGE_METADATAS];

	/**
	 * Current message header we are upto.
	 */
	KLMessageMetadata *currMessageMetadata;

	/**
	 * Pointer to the last message metadata
	 */
	KLMessageMetadata *lastMessageMetadata;
};


#endif

