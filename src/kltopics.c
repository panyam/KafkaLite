
#include "klprivate.h"

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
		if (topic->dataFile != NULL) fclose(topic->dataFile); topic->dataFile = NULL;
		if (topic->indexFile != NULL) fclose(topic->indexFile); topic->indexFile = NULL;
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
		kl_buffer_append(topic->dataBuffer, (const char *)(&msgsize), sizeof(msgsize));
		kl_buffer_append(topic->dataBuffer, msg, msgsize);
		kl_buffer_append(topic->indexBuffer, (const char *)(&info), sizeof(info));
		topic->numMessages++;
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

