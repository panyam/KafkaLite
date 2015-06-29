
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
    UInt64 currIndex;

    /**
     * Current message offset.
     */
    UInt64 currOffset;

    /**
     * The last offset at which a flush happened.
     */
    UInt64 flushedAtIndex;

    /**
     * The last offset at which a flush happened.
     */
    UInt64 flushedAtOffset;

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
    UInt64 flushThreshold;

    /**
     * Reference count of this topic.
     */
    int refCount;

    /**
     * A lock for the file positions for each of the topic files.
     */
    void *filePosLock;
	Int64 filePos;
	bool usePread;
	bool usePwrite;
};

/**
 * Topic iterators.
 */
struct KLIterator
{
    KLTopic *topic;

    /**
     * Current offset pointed by the interator.
     */
    UInt64 currIndex;

    /**
     * A buffer holding info about 
     */
    KLMessageHeader currMessageHeader;
};


#endif

