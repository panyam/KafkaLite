
#include "klprivate.h"

#define ltell(fdes)        lseek((fdes), 0, SEEK_CUR)

void kl_topic_flush(KLTopic *topic);
size_t kl_topic_publish_single(KLTopic *topic, const char *msg, size_t msgsize);
off_t kl_topic_find_unlocked(KLContextRef context, const char *name);

size_t kl_read_file(KLTopic *topic, int fd, void *buff, size_t size, off_t offset)
{
    if (topic->usePread)
    {
        return pread(fd, buff, size, offset);
    } else {
        lseek(fd, offset, SEEK_SET);
        return read(fd, buff, size);
    }
}

void kl_topic_initialize(KLContext *context, KLTopic *topic, const char *name)
{
    topic->usePread = topic->usePwrite = true;
    topic->context = context;
    topic->name = strdup(name);
    topic->refCount = 0;
    topic->currIndex = 0;
    topic->currOffset = 0;
    topic->dataBuffer = kl_buffer_new(512 * 512);
    topic->indexBuffer = kl_buffer_new(32000);
    topic->flushThreshold = 1000000;        // flush from memory to disk once data in mem exceeds this
    topic->filePosLock = KL_MUTEX_NEW(context->lockManager, NULL);

    char buffer[2048];

    errno = 0;
    sprintf(buffer, "%s/%s.data", context->topicsDir, name);
    topic->dataFile = open(buffer, O_RDWR | O_CREAT, 0777);
    if (topic->indexFile < 0)
    {
        kl_log("Unable to open data file. error: %s", strerror(errno));
        return ;
    }

    errno = 0;
    sprintf(buffer, "%s/%s.metadata", context->topicsDir, name);
    topic->metadataFile = open(buffer, O_RDWR | O_CREAT, 0777);
    if (topic->metadataFile < 0)
    {
        kl_log("Unable to open metadata file. error: %s", strerror(errno));
        return ;
    }

    errno = 0;
    sprintf(buffer, "%s/%s.index", context->topicsDir, name);
    topic->indexFile = open(buffer, O_RDWR | O_CREAT, 0777);
    if (topic->indexFile < 0)
    {
        kl_log("Unable to open index file. error: %s", strerror(errno));
        return ;
    }

    // from the index file gather the number of messages and the num

    uint64_t values[2] = {0};
    kl_read_file(topic, topic->metadataFile, values, sizeof(values), 0);
    topic->currIndex = values[0];
    topic->currOffset = values[1];

    // go to where we can start writing to index file 
    // - only need to do this once per open
    lseek(topic->indexFile, topic->currIndex * sizeof(KLMessageMetadata), SEEK_SET);

    // now go to the point where we can start writing data
    // - only need to do this once per open
    lseek(topic->dataFile, topic->currOffset, SEEK_SET);
}

void kl_topic_finalize(KLTopic *topic)
{
    if (topic)
    {
        kl_log("\nClosing topic: %s", topic->name);
        // flush the in mem data if necessary
        kl_topic_flush(topic);

        KLContext *context = topic->context;
        topic->context = NULL;
        topic->refCount = 0;
        topic->currIndex = 0;
        topic->currOffset = 0;
        free(topic->name); topic->name = NULL;
        kl_buffer_destroy(topic->dataBuffer);
        kl_buffer_destroy(topic->indexBuffer);
        if (topic->dataFile != 0) close(topic->dataFile); topic->dataFile = 0;
        if (topic->indexFile != 0) close(topic->indexFile); topic->indexFile = 0;
        KL_MUTEX_DESTROY(context->lockManager, topic->filePosLock);
    }
}

/**
 * Opens a new topic into which messages can be published.
 * If a topic is opened multiple times then the same topic
 * object is returned.
 */
KLTopic *kl_topic_open(KLContextRef context, const char *name)
{
    KL_MUTEX_LOCK(context->lockManager, context->topicsMutex);

    // see if this topic is already opened
    // if so then just increment its count
    KLTopic *topic = NULL;
    off_t index = kl_topic_find_unlocked(context, name);
    if (index < 0)
    {
        topic = kl_array_insert_at(context->topics, -1);
        kl_topic_initialize(context, topic, name);
    } else {
        topic = kl_array_element_at(context->topics, index);
    }
    topic->refCount++;

    KL_MUTEX_UNLOCK(context->lockManager, context->topicsMutex);
    return topic;
}

/**
 * Closes a topic if it is opened.
 */
bool kl_topic_close(KLTopic *topic)
{
    if (!topic)
        return false;

    bool alive = true;
    KLContextRef context = topic->context;
    KL_MUTEX_LOCK(context->lockManager, context->topicsMutex);
    // see if this topic is already opened
    // if so then just increment its count
    off_t index = kl_topic_find_unlocked(context, topic->name);
    if (index >= 0)
    {
        topic = kl_array_element_at(context->topics, index);
        topic->refCount--;
        if (topic->refCount == 0)
        {
            alive = false;
            kl_topic_finalize(topic);
            kl_array_remove_at(context->topics, index);
        }
    }
    KL_MUTEX_UNLOCK(context->lockManager, context->topicsMutex);
    return alive;
}

/**
 * Publish a single message.
 */
size_t kl_topic_publish(KLTopic *topic, const char *msg, size_t msgsize)
{
    if (!topic) return 0;
    KLContextRef context = topic->context;
    KL_MUTEX_LOCK(context->lockManager, topic->filePosLock);
    size_t out = kl_topic_publish_single(topic, msg, msgsize);
    KL_MUTEX_UNLOCK(context->lockManager, topic->filePosLock);
    return out;
}

/**
 * Publish a batch of messages to the topic.
 */
size_t kl_topic_publish_multi(KLTopic *topic, size_t numMessages, const char *msgs[], size_t *msgsizes)
{
    if (!topic) return 0;
    KLContextRef context = topic->context;
    KL_MUTEX_LOCK(context->lockManager, topic->filePosLock);
    size_t out = 0;
    for (off_t i = 0;i < numMessages;i++)
    {
        out += kl_topic_publish_single(topic, msgs[i], msgsizes[i]);
    }
    KL_MUTEX_UNLOCK(context->lockManager, topic->filePosLock);
    return out;
}

size_t kl_topic_publish_single(KLTopic *topic, const char *msg, size_t msgsize)
{
    KLMessageMetadata info;
    info.offset = topic->currOffset;
    info.size = msgsize;

    size_t payloadSize = sizeof(msgsize) + msgsize;
    size_t unflushedLength = 0;

    if (topic->dataBuffer)
    {
        // TODO: if buffer has nothing and msg size > flush threshold
        // then write directly to disk - no need to copy to buffer 
        // first!

        // write to the buffer first
        uint64_t msgsize64 = msgsize;
        kl_buffer_append(topic->dataBuffer, (const char *)(&msgsize64), sizeof(msgsize64));
        kl_buffer_append(topic->dataBuffer, msg, msgsize);

        // write the message info first before updating the header
        kl_buffer_append(topic->indexBuffer, (const char *)(&info), sizeof(info));

        topic->currIndex++;
        topic->currOffset += payloadSize;

        unflushedLength = kl_buffer_size(topic->dataBuffer);
    } else {
        if (topic->usePwrite)
        {
            // write the message
            off_t off = topic->currOffset;
            uint64_t msgsize64 = msgsize;
            pwrite(topic->dataFile, (const char *)(&msgsize64), sizeof(msgsize64), off);
            off += sizeof(msgsize);
            pwrite(topic->dataFile, msg, msgsize, off);

            // now write the message info before updating the header
            pwrite(topic->indexFile, (const char *)(&info), sizeof(info), topic->currIndex * sizeof(KLMessageMetadata));
        } else {
            // write the message
            lseek(topic->dataFile, topic->currOffset, SEEK_SET);
            write(topic->dataFile, (const char *)(&msgsize), sizeof(msgsize));
            write(topic->dataFile, msg, msgsize);

            // now write the message info before updating the header
            lseek(topic->indexFile, topic->currIndex * sizeof(KLMessageMetadata), SEEK_SET);
            write(topic->indexFile, (const char *)(&info), sizeof(info));
        }

        topic->currIndex++;
        topic->currOffset += payloadSize;

        // now write the header in the metadata file
        lseek(topic->metadataFile, 0, SEEK_SET);
        write(topic->metadataFile, &topic->currIndex, sizeof(topic->currIndex));
        write(topic->metadataFile, &topic->currOffset, sizeof(topic->currOffset));

        unflushedLength = (size_t)(topic->currOffset - topic->flushedAtOffset);
    }

    // if we have not flushed in while then do it
    if (unflushedLength >= topic->flushThreshold)
        kl_topic_flush(topic);
    return topic->currOffset;
}

/**
 * Ensures a topic is flushed and persisted.
 */
void kl_topic_flush(KLTopic *topic)
{
    if (topic->dataBuffer)
    {
        size_t dataLength = kl_buffer_size(topic->dataBuffer);
        if (dataLength > 0)
        {
            lseek(topic->dataFile, topic->flushedAtOffset, SEEK_SET);
            write(topic->dataFile, kl_buffer_bytes(topic->dataBuffer), dataLength);
            kl_buffer_reset(topic->dataBuffer);

            size_t indexLength = kl_buffer_size(topic->indexBuffer);
            lseek(topic->indexFile, topic->flushedAtIndex * sizeof(KLMessageMetadata), SEEK_SET);
            write(topic->indexFile, kl_buffer_bytes(topic->indexBuffer), indexLength);
            kl_buffer_reset(topic->indexBuffer);

            // now write the header in the index file
            lseek(topic->metadataFile, 0, SEEK_SET);
            write(topic->metadataFile, &topic->currIndex, sizeof(topic->currIndex));
            write(topic->metadataFile, &topic->currOffset, sizeof(topic->currOffset));

            fsync(topic->dataFile);
            fsync(topic->indexFile);
            fsync(topic->metadataFile);
        }
    } else if (topic->flushedAtIndex < topic->currIndex) {
        fsync(topic->dataFile);
        fsync(topic->indexFile);
        fsync(topic->metadataFile);
    }
    topic->flushedAtIndex = topic->currIndex;
    topic->flushedAtOffset = topic->currOffset;
}

/**
 * Returns the number of messages in this topic.
 */
size_t kl_topic_message_count(KLTopic *topic)
{
    if (!topic)
        return 0;
    KL_MUTEX_LOCK(topic->context->lockManager, topic->filePosLock);
    off_t out = topic->currIndex;
    KL_MUTEX_UNLOCK(topic->context->lockManager, topic->filePosLock);
    return (size_t)out;
}

/**
 * Get the info about count number of messages starting from a particular message index.
 * The output buffer "out" must point to a buffer that has enough space for
 * outCount KLMessageMetadata objects.
 *
 * Returns the number of message metadata objects fetched/available.
 */
size_t kl_topic_get_message_metadata(KLTopic *topic, off_t index, KLMessageMetadata *out, size_t outCount)
{
    KLContextRef context = topic->context;
    KL_MUTEX_LOCK(context->lockManager, topic->filePosLock);
    off_t endIndex = outCount + index;
    if (endIndex > topic->currIndex)
    {
        endIndex = topic->currIndex;
        outCount = (size_t)(endIndex - index);
    }

    size_t totalOutCount = 0;
    off_t index1 = index;
    off_t index2 = topic->flushedAtIndex;
    if (index2 > endIndex)
        index2 = endIndex;
    if (index1 < index2)
    {
        // index1 to index2 is on disk so read from disk
        outCount = (size_t)(index2 - index1);
        kl_read_file(topic, topic->indexFile,
                     out, outCount * sizeof(KLMessageMetadata),
                     index1 * sizeof(KLMessageMetadata));
        out += outCount; //  * sizeof(KLMessageMetadata);
        totalOutCount += outCount;
    } else {
        index2 = index1;
    }

    // index2 to index3 is on the cache
    off_t index3 = endIndex;
    if (index2 > topic->flushedAtIndex && index2 < index3)
    {
        outCount = (size_t)(index3 - index2);
        kl_buffer_copy(topic->indexBuffer,
                        (index2 - topic->flushedAtIndex) * sizeof(KLMessageMetadata),
                        (char *)out, outCount * sizeof(KLMessageMetadata));
        totalOutCount += outCount;
    }

    KL_MUTEX_UNLOCK(context->lockManager, topic->filePosLock);

    return totalOutCount;
}

size_t kl_topic_get_messages(KLTopic *topic, KLMessageMetadata *const headers, size_t numMessages, KLMessage *outMessages)
{
    KLContextRef context = topic->context;
    KL_MUTEX_LOCK(context->lockManager, topic->filePosLock);

    // calculate the total read size and do one big read instead of multiple small ones
    size_t totalReadSize = numMessages * sizeof(KLMessage);
    for (off_t i = 0;i < numMessages;i++)
        totalReadSize += headers[i].size;

    KLMessageMetadata *const firstMsgHeader = headers;
    off_t endOffset = firstMsgHeader->offset + totalReadSize;
    if (endOffset > topic->currOffset)
            endOffset = topic->currOffset;

    off_t offset1 = firstMsgHeader->offset;
    off_t offset2 = topic->flushedAtOffset;
    if (offset2 > endOffset)
        offset2 = endOffset;;

    char *outBuffer = (char *)outMessages;
    if (offset1 < offset2)
    {
        // read the first bit from the file
        kl_read_file(topic, topic->dataFile, outBuffer, (size_t)(offset2 - offset1), offset1);
        outBuffer += (offset2 - offset1);
    } else {
        offset2 = offset1;
    }

    // read the rest from the buffer
    off_t offset3 = endOffset;
    if (offset2 < offset3)
    {
        kl_buffer_copy(topic->dataBuffer, offset2 - topic->flushedAtOffset, outBuffer, (size_t)(offset3 - offset2));
    }

    KL_MUTEX_UNLOCK(context->lockManager, topic->filePosLock);
    return 0;
}

/**
 * Finds a topic by name and returns its index.
 */
off_t kl_topic_find(KLContextRef context, const char *name)
{
    KL_MUTEX_LOCK(context->lockManager, context->topicsMutex);
    off_t index = kl_topic_find_unlocked(context, name);
    KL_MUTEX_UNLOCK(context->lockManager, context->topicsMutex);
    return index;
}

off_t kl_topic_find_unlocked(KLContextRef context, const char *name)
{
    for (off_t i = 0, count = kl_array_count(context->topics);i < count;i++)
    {
        KLTopic *topic = (KLTopic *)kl_array_element_at(context->topics, i);
        if (strcmp(topic->name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

