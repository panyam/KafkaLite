
#include "klprivate.h"

#define ltell(fdes)		lseek((fdes), 0, SEEK_CUR)

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
	topic->numMessages = 0;
	topic->currOffset = 0;
	topic->dataBuffer = kl_buffer_new(0);
	topic->indexBuffer = kl_buffer_new(0);
	topic->flushThreshold = 100000;		// flush from memory to disk once data in mem exceeds this
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
	read(topic->metadataFile, &topic->numMessages, sizeof(topic->numMessages));
	read(topic->metadataFile, &topic->currOffset, sizeof(topic->currOffset));

	// go to where we can start writing to index file 
	// - only need to do this once per open
	lseek(topic->indexFile, topic->numMessages * sizeof(KLMessageInfo), SEEK_SET);

	// now go to the point where we can start writing data
	// - only need to do this once per open
	lseek(topic->dataFile, topic->currOffset, SEEK_SET);
	kl_log("\nOpening Topic (%s), Offset: %ld, Num Messages: %ld", topic->name, topic->currOffset, topic->numMessages);
}

void kl_topic_finalize(KLTopic *topic)
{
	if (topic)
	{
		topic->context = NULL;
		topic->refCount = 0;
		topic->numMessages = 0;
		topic->currOffset = 0;
		free(topic->name); topic->name = NULL;
		kl_buffer_destroy(topic->dataBuffer);
		kl_buffer_destroy(topic->indexBuffer);
		if (topic->dataFile != 0) close(topic->dataFile); topic->dataFile = 0;
		if (topic->indexFile != 0) close(topic->indexFile); topic->indexFile = 0;
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

void kl_topic_publish(KLTopic *topic, const char *msg, size_t msgsize)
{
	if (topic)
	{
		KLContextRef context = topic->context;

		KL_RWLOCK_WRLOCK(context->mutexFactory, context->topicRWLock);
		KLMessageInfo info;
		info.offset = topic->currOffset;
		info.size = msgsize;

		size_t newNumMessages = topic->numMessages + 1;
		size_t newOffset = sizeof(msgsize) + msgsize + topic->currOffset;
		// info.index = topic->numMessages;
		// info.timestamp = 0x10101010;

		// write the actual data to the file
		if (topic->dataBuffer)
		{
			// write to the buffer first
			kl_buffer_append(topic->dataBuffer, (const char *)(&msgsize), sizeof(msgsize));
			kl_buffer_append(topic->dataBuffer, msg, msgsize);

			// write the message info first before updating the header
			kl_buffer_append(topic->indexBuffer, (const char *)(&info), sizeof(info));

			size_t dataLength = kl_buffer_size(topic->dataBuffer);
			if (dataLength > topic->flushThreshold)
			{
				// then flush it
				write(topic->dataFile, kl_buffer_bytes(topic->dataBuffer), dataLength);
				kl_buffer_reset(topic->dataBuffer);

				size_t indexLength = kl_buffer_size(topic->indexBuffer);
				write(topic->indexFile, kl_buffer_bytes(topic->indexBuffer), indexLength);
				kl_buffer_reset(topic->indexBuffer);

				// now write the header in the index file
				lseek(topic->metadataFile, 0, SEEK_SET);
				write(topic->metadataFile, &newNumMessages, sizeof(newNumMessages));
				write(topic->metadataFile, &newOffset, sizeof(newOffset));
			}
		} else {
			write(topic->dataFile, (const char *)(&msgsize), sizeof(msgsize));
			write(topic->dataFile, msg, msgsize);

			// write the message info first before updating the header
			write(topic->indexFile, (const char *)(&info), sizeof(info));

			// now write the header in the index file
			lseek(topic->metadataFile, 0, SEEK_SET);
			write(topic->metadataFile, &newNumMessages, sizeof(newNumMessages));
			write(topic->metadataFile, &newOffset, sizeof(newOffset));
		}

		// sync the changes
		// fsync(topic->dataFile);
		// fsync(topic->indexFile);
		// fsync(topic->metadataFile);

		topic->numMessages = newNumMessages;
		topic->currOffset = newOffset;

		// now see if these needs to be synced
		KL_RWLOCK_UNLOCK(context->mutexFactory, context->topicRWLock);
	}
}

int kl_topic_message_count(KLTopic *topic)
{
	if (!topic)
		return 0;
	KL_RWLOCK_RDLOCK(topic->context->mutexFactory, topic->context->topicRWLock);
	int out = topic->numMessages;
	KL_RWLOCK_UNLOCK(topic->context->mutexFactory, topic->context->topicRWLock);
	return out;
}

