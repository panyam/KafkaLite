
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
	// topic->dataBuffer = kl_buffer_new(0);
	// topic->indexBuffer = kl_buffer_new(0);
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
	sprintf(buffer, "%s/%s.index", context->topicsDir, name);
	topic->indexFile = open(buffer, O_RDWR | O_CREAT, 0777);
	if (topic->indexFile < 0)
	{
		kl_log("Unable to open index file. error: %s", strerror(errno));
		return ;
	}

	// from the index file gather the number of messages and the num
	lseek(topic->indexFile, 0, SEEK_SET);
	read(topic->indexFile, &topic->numMessages, sizeof(topic->numMessages));
	read(topic->indexFile, &topic->currOffset, sizeof(topic->currOffset));
	// go to where we can start writing to index file
	lseek(topic->indexFile, topic->numMessages * sizeof(KLMessageInfo), SEEK_CUR);

	// now go to the point where we can start writing data
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
		// kl_buffer_destroy(topic->dataBuffer);
		// kl_buffer_destroy(topic->indexBuffer);
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

		KL_RWLOCK_WRLOCK(context->mutexFactory, context->rwLock);
		KLMessageInfo info;
		info.offset = topic->currOffset;
		info.index = topic->numMessages;
		info.size = msgsize;
		info.timestamp = 0x10101010;

		// write the actual data to the file
		write(topic->dataFile, (const char *)(&msgsize), sizeof(msgsize));
		write(topic->dataFile, msg, msgsize);
		// sync the changes
		fsync(topic->dataFile);

		// write the message info first before updating the header
		size_t headerSize = sizeof(topic->numMessages) + sizeof(topic->currOffset);
		lseek(topic->indexFile, headerSize + (topic->numMessages * sizeof(KLMessageInfo)), SEEK_SET);
		write(topic->indexFile, (const char *)(&info), sizeof(info));
		// sync the changes
		fsync(topic->indexFile);

		// now write the header in the index file
		size_t newNumMessages = topic->numMessages + 1;
		size_t newOffset = sizeof(msgsize) + msgsize + topic->currOffset;
		lseek(topic->indexFile, 0, SEEK_SET);
		write(topic->indexFile, &newNumMessages, sizeof(newNumMessages));
		write(topic->indexFile, &newOffset, sizeof(newOffset));
		fsync(topic->indexFile);

		topic->numMessages = newNumMessages;
		topic->currOffset = newOffset;

		// now see if these needs to be synced
		KL_RWLOCK_UNLOCK(context->mutexFactory, context->rwLock);
	}
}

int kl_topic_message_count(KLTopic *topic)
{
	if (!topic)
		return 0;
	KL_RWLOCK_RDLOCK(topic->context->mutexFactory, topic->context->rwLock);
	int out = topic->numMessages;
	KL_RWLOCK_UNLOCK(topic->context->mutexFactory, topic->context->rwLock);
	return out;
}

