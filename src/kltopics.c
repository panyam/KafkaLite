
#include "klprivate.h"

#define ltell(fdes)		lseek((fdes), 0, SEEK_CUR)

void kl_topic_flush(KLTopic *topic);
void kl_topic_publish_single(KLTopic *topic, const char *msg, size_t msgsize);

int kl_topic_find_unlocked(KLContextRef context, const char *name)
{
	for (int i = 0, count = kl_array_count(context->topics);i < count;i++)
	{
		KLTopic *topic = (KLTopic *)kl_array_element_at(context->topics, i);
		if (strcmp(topic->name, name) == 0)
		{
			return i;
		}
	}
	return -1;
}

void kl_topic_initialize(KLContext *context, KLTopic *topic, const char *name)
{
	topic->context = context;
	topic->name = strdup(name);
	topic->refCount = 0;
	topic->currIndex = 0;
	topic->currOffset = 0;
	topic->dataBuffer = kl_buffer_new(0);
	topic->indexBuffer = kl_buffer_new(0);
	topic->flushThreshold = 100000;		// flush from memory to disk once data in mem exceeds this
	topic->filePosLock = KL_MUTEX_NEW(context->mutexFactory, NULL);

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
	lseek(topic->metadataFile, 0, SEEK_SET);
	read(topic->metadataFile, &topic->currIndex, sizeof(topic->currIndex));
	read(topic->metadataFile, &topic->currOffset, sizeof(topic->currOffset));

	// go to where we can start writing to index file 
	// - only need to do this once per open
	lseek(topic->indexFile, topic->currIndex * sizeof(KLMessageHeader), SEEK_SET);

	// now go to the point where we can start writing data
	// - only need to do this once per open
	lseek(topic->dataFile, topic->currOffset, SEEK_SET);
	kl_log("\nOpening Topic (%s), Offset: %ld, Num Messages: %ld", topic->name, topic->currOffset, topic->currIndex);
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
		KL_MUTEX_DESTROY(context->mutexFactory, topic->filePosLock);
	}
}

/**
 * Finds a topic by name and returns its index.
 */
int kl_topic_find(KLContextRef context, const char *name)
{
	KL_MUTEX_LOCK(context->mutexFactory, context->topicsMutex);
	int index = kl_topic_find_unlocked(context, name);
	KL_MUTEX_UNLOCK(context->mutexFactory, context->topicsMutex);
	return index;
}

/**
 * Opens a new topic into which messages can be published.
 * If a topic is opened multiple times then the same topic
 * object is returned.
 */
KLTopic *kl_topic_open(KLContextRef context, const char *name)
{
	KL_MUTEX_LOCK(context->mutexFactory, context->topicsMutex);

	// see if this topic is already opened
	// if so then just increment its count
	KLTopic *topic = NULL;
	int index = kl_topic_find_unlocked(context, name);
	if (index < 0)
	{
		topic = kl_array_insert_at(context->topics, -1);
		kl_topic_initialize(context, topic, name);
	} else {
		topic = kl_array_element_at(context->topics, index);
	}
	topic->refCount++;

	KL_MUTEX_UNLOCK(context->mutexFactory, context->topicsMutex);
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
	KL_MUTEX_LOCK(context->mutexFactory, context->topicsMutex);
	// see if this topic is already opened
	// if so then just increment its count
	int index = kl_topic_find_unlocked(context, topic->name);
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
	KL_MUTEX_UNLOCK(context->mutexFactory, context->topicsMutex);
	return alive;
}

/**
 * Publish a single message.
 */
void kl_topic_publish(KLTopic *topic, const char *msg, size_t msgsize)
{
	if (!topic) return ;
	KLContextRef context = topic->context;
	KL_MUTEX_LOCK(context->mutexFactory, topic->filePosLock);
	kl_topic_publish_single(topic, msg, msgsize);
	KL_MUTEX_UNLOCK(context->mutexFactory, topic->filePosLock);
}

/**
 * Publish a batch of messages to the topic.
 */
void kl_topic_publish_multi(KLTopic *topic, int numMessages, const char *msgs[], size_t *msgsizes)
{
	if (!topic) return ;
	KLContextRef context = topic->context;
	KL_MUTEX_LOCK(context->mutexFactory, topic->filePosLock);
	for (int i = 0;i < numMessages;i++)
	{
		kl_topic_publish_single(topic, msgs[i], msgsizes[i]);
	}
	KL_MUTEX_UNLOCK(context->mutexFactory, topic->filePosLock);
}

void kl_topic_publish_single(KLTopic *topic, const char *msg, size_t msgsize)
{
	KLMessageHeader info;
	info.offset = topic->currOffset;
	info.size = msgsize;

	size_t payloadSize = sizeof(msgsize) + msgsize;
	size_t unflushedLength = 0;

	if (topic->dataBuffer)
	{
		// write to the buffer first
		kl_buffer_append(topic->dataBuffer, (const char *)(&msgsize), sizeof(msgsize));
		kl_buffer_append(topic->dataBuffer, msg, msgsize);

		// write the message info first before updating the header
		kl_buffer_append(topic->indexBuffer, (const char *)(&info), sizeof(info));

		topic->currIndex++;
		topic->currOffset += payloadSize;

		unflushedLength = kl_buffer_size(topic->dataBuffer);
	} else {
		// write the message
		lseek(topic->dataFile, topic->currOffset, SEEK_SET);
		write(topic->dataFile, (const char *)(&msgsize), sizeof(msgsize));
		write(topic->dataFile, msg, msgsize);

		// now write the message info before updating the header
		lseek(topic->indexFile, topic->currIndex * sizeof(KLMessageHeader), SEEK_SET);
		write(topic->indexFile, (const char *)(&info), sizeof(info));

		topic->currIndex++;
		topic->currOffset += payloadSize;

		// now write the header in the metadata file
		lseek(topic->metadataFile, 0, SEEK_SET);
		write(topic->metadataFile, &topic->currIndex, sizeof(topic->currIndex));
		write(topic->metadataFile, &topic->currOffset, sizeof(topic->currOffset));

		unflushedLength = topic->currOffset - topic->flushedAtOffset;
	}

	// if we have not flushed in while then do it
	if (unflushedLength > topic->flushThreshold)
		kl_topic_flush(topic);
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
			write(topic->dataFile, kl_buffer_bytes(topic->dataBuffer), dataLength);
			kl_buffer_reset(topic->dataBuffer);

			size_t indexLength = kl_buffer_size(topic->indexBuffer);
			write(topic->indexFile, kl_buffer_bytes(topic->indexBuffer), indexLength);
			kl_buffer_reset(topic->indexBuffer);

			// now write the header in the index file
			lseek(topic->metadataFile, 0, SEEK_SET);
			write(topic->metadataFile, &topic->currIndex, sizeof(topic->currIndex));
			write(topic->metadataFile, &topic->currOffset, sizeof(topic->currOffset));

		}
	}
	topic->flushedAtIndex = topic->currIndex;
	topic->flushedAtOffset = topic->currOffset;
}

/**
 * Returns the number of messages in this topic.
 */
int kl_topic_message_count(KLTopic *topic)
{
	if (!topic)
		return 0;
	KL_MUTEX_LOCK(topic->context->mutexFactory, topic->filePosLock);
	int out = topic->currIndex;
	KL_MUTEX_UNLOCK(topic->context->mutexFactory, topic->filePosLock);
	return out;
}

/**
 * Get the info about count number of messages starting from a particular message index.
 * The output buffer "out" must point to a buffer that has enough space for
 * outCount KLMessageHeader objects.
 */
int kl_topic_get_message_info(KLTopic *topic, int index, KLMessageHeader *out, int outCount)
{
	KLContextRef context = topic->context;
	KL_MUTEX_LOCK(context->mutexFactory, topic->filePosLock);
	int endIndex = outCount + index;
	if (endIndex >= topic->currIndex)
	{
		outCount -= (1 + endIndex - topic->currIndex);
		endIndex = outCount + index;
	}

	if (endIndex > topic->flushedAtIndex)
	{
		// flush if consumption requires a simple read (keeps it simple)
		kl_topic_flush(topic);
	}
	// see what the size is
	lseek(topic->indexFile, index * sizeof(KLMessageHeader), SEEK_SET);
	// read the message info
	read(topic->indexFile, out, outCount * sizeof(KLMessageHeader));
	KL_MUTEX_UNLOCK(context->mutexFactory, topic->filePosLock);
	return outCount;
}

int kl_topic_get_messages(KLTopic *topic, KLMessageHeader *firstMessage, int numMessages, KLMessage *outMessages)
{
	KLContextRef context = topic->context;
	KL_MUTEX_LOCK(context->mutexFactory, topic->filePosLock);
	lseek(topic->dataFile, firstMessage->offset, SEEK_SET);
	// TODO: see if it is just faster to read 
	// sum(firstMessage[0..numMessages].offset) + numMessages * sizeof(size_t)
	// bytes in one go into outMessages array
	for (int i = 0;i < numMessages;i++)
	{
		KLMessage *message = outMessages + i;
		read(topic->dataFile, message, sizeof(size_t));
		read(topic->dataFile, message->data, message->size);
	}
	KL_MUTEX_UNLOCK(context->mutexFactory, topic->filePosLock);
	return 0;
}

